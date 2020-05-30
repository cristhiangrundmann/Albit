#include "albit.h"
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

void *get_shared(const char *name, int size)
{
    int fd = shm_open(name, O_RDONLY, S_IRUSR | S_IWUSR);
    if(fd == -1) return nullptr;
    void *data =  mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED) return nullptr;
    return data;
}

int *lists;
MULTINODE *multi;
BASICNODE *basic;
char *titles_names;
Title_Data *titles_data;

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



int main()
{
    int *sizes = (int*)get_shared("albit.sizes", 4*5);
    if(sizes == nullptr) return -1;

    printf("0\n");

    lists = (int*)get_shared("albit.lists", sizes[0]);
    if(lists == nullptr) return -1;

    printf("1\n");

    basic = (BASICNODE*)get_shared("albit.trie_basic", sizes[1]);
    if(basic == nullptr) return -1;

    printf("2\n");

    multi = (MULTINODE*)get_shared("albit.trie_multi", sizes[2]);
    if(multi == nullptr) return -1;

    printf("3\n");

    titles_data = (Title_Data*)get_shared("albit.titles_data", sizes[3]);
    if(titles_data == nullptr) return -1;

    printf("4\n");

    titles_names = (char*)get_shared("albit.titles_names", sizes[4]);
    if(titles_names == nullptr) return -1;

    printf("5\n");

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
                char *t = &titles_names[titles_data[__ID__].offset];
                for(; *t != 0xa; t++) Print_UTF8(*t);
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