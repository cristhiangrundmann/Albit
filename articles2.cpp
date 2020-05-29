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

    NON, SKP, END
};

int ISO_8859[256] = {
    END, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, SKP, NON, NON, NON, NON, NON, NON, NON, NON,
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


#define ALPHSIZE 36

struct MULTINODE
{
    int list;
    int next[ALPHSIZE];

    MULTINODE()
    {
        list = -1;
        for(int i = 0; i < ALPHSIZE; i++) next[i] = 0;
    }
};

struct BASICNODE
{
    int list;
    int next;
    char c;

    BASICNODE()
    {
        list = -1;
        next = 0;
        c = -1;
    }
};

vector<vector<int>> lists;
vector<MULTINODE*> multi;
vector<BASICNODE*> basic;

int id = -1;

void make(int *n)
{
    if(*n != 0) return;

    BASICNODE *bn = new BASICNODE;
    *n = -basic.size()-1;
    basic.push_back(bn);
}

void insert(char *str) //ALPHABET
{    
    if(*str == END) return;

    int *n = &multi[0]->next[*str];
    str++;

    make(n);

    for(; *str != END; str++)
    {
        if(*str == SKP) continue;
        if(*n > 0) 
        {
            n = &multi[*n]->next[*str];
            make(n);
        }
        else if(*n < 0)
        {
            int b = -(*n)-1;
            char c = basic[b]->c;
            if(c == *str || c == -1)
            {
                basic[b]->c = *str;
                n = &basic[b]->next;
                make(n);
            }
            else
            {
                MULTINODE *mn = new MULTINODE;
                int k = multi.size();
                multi.push_back(mn);

                mn->next[c] = basic[b]->next;
                mn->list = basic[b]->list;
                
                *n = k;
                n = &mn->next[*str];
                *n = -b-1;
                basic[b]->c = -1;
                basic[b]->list = -1;
                basic[b]->next = 0;
            }
        }
    }

    int *llist;
    if(*n == 0) return;
    if(*n > 0) llist = &multi[*n]->list;
    else llist = &basic[-(*n)-1]->list;

    if(*llist == -1)
    {
        *llist = lists.size();
        vector<int> list;
        lists.push_back(list);
    }

    if(lists[*llist].size() > 0) if(lists[*llist].back() == id) return;

    lists[*llist].push_back(id);
}

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

void process_line()
{
    char *str = line;

    if(linelen >= 4)
    if( *(uint32_t*)line == 0x636f6423 )
    {
        id++;
        str += 5;
    }

    
    for(char *cur = str; cur < line+linelen; cur++)
    {
        *cur = ISO_8859[(unsigned char)*cur];
        if(*cur == NON)
        {
            *cur = END;
            insert(str);
            str = cur+1;
            continue;
        }
    }
}

void process_file(const char *filename)
{
    ifstream db(filename);
    if(!db.is_open())
    {
        printf("Couldn't open %s\n", filename);
        return;
    }
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
    multi.push_back(new MULTINODE);

    start();

    for(int i = 0; i < 164; i++)
    {
        printf("Processing database 'db%d'... (%ld BASICNODES\t%ld MULTINODES)\n", i, 
        basic.size(), multi.size());
        char filename[24];
        sprintf(filename, "./database3/db%d", i);
        process_file(filename);
    }

    float t_prep = stop();

    printf("Pre-processing time: %f\n", t_prep);

    printf("%d articles\n", id);

    //SAVE TRIE
    FILE *ftrie = fopen("trie/trie_multi", "wb");
    for(int i = 0; i < multi.size(); i++)
    {
        fwrite(multi[i], sizeof(MULTINODE), 1, ftrie);
    }
    fclose(ftrie);

    ftrie = fopen("trie/trie_basic", "wb");
    for(int i = 0; i < basic.size(); i++)
    {
        fwrite(basic[i], sizeof(BASICNODE), 1, ftrie);
    }
    fclose(ftrie);

    //SAVE LISTS
    FILE *flist = fopen("trie/lists", "wb");

    int offset = lists.size() * 2;
    for(int i = 0; i < lists.size(); i++)
    {
        int s = lists[i].size();
        fwrite(&s, 4, 1, flist);
        fwrite(&offset, 4, 1, flist);
        offset += s;
    }

    for(int i = 0; i < lists.size(); i++)
    for(int j = 0; j < lists[i].size(); j++)
    {
        fwrite(&(lists[i])[j], 4, 1, flist);
    }

    fclose(flist);
    return 0;
}