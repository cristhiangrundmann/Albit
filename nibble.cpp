#include "albit.h"



MULTINODE *multi;
BASICNODE *basic;
vector<A_NODE*> anodes;
vector<B_NODE*> bnodes;

void convert(int *n)
{
    if(*n > 0)
    {
        A_NODE *an = new A_NODE;
        an->list = multi[*n].list;
        for(int i = 0; i < 6; i++)
        {
            B_NODE *bn = nullptr;
            for(int j = 0; j < 6; j++)
            {
                if(multi[*n].next[6*i+j] == 0) continue;
                if(bn == nullptr) 
                {
                    bn = new B_NODE;
                    an->next[i] = bnodes.size();
                    bnodes.push_back(bn);
                }
                bn->next[j] = multi[*n].next[6*i+j];
                convert(&bn->next[j]);
            }   
        }
        *n = anodes.size();
        anodes.push_back(an);
    }
    else if(*n < 0) if(basic[-(*n)-1].next != 0) convert(&basic[-(*n)-1].next);
}

int main()
{
    int msize, bsize;
    multi = (MULTINODE*)Load_on_RAM("trie/trie_multi");
    basic = (BASICNODE*)Load_on_RAM("trie/trie_basic", &bsize);


    //MANUAL ROOT INSERTION
    A_NODE *an = new A_NODE;
    an->list = multi[0].list;
    anodes.push_back(an);
    for(int i = 0; i < 6; i++)
    {
        B_NODE *bn = new B_NODE;
        an->next[i] = bnodes.size();
        bnodes.push_back(bn);
        for(int j = 0; j < 6; j++)
        {
            bn->next[j] = multi[0].next[6*i+j];
            convert(&bn->next[j]);
        }

    }

    printf("Anodes: %ld\n", anodes.size());
    printf("Bnodes: %ld\n", bnodes.size());
    printf("TOTAL SIZE: %ld\n", anodes.size() * sizeof(A_NODE)+
    bnodes.size() * sizeof(B_NODE));

    printf("sizeof(BASICNODE) = %ld\n", sizeof(BASICNODE));

    FILE *af = fopen("trie/light/trie_a", "wb");
    for(A_NODE *a : anodes)
    {
        fwrite(a, sizeof(A_NODE), 1, af);
    }
    fclose(af);

    FILE *bf = fopen("trie/light/trie_b", "wb");
    for(B_NODE *b : bnodes)
    {
        fwrite(b, sizeof(B_NODE), 1, bf);
    }
    fclose(bf);

    FILE *bs = fopen("trie/light/trie_basic", "wb");
    fwrite(basic, bsize, 1, bs);
    fclose(bs);
}