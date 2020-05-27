#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#define ALPHSIZE 8

using std::vector;
using std::string;
using std::ifstream;

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

vector<MULTINODE*> multi;
vector<BASICNODE*> basic;


void make(int *n)
{
    if(*n != 0) return;
    BASICNODE *bn = new BASICNODE;
    *n = -basic.size()-1;
    basic.push_back(bn);
}

void insert(char *w, int list)
{
    {
        char *t;
        for(t = w; *t && *t != ' '; t++) *t -= '0';
        *t = ALPHSIZE;
    }

    if(*w >= ALPHSIZE) return;

    int *n = &multi[0]->next[*w];
    make(n);

    for(w++; *w < ALPHSIZE; w++)
    {
        if(*n > 0)
        {
            n = &multi[*n]->next[*w];
            make(n);
        }
        else if(*n < 0)
        {
            int b = -(*n)-1;
            char c = basic[b]->c;
            if(c == -1 || c == *w)
            {
                basic[b]->c = *w;
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
                n = &mn->next[*w];
                *n = -b-1;
                basic[b]->c = -1;
                basic[b]->list = -1;
                basic[b]->next = 0;
            }
        }
    }

    if(*n > 0) multi[*n]->list = list;
    else basic[-(*n)-1]->list = list;

}

int find(char *w)
{
    {
        char *t;
        for(t = w; *t && *t != ' '; t++) *t -= '0';
        *t = ALPHSIZE;
    }

    int n = 0, list = -1;
    for(; *w < ALPHSIZE; w++)
    {
        if(n >= 0)
        {
            if(multi[n]->next[*w] != 0) 
            {
                n = multi[n]->next[*w];
                if(n >= 0) list = multi[n]->list;
                else list = basic[-n-1]->list;
            }
            else return -1;
        }
        else
        {
            int b = -n-1;
            char c = basic[b]->c;
            if(c != *w) return -1;
            n = basic[b]->next;
            if(n >= 0) list = multi[n]->list;
            else list = basic[-n-1]->list;
        }
    }
    return list;
}


int main()
{

    multi.push_back(new MULTINODE);
    static char str[32];

    int list = 0;
    while(1)
    {
        printf("LIST %d: ", list);
        std::cin >> str;
        if(*str == '!') break;

        insert(str, list++);
    }

    while(1)
    {
        std::cin >> str;
        list = find(str);
        printf("LIST = %d\n", list);
    }

    for(MULTINODE *m : multi) if(m) delete m;
    for(BASICNODE *b : basic) if(b) delete b;


    return 0;
}