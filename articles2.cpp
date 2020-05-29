#include "albit.h"

#define BUFFERSIZE 0x10000

char line[BUFFERSIZE];
size_t linelen;

struct LIST
{
    int id;
    float weight;
};

struct VVLIST
{
    int list;
    int art;
};

bool LIST_COMPARE(LIST a, LIST b)
{
    return a.weight > b.weight;
}

vector<vector<LIST>> lists;
vector<MULTINODE*> multi;
vector<BASICNODE*> basic;
vector<VVLIST> words;

int id = -1;
int weight;

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
        vector<LIST> list;
        lists.push_back(list);
    }

    if(lists[*llist].size() > 0) if(lists[*llist].back().id == id) 
    {
        lists[*llist].back().weight += weight;
        return;
    }

    VVLIST vv;
    vv.list = *llist;
    vv.art = lists[*llist].size();
    words.push_back(vv);

    LIST ll;
    ll.id = id;
    ll.weight = weight;
    lists[*llist].push_back(ll);
}

void calc_freq()
{
    if(words.size() == 0) return;
    for(int i = 0; i < words.size(); i++) lists[words[i].list][words[i].art].weight /= words.size();
    words.clear();
}

void process_line()
{
    char *str = line;

    if(linelen >= 4)
    if( *(uint32_t*)line == 0x636f6423 )
    {
        calc_freq();
        id++;
        str += 5;
        weight = 5;
    }
    else weight = 1;

    
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
    calc_freq();

    float t_prep = stop();

    //SORT BY WEIGHT
    for(int i = 0; i < lists.size(); i++)
    {
        sort(lists[i].begin(), lists[i].end(), LIST_COMPARE);
    }

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