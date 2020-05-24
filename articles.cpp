#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>

using std::vector;

#define BUFFERSIZE 0x10000

char buffer_a[BUFFERSIZE + 1];
char buffer_b[BUFFERSIZE + 1];
char buffer_l[BUFFERSIZE + 1];

char *buf_get, *buf_set, *line, span;
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

char make_line()
{
    if(linelen >= BUFFERSIZE)
    {
        printf("Ignoring large line: %ld chars\n", linelen);
        return 0;
    }

    strcpy(buffer_l, line);
    if(span) strcat(buffer_l, buf_get);

    return 1;
}

char *title_str;
int id = -1;

char process_tags()
{
    static const char *doctag = "<doc ";
    const char *tag = doctag;
    char *cur = buffer_l;

    for(; *tag; tag++)
    {
        if(*tag != *cur) return 0;
        cur++;
    }

    int quos = 0;
    
    //char *id_str;

    for(; *cur; cur++)
    if(*cur == '"') 
    {
        quos++;
        if(quos == 3) 
        {
            title_str = cur+1;
            break;
        }
    }

    if(quos != 3) return 0;

    quos = 0;

    cur = buffer_l + linelen - 1;

    for(; *cur; cur--)
    if(*cur == '"')
    {
        quos++;
        //if(quos == 1) *cur = 0;
        //if(quos == 2) id = atoi(cur+1);
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
    if(!make_line()) return;

    if(process_tags()) return;

    int n = 0;
    char *c = buffer_l;
    for(; *c != ' '; c++)
    {
        int i;
        if(*c >= 'A' && *c <= 'Z') *c |= 32;

        if(*c >= 'a' && *c <= 'z') i = *c - 'a' + 10;
        else if(*c >= '0' && *c <= '9') i = *c - '0';
        else return;

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
    //*c = 0;


    if(trie[n].list == -1)
    {
        trie[n].list = lists.size();
        vector<int> list;
        lists.push_back(list);
    }

    if(lists[trie[n].list].size() > 0) if(lists[trie[n].list].back() == id) return;


    lists[trie[n].list].push_back(id);
    trie[n].num++;

    //printf("%s\n", buffer_l);
    //exit(0);    
}

void process_file(const char *filename)
{
    FILE *dbi = fopen(filename, "rb");
    if(!dbi) return;

    buf_get = buffer_a;
    buf_set = buffer_b;

    span = 0;
    line = buf_set;

    size_t len;

    while(len = fread(buf_set, 1, BUFFERSIZE, dbi))
    {
        buf_set[len] = 0;
        char *t = buf_set; buf_set = buf_get; buf_get = t;

        char *cur = buf_get;
        for(; *cur; cur++)
        {
            if(*cur == '\n')
            {
                *cur = 0;
                process_line();
        
                line = cur + 1;
                span = 0;
                linelen = 0;
                continue;
            }
            linelen++;
        }
        span = buf_get[len - 1] != '\n';
    }
    process_line();

    fclose(dbi);
}

int main()
{

    NODE root;
    trie.push_back(root);

    time_t time0 = time(0);

    for(int i = 0; i < 1; i++)
    {
        printf("::%d::", i);
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
                good = 0;
                break;
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