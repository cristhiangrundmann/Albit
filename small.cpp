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

void make(int *n)
{
    if(*n == 0)
    {
        BASICNODE *bn = new BASICNODE;
        int b = -basic.size()-1;
        basic.push_back(bn);
        *n = b;
    }
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
                printf(".\n");
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
                delete basic[b];
                basic[b] = nullptr;
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
    }

    for(MULTINODE *m : multi) if(m) delete m;
    for(BASICNODE *b : basic) if(b) delete b;


    return 0;
}