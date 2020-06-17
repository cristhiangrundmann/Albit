#include "albit.h"
#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

void *new_shared(const char *name, int size)
{
    int fd = shm_open(name, O_RDWR | O_CREAT, 0666);
    if(fd == -1) return nullptr;
    if(ftruncate(fd, size) == -1) return nullptr;
    void *m = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED) return nullptr;
    return m;
}

void *load_shared(const char *filename, const char *name, int *size)
{
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    int s = ftell(fp);
    if(size) *size = s;
    fseek(fp, 0, SEEK_SET);
    void *data = new_shared(name, s);
    if(data == nullptr) return nullptr;
    if(1 != fread(data, s, 1, fp))
    {
        free(data);
        fclose(fp);
        return nullptr;
    }
    fclose(fp);
    return data;
}

int *lists;
MULTINODE *multi;
BASICNODE *basic;
char *titles_names;
Title_Data *titles_data;

int *sizes;

void clean()
{

    munmap(lists, sizes[0]);
    munmap(basic, sizes[1]);
    munmap(multi, sizes[2]);
    munmap(titles_data, sizes[3]);
    munmap(titles_names, sizes[4]);
    shm_unlink("albit.sizes");
    shm_unlink("albit.lists");
    shm_unlink("albit.trie_basic");
    shm_unlink("albit.trie_multi");
    shm_unlink("albit.titles_data");
    shm_unlink("albit.titles_names");

}

sig_atomic_t stopFlag = 0;

void handler(int)
{
    stopFlag = 1;
}

int main()
{
    sizes = (int*)new_shared("albit.sizes", 4*5);
    if(sizes == nullptr) return -1;

    int cur_size;
    lists = (int*)load_shared("trie/lists", "albit.lists", &cur_size);
    if(lists == nullptr) return -1;
    sizes[0] = cur_size;
    
    basic = (BASICNODE*)load_shared("trie/trie_basic", "albit.trie_basic", &cur_size);
    if(basic == nullptr) return -1;
    sizes[1] = cur_size;

    multi = (MULTINODE*)load_shared("trie/trie_multi", "albit.trie_multi", &cur_size);
    if(multi == nullptr) return -1;
    sizes[2] = cur_size;

    titles_data = (Title_Data*)load_shared("titles/titles_data", "albit.titles_data", &cur_size);
    if(titles_data == nullptr) return -1;
    sizes[3] = cur_size;

    titles_names = (char*)load_shared("titles/titles_names", "albit.titles_names", &cur_size);
    if(titles_names == nullptr) return -1;
    sizes[4] = cur_size;

    // Impede de dar ^C sem limpar os shm
    signal(SIGINT, &handler);

    printf("Server ready and running...\n");

    while(stopFlag == 0)
    {
        char c = getchar();
        if(c == 'q') break;
    }

    clean();

    return 0;
}