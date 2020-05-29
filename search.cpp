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

int *lists;
MULTINODE *multi;
BASICNODE *basic;

int find(char *str) //ASCII
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
            if(multi[n].next[*str] != 0) 
            {
                n = multi[n].next[*str];
                if(n >= 0) list = multi[n].list;
                else list = basic[-n-1].list;
            }
            else return -1;
        }
        else
        {
            int b = -n-1;
            char c = basic[b].c;
            if(c != *str) return -1;
            n = basic[b].next;
            if(n >= 0) list = multi[n].list;
            else list = basic[-n-1].list;
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

struct Title_Data {

    uint32_t offset, database, database_offset;

} *titles_data;

char *titles_names;

typedef uint8_t byte;

const byte error[] = {0xF0, 0x9F, 0x96, 0x95, 0xEF, 0xB8, 0x8F, 0x00};

void Print_UTF8(const char* word) {

    const byte* array = (const byte*)word;

    for(; *array != 0; array++) {
        if(*array == 0x0a) return;

        if (*array > 0x7F) {
            // C2
            if (*array >= 0xA0 && *array < 0xC0) {
                switch(*array) {
                    case(0xA0):
                        printf(" ");
                        continue;
                    case(0xA4):
                        printf("%c%c%c", 0xE2, 0x82, 0xAC);
                        continue;
                    case(0xA6):
                        printf("%c%c", 0xC5, 0xA0);
                        continue;
                    case(0xA8):
                        printf("%c%c", 0xC5, 0xA1);
                        continue;
                    case(0xB4):
                        printf("%c%c", 0xC5, 0xBD);
                        continue;
                    case(0xB8):
                        printf("%c%c", 0xC5, 0xBE);
                        continue;
                    case(0xBC):
                        printf("%c%c", 0xC5, 0x92);
                        continue;
                    case(0xBD):
                        printf("%c%c", 0xC5, 0x93);
                        continue;
                    case(0xBE):
                        printf("%c%c", 0xC5, 0xB8);
                        continue;
                    default:
                        printf("%c%c", 0xC2, *array);
                        continue;
                }
            }
            //C3
            if (*array >= 0xC0 && *array <= 0xFF) {
                printf("%c%c", 0xC3, (*array) - 0x40);
                continue;
            }
            //ERROR
            printf("%s", error);
            continue;
        }

        printf("%c", *array);
    }
}


int main()
{
    start();   
    FILE *ftrie = fopen("trie/trie_multi", "rb");
    fseek(ftrie, 0, SEEK_END);
    long fsize = ftell(ftrie);
    fseek(ftrie, 0, SEEK_SET);
    multi = (MULTINODE*)malloc(fsize);
    fread(multi, fsize, 1, ftrie);
    fclose(ftrie);

    ftrie = fopen("trie/trie_basic", "rb");
    fseek(ftrie, 0, SEEK_END);
    fsize = ftell(ftrie);
    fseek(ftrie, 0, SEEK_SET);
    basic = (BASICNODE*)malloc(fsize);
    fread(basic, fsize, 1, ftrie);
    fclose(ftrie);


    ftrie = fopen("trie/lists", "rb");
    fseek(ftrie, 0, SEEK_END);
    fsize = ftell(ftrie);
    fseek(ftrie, 0, SEEK_SET);
    lists = (int*)malloc(fsize);
    fread(lists, fsize, 1, ftrie);
    fclose(ftrie);

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

    printf("Load time: %f seconds\n", stop());

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
        
        if(list >= 0)
        {
            int page = 20;
            printf("\033[1;32m");
            int results = lists[list*2];
            int start = lists[list*2+1];
            printf("Found %d results (%f seconds)\n", results, t_search);
            for(int i = 0; i < results; i++)
            {
                int __ID__ = lists[start+i];
                printf("\033[0;32m");
                printf("Result %d: \033[0m", i+1);
                Print_UTF8(&titles_names[titles_data[__ID__].offset]);
                printf("\n");
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