#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;

#define BUFFERSIZE 0x10000

char line[BUFFERSIZE];
size_t linelen;


struct NODE
{
    int next[10+26];
    int list;
    int num;

    NODE()
    {
        for(int i = 0; i < 10+26; i++) next[i] = -1;
        list = -1;
        num = -1;
    }
};

vector<vector<int>> lists;
vector<NODE> trie;


char *title;
int id = -1;

char process_tags()
{
    static const char *doctag = "<doc ";
    const char *tag = doctag;
    char *cur = line;

    for(; *tag; tag++)
    {
        if(*tag != *cur) return 0;
        cur++;
    }

    int quos = 0;
    

    for(; *cur; cur++)
    if(*cur == '"') 
    {
        quos++;
        if(quos == 3) 
        {
            title = cur+1;
            break;
        }
    }

    if(quos != 3)
    {
        
        return 0;
    }

    quos = 0;

    cur = line + linelen - 1;

    for(; *cur; cur--)
    if(*cur == '"')
    {
        quos++;

        if(quos == 7)
        {
            *cur = 0;
            break;
        }
    }

    if(quos != 7) return 0;

    id++;
    return 1;
}

void process_line()
{

    if(process_tags()) return;

    int n = 0;
    char *c = line;
    if(*c == ' ')
    {
        return;
    }
    for(; *c != ' '; c++)
    {
        int i;
        if(*c >= 'A' && *c <= 'Z') *c |= 32;

        if(*c >= 'a' && *c <= 'z') i = *c - 'a' + 10;
        else if(*c >= '0' && *c <= '9') i = *c - '0';
        else continue;

        int k = trie[n].next[i];

        if(k == -1)
        {
            k = trie.size();
            trie[n].next[i] = k;
            NODE node;
            trie.push_back(node);
        }
        n = k;
    }



    if(trie[n].list == -1)
    {
        trie[n].list = lists.size();
        vector<int> list;
        lists.push_back(list);
    }

    if(lists[trie[n].list].size() > 0) if(lists[trie[n].list].back() == id) return;


    lists[trie[n].list].push_back(id);
    trie[n].num++;
  
}

void process_file(const char *filename)
{
    ifstream db(filename);
    if(!db.is_open()) return;
    string str;
    while(getline(db, str))
    {
        linelen = str.size();
        strcpy(line, str.c_str());
        process_line();
    }
    db.close();

}

int main()
{

    NODE root;
    trie.push_back(root);

    time_t time0 = time(0);

    for(int i = 0; i < 20; i++)
    {
        printf("db%d\n", i);
        char filename[24];
        sprintf(filename, "./database/db%d", i);
        process_file(filename);
    }

    printf("Time = %lu\n", time(0) - time0);

    printf("Agora pesquisa nessa coisa\n");

    while(1)
    {
        char word[32];
        std::cin >> word;

        int n = 0;
        char good = 1;
        for(char *c = word; *c; c++)
        {
            int i;
            if(*c >= 'A' && *c <= 'Z') *c |= 32;

            if(*c >= 'a' && *c <= 'z') i = *c - 'a' + 10;
            else if(*c >= '0' && *c <= '9') i = *c - '0';
            else 
            {
                //good = 0;
                continue;
            }

            int k = trie[n].next[i];

            if(k == -1) 
            {
                good = 0;
                break;
            }
            n = k;
        }

        
        if(good)
        for(int i = 0; i < lists[trie[n].list].size(); i++)
        {
            printf("%d\n", lists[trie[n].list][i]);
        }
    }

    return 0;
}