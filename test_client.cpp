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

static vector<byte> article;

static void Load_Article(int id) {

    article.empty();

    Title_Data& doc = titles_data[id];

    ifstream file(string("../database_clean/db" + to_string(doc.database)));
    file.seekg(doc.database_offset);

    bool line_broken = false;

    string line;
    while(getline(file, line)) {
        
        if (line.size() == 0) {
            if (!line_broken)
                article.push_back('\n');
            line_broken = true;
            continue;
        }

        line_broken = false;
        
        if(line.compare(0, 4, "#doc") == 0) break;

        line += '\n';
        article.insert(article.end(), line.begin(), line.end());

    }

    file.close();

}

const byte *find_cur;
int find() //ASCII
{
    int n = 0, list = -1;
    for(; *find_cur != END; find_cur++)
    {
        if(*find_cur == SKP) continue;
        if(*find_cur == NON) break;
        if(n >= 0)
        {
            if(multi[n].next[*find_cur] != 0) 
            {
                n = multi[n].next[*find_cur];
                if(n >= 0) list = multi[n].list;
                else list = basic[-n-1].list;
            }
            else return -1;
        }
        else
        {
            int b = -n-1;
            char c = basic[b].c;
            if(c != *find_cur) return -1;
            n = basic[b].next;
            if(n >= 0) list = multi[n].list;
            else list = basic[-n-1].list;
        }
    }

    return list;
}

bool SIZE_COMPARE(int a, int b)
{
    int sa = lists[2*a], sb = lists[2*b];
    return sa < sb;
}

#define MAX_RESULTS 1300000
int b0[MAX_RESULTS], b1[MAX_RESULTS];
int *mresults = b0, *mtemp = b1;
int mresults_size = 0;

void intersect(int L)
{
    int inter_cur = 0;
    int size = lists[2*L];
    int *big = &lists[lists[2*L+1]];

    for(int i = 0; i < size; i++)
    {
        int id = big[i];
        for(int j = 0; j < mresults_size; j++) if(mresults[j] == id) mtemp[inter_cur++] = id;
    }

    int *t = mtemp;
    mtemp = mresults;
    mresults = t;
    mresults_size = inter_cur;
}

void find_multi()
{
    //GET LIST OF LISTS on mresult
    mresults_size = 0;
    vector<int> stack;

    while(1)
    {

        int r = find();

        if(r < 0)
        {
            mresults_size = 0;
            return;
        }

        stack.push_back(r);

        //TRASH
        if(*find_cur == END) break;
        for(; *find_cur != END && *find_cur >= ALPHSIZE; find_cur++);
        if(*find_cur == END) break;

    }

    if(stack.size() == 0) return;
    sort(stack.begin(), stack.end(), SIZE_COMPARE); // ASCENDING ORDER PLEASE
    
    {
        int size = lists[stack[0]*2];
        int *first = &lists[lists[stack[0]*2+1]];
        //mresults = vector<int>(first, &first[size]);
        mresults_size = size;
        for(int i = 0; i < size; i++) mresults[i] = first[i];
    }

    for(int i = 1; i < stack.size(); i++)
    {
        //INTERSECT mresults with bigger list @stack[i]
        intersect(stack[i]);
        if(mresults_size == 0) break;
    }

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

        Load_Article(id);
        for(byte b : article)
            Print_UTF8(b);
    
    } 
    else 
    {

        // TODO - more words
        char *word = argv[2];
        for(char *c = word; *c; c++) if(*c == '+') *c = ' ';
        
        start();

        vector<byte> conv = Convert_ISO(word, '\0', true);
        find_cur = conv.data();
        find_multi();

        float t_search = stop();
        printf("%f\n", (t_search * 1000));
        
        for(int i = 0; i < mresults_size; i++)
        {
            int __ID__ = mresults[i];
            char *t = &titles_names[titles_data[__ID__].offset];
            printf("%d => ", __ID__);
            for(; *t != 0xa; t++) Print_UTF8(*t);
            printf("\n");
        }

    }

    return 0;

}