#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define BUFFERSIZE 0x10000

char buffer_a[BUFFERSIZE + 1];
char buffer_b[BUFFERSIZE + 1];
char buffer_l[BUFFERSIZE + 1];

char *buf_get, *buf_set, *line, span;
size_t linelen;

struct NODE
{
    char type; //
    union
    {
        //$ 0-9 a-z
        NODE* char_next[1 + 10 + 26];

        int docs[64];
        NODE *docs_next;


    };
};

char make_line()
{
    if(linelen >= BUFFERSIZE)
    {
        printf("Ignoring large line: %ld chars\n", linelen);
        return 0;
    }

    strcpy(buffer_l, line);
    if(span) strcat(buffer_l, buf_get);

    return 1;
}

char *title_str;
int id = -1;

char process_tags()
{
    static const char *doctag = "<doc ";
    const char *tag = doctag;
    char *cur = buffer_l;

    for(; *tag; tag++)
    {
        if(*tag != *cur) return 0;
        cur++;
    }

    int quos = 0;
    
    //char *id_str;

    for(; *cur; cur++)
    if(*cur == '"') 
    {
        quos++;
        if(quos == 3) 
        {
            title_str = cur+1;
            break;
        }
    }

    if(quos != 3) return 0;

    quos = 0;

    cur = buffer_l + linelen - 1;

    for(; *cur; cur--)
    if(*cur == '"')
    {
        quos++;
        //if(quos == 1) *cur = 0;
        //if(quos == 2) id = atoi(cur+1);
        if(quos == 7)
        {
            *cur = 0;
            break;
        }
    }

    if(quos != 7) return 0;
    id++;
    return 1;
}

char ccc[256];

void process_line()
{
    if(!make_line()) return;

    if(process_tags()) return;

    

    
}

void process_file(const char *filename)
{
    FILE *dbi = fopen(filename, "rb");
    if(!dbi) return;

    buf_get = buffer_a;
    buf_set = buffer_b;

    span = 0;
    line = buf_set;

    size_t len;

    while(len = fread(buf_set, 1, BUFFERSIZE, dbi))
    {
        buf_set[len] = 0;
        char *t = buf_set; buf_set = buf_get; buf_get = t;

        char *cur = buf_get;
        for(; *cur; cur++)
        {
            if(*cur == '\n')
            {
                *cur = 0;
                process_line();
        
                line = cur + 1;
                span = 0;
                linelen = 0;
                continue;
            }
            linelen++;
        }
        span = buf_get[len - 1] != '\n';
    }
    process_line();

    fclose(dbi);
}

int main()
{

  

    time_t time0 = time(0);

    for(int i = 0; i < 164; i++)
    {
        printf("::%d::\n", i);
        char filename[24];
        sprintf(filename, "./database/db%d", i);
        process_file(filename);
    }

    printf("Time = %lu\n", time(0) - time0);

    return 0;
}