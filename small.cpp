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

void insert(unsigned char *str, int list)
{
    {
        unsigned char *t;
        for(t = str; *t; t++) *t -= '0';
        *t = ALPHSIZE;
    }
    
    if(*str >= ALPHSIZE) return; 

    int *n = &multi[0]->next[*str];
    str++;

    make(n);

    for(; *str < ALPHSIZE; str++)
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

                int nc = mn->next[c];
                mn->next[c] = basic[b]->next;
                if(nc > 0) multi[nc]->list = basic[b]->list;
                else if(nc < 0) basic[-nc-1]->list = basic[b]->list;
                
                *n = k;
                n = &mn->next[*str];
                *n = -b-1;
                basic[b]->c = -1;
                basic[b]->list = -1;
                basic[b]->next = 0;
            }
        }
    }

    if(*n == 0) return;
    if(*n > 0) multi[*n]->list = list;
    else basic[-(*n)-1]->list = list;
}

int find(unsigned char *str)
{
    {
        unsigned char *t;
        for(t = str; *t; t++) *t -= '0';
        *t = ALPHSIZE;
    }

    int n = 0, list = -1;
    for(; *str < ALPHSIZE; str++)
    {
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



int main()
{

    multi.push_back(new MULTINODE);
    static unsigned char str[32];

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