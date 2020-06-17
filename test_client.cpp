#include "albit.h"
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

void *get_shared(const char *name, int size)
{
    int fd = shm_open(name, O_RDONLY, 0666);
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


int main(int argc, char ** argv)
{

    if(argc <= 1) return 1;
    
    int type, id;

    if (strcmp(argv[1], "article") == 0)
    {
        type = 1;
        if (argc != 3)
            return 2;
        id = atoi(argv[2]);
    }
    else if (strcmp(argv[1], "search") == 0)
    {
        if (argc < 3)
            return 3;
        type = 2;
    }
    else
    {
        return 4;
    }

    int *sizes = (int*)get_shared("albit.sizes", 4*5);
    if(sizes == nullptr) return 5;

    lists = (int*)get_shared("albit.lists", sizes[0]);
    if(lists == nullptr) return 6;

    basic = (BASICNODE*)get_shared("albit.trie_basic", sizes[1]);
    if(basic == nullptr) return 7;

    multi = (MULTINODE*)get_shared("albit.trie_multi", sizes[2]);
    if(multi == nullptr) return 8;

    titles_data = (Title_Data*)get_shared("albit.titles_data", sizes[3]);
    if(titles_data == nullptr) return 9;

    titles_names = (char*)get_shared("albit.titles_names", sizes[4]);
    if(titles_names == nullptr) return 10;

    if (type == 1)
    {

        vector<byte> art = Load_Article(id, titles_data);
        for(byte b : art)
            Print_UTF8(b);
    
    } 
    else 
    {

        // TODO - more words
        char *word = argv[2];
        start();
        int list = find(word);
        float t_search = stop();
        printf("%f\n", (t_search * 1000));

        int results = lists[list*2];
        int start = lists[list*2+1];
        
        for(int i = 0; i < results; i++)
        {
            int __ID__ = lists[start+i];
            char *t = &titles_names[titles_data[__ID__].offset];
            printf("%d => ", __ID__);
            for(; *t != 0xa; t++) Print_UTF8(*t);
            printf("\n");
        }

    }

    return 0;
    
}