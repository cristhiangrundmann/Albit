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

    void print()
    {
        printf("Multi: ");
        for(int i = 0; i < ALPHSIZE; i++) printf("%d, ", next[i]);
        printf("\n");
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

    void print()
    {
        printf("Basic: c = %c, n = %d\n", c+'0', next);
    }

};

vector<MULTINODE*> multi;
vector<BASICNODE*> basic;
vector<int> basic_free;

void make(int *n)
{
    if(*n != 0) return;

    BASICNODE *bn;
    if(basic_free.size() > 0)
    {
        int b = basic_free.back();
        basic_free.pop_back();
        basic[b]->c = -1;
        basic[b]->next = 0;
        *n = -b-1;

        return;
    }
    bn = new BASICNODE;
    int b = basic.size();
    basic.push_back(bn);
    *n = -b-1;
}

int insert(unsigned char *str)
{
    {
        unsigned char *t;
        for(t = str; *t; t++) *t -= '0';
        *t = ALPHSIZE;
    }
    
    if(*str >= ALPHSIZE) return 0; 

    int *n = &multi[0]->next[*str];
    str++;

    make(n);

    for(; *str < ALPHSIZE; str++)
    {
        if(*n > 0) n = &multi[*n]->next[*str];
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
                basic_free.push_back(b);

                int nc = mn->next[c];
                mn->next[c] = basic[b]->next;
                if(nc > 0) multi[nc]->list = basic[b]->list;
                else if(nc < 0) basic[-nc-1]->list = basic[b]->list;
                


                *n = k;
                n = &mn->next[*str];
                make(n);
            }
        }
    }
}



int main()
{

    multi.push_back(new MULTINODE);

    while(1)
    {
        static unsigned char str[32];
        std::cin >> str;

        int i = insert(str);
        
        for(int i = 0; i < multi.size(); i++) 
        {
            printf("%d ", i);
            if(multi[i]) multi[i]->print();
        }

        for(int i = 0; i < basic.size(); i++) 
        {
            if(!basic[i]) continue;
            printf("%d ", -i-1);
            basic[i]->print();
        }

        printf("free: ");
        for(int i = 0; i < basic_free.size(); i++) 
        {
            printf("%d ", -basic_free[i]-1);
        }
        printf("\n");
    }

    for(MULTINODE *m : multi) if(m) delete m;
    for(BASICNODE *b : basic) if(b) delete b;


    return 0;
}