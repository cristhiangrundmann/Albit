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

enum {
    C_0, C_1, C_2, C_3,
    C_4, C_5, C_6, C_7,
    C_8, C_9,
    
    C_A, C_B, C_C, C_D, C_E,
    C_F, C_G, C_H, C_I, C_J,
    C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S,
    C_T, C_U, C_V, C_W,
    C_X, C_Y, C_Z,

    NON, END
};

int ISO_8859[256] = {
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    C_0, C_1, C_2, C_3, C_4, C_5, C_6, C_7, C_8, C_9, NON, NON, NON, NON, NON, NON,
    C_A, C_A, C_B, C_C, C_D, C_E, C_F, C_G, C_H, C_I, C_J, C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S, C_T, C_U, C_V, C_W, C_X, C_Y, C_Z, NON, NON, NON, NON, NON,
    NON, C_A, C_B, C_C, C_D, C_E, C_F, C_G, C_H, C_I, C_J, C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S, C_T, C_U, C_V, C_W, C_X, C_Y, C_Z, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, C_C, C_L, C_C, C_Y, C_S, NON, C_S, C_C, C_A, NON, NON, NON, C_R, NON,
    C_O, NON, C_2, C_3, C_Z, C_U, NON, NON, C_Z, C_1, C_O, NON, C_O, C_O, C_Y, NON,
    C_A, C_A, C_A, C_A, C_A, C_A, C_A, C_C, C_E, C_E, C_E, C_E, C_I, C_I, C_I, C_I,
    C_D, C_N, C_O, C_O, C_O, C_O, C_O, C_X, C_O, C_U, C_U, C_U, C_U, C_Y, C_P, C_S,
    C_A, C_A, C_A, C_A, C_A, C_A, C_A, C_C, C_E, C_E, C_E, C_E, C_I, C_I, C_I, C_I,
    C_D, C_N, C_O, C_O, C_O, C_O, C_O, NON, C_O, C_U, C_U, C_U, C_U, C_Y, C_P, C_Y
};


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

struct timespec t_start, t_end;
void start()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_start);   
}

float stop()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
    return ((t_end.tv_sec - t_start.tv_sec) * 1000000000 + 
    (t_end.tv_nsec - t_start.tv_nsec)) / 1000000000.f;
}

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

    for(; *c != ' '; c++)
    {
        int i;
        i = (char)ISO_8859[(unsigned char)*c];

        if(i == NON) continue;

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

    if(n == 0)
    {
        return;
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

struct Title_Data {

    uint32_t offset, database, database_offset;

} *titles_data;

char *titles_names;

int main()
{

    NODE root;
    trie.push_back(root);

    start();

    for(int i = 0; i < 10; i++)
    {
        printf("Processing database 'db%d'...\n", i);
        char filename[24];
        sprintf(filename, "./database/db%d", i);
        process_file(filename);
    }

    float t_prep = stop();

    printf("Pre-processing time: %f\n", t_prep);

    ifstream ifs("titles/titles_data", std::ios::binary);
    if(!ifs.is_open()) return 1;
    ifs.seekg (0, ifs.end);
    long size = ifs.tellg();
    ifs.seekg(0);
    titles_data = (Title_Data*)(new char[size]);
    if(!titles_data) return 2;
    ifs.read((char*)titles_data, size);
    ifs.close();

    ifs.open("titles/titles_names", std::ios::binary);
    if(!ifs.is_open()) return 1;
    ifs.seekg (0, ifs.end);
    size = ifs.tellg();
    ifs.seekg(0);
    titles_names = new char[size];
    if(!titles_names) return 2;
    ifs.read(titles_names, size);
    ifs.close();

    while(1)
    {
        char word[32];
        printf("\033[0;34m"); 
        printf("Search >> ");
        printf("\033[1;34m"); 
        std::cin >> word;
        start();

        int n = 0;
        char good = 1;
        for(char *c = word; *c; c++)
        {
            int i;
            i = ISO_8859[(unsigned char)*c];
            if(i == NON) continue;

            int k = trie[n].next[i];

            if(k == -1) 
            {
                good = 0;
                break;
            }
            n = k;
        }

        int list = trie[n].list;
        if(list < 0) good = 0;

        float t_search = stop();
        
        if(good)
        {
            int page = 20;
            printf("\033[1;32m"); 
            printf("Found %ld results (%f seconds)\n", lists[trie[n].list].size(), t_search);
            for(int i = 0; i < lists[trie[n].list].size(); i++)
            {
                int __ID__ = lists[trie[n].list][i];
                printf("\033[0;32m");
                printf("Result %d: \033[0m%s\n", i+1, &titles_names[titles_data[__ID__].offset]);
                page--;
                if(page == 0)
                {
                    ASK_AGAIN:
                    printf("\033[0;33m"); 
                    printf("Do you want more results? (Y/N)\033[0m\n");
                    char yn;
                    std::cin >> yn;
                    int i = ISO_8859[(unsigned char)yn];
                    if(i == C_Y) page = 20;
                    else if(i == C_N) break;
                    else goto ASK_AGAIN;
                }
            }
        }
        else printf("\033[0;31mNo matches\n");
    }

    return 0;
}