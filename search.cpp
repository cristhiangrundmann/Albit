#include "albit.h"

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

char *titles_names;
Title_Data *titles_data;

int main()
{
    start();   
    multi = (MULTINODE*)Load_on_RAM(string("trie/trie_multi"));
    basic = (BASICNODE*)Load_on_RAM(string("trie/trie_basic"));
    lists = (int*)Load_on_RAM(string("trie/lists"));
    titles_data = (Title_Data*)Load_on_RAM(string("titles/titles_data"));
    titles_names = (char*)Load_on_RAM(string("titles/titles_names"));
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