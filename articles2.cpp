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


char *title;
int id = -1;

void make(int *n)
{
    if(*n != 0) return;

    BASICNODE *bn = new BASICNODE;
    *n = -basic.size()-1;
    basic.push_back(bn);
}

void insert_iso(char*);

void insert(char *str)
{
    char *t = str;

    for(; *t; t++)
    {
        if(*t == ' ')
        {
            *t = END;
            insert_iso(str);
            return;
        }
        *t = ISO_8859[(unsigned char)*t];
        if((unsigned char)*t >= ALPHSIZE)
        {
            *t = END;
            insert_iso(str);
            str = t+1;
        }
    }
}

void insert_iso(char *str)
{    
    if(*str == END) return;

    int *n = &multi[0]->next[*str];
    str++;

    make(n);

    for(; *str != END; str++)
    {
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

int find(char *str)
{
    {
        char *t;
        for(t = str; *t && *t != ' '; t++) *t = ISO_8859[(unsigned char)*t];
        *t = END;
    }

    int n = 0, list = -1;
    for(; *str != END; str++)
    {
        if((unsigned char)*str >= ALPHSIZE) continue;
        if(n >= 0)
        {
            if(multi[n]->next[*str] != 0) 
            {
                n = multi[n]->next[*str];
                if(n >= 0) list = multi[n]->list;
                else list = basic[-n-1]->list;
            }
            else return -1;
        }
        else
        {
            int b = -n-1;
            char c = basic[b]->c;
            if(c != *str) return -1;
            n = basic[b]->next;
            if(n >= 0) list = multi[n]->list;
            else list = basic[-n-1]->list;
        }
    }
    return list;
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

    if(quos != 3) return 0;

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

    insert(line);
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

struct Title_Data {

    uint32_t offset, database, database_offset;

} *titles_data;

char *titles_names;

int main()
{
    multi.push_back(new MULTINODE);

    start();

    for(int i = 0; i < 164; i++)
    {
        printf("Processing database 'db%d'... (%ld BASICNODES\t%ld MULTINODES)\n", i, 
        basic.size(), multi.size());
        char filename[24];
        sprintf(filename, "./database/db%d", i);
        process_file(filename);
    }

    float t_prep = stop();

    printf("Pre-processing time: %f\n", t_prep);

    printf("%d articles\n", id);

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

        int list = find(word);
        
        float t_search = stop();
        
        if(list != -1)
        {
            int page = 20;
            printf("\033[1;32m"); 
            printf("Found %ld results (%f seconds)\n", lists[list].size(), t_search);
            for(int i = 0; i < lists[list].size(); i++)
            {
                int __ID__ = lists[list][i];
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