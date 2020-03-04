#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define BUFFERSIZE 0x10000

char buffer_a[BUFFERSIZE + 1];
char buffer_b[BUFFERSIZE + 1];
char buffer_l[BUFFERSIZE + 1];

char *buf_old, *buf_new, *line, span;
size_t linelen, lineno;

char make_line()
{
    if(linelen >= BUFFERSIZE)
    {
        printf("Ignoring large line: %ld chars\n", linelen);
        return 0;
    }

    strcpy(buffer_l, line);
    if(span) strcat(buffer_l, buf_new);

    return 1;
}

void process_line()
{
    if(!make_line()) return;

    //...
}

void process_file(int i)
{
    char filename[24];
    sprintf(filename, "./database/db%d", i);
    FILE *dbi = fopen(filename, "rb");
    if(!dbi) return;

    buf_old = buffer_a;
    buf_new = buffer_b;

    span = 0;
    line = buf_new;

    size_t len;

    while(len = fread(buf_new, 1, BUFFERSIZE, dbi))
    {
        buf_new[len] = 0;
        for(char *cur = buf_new; *cur; cur++)
        {
            if(*cur == '\n')
            {
                lineno++;

                if(*line != '\n')
                {
                    *cur = 0;
                    process_line();
                }
                line = cur + 1;
                span = 0;
                linelen = 0;
                continue;
            }
            linelen++;
        }
        span = buf_new[len - 1] != '\n';
        char *t = buf_new; buf_new = buf_old; buf_old = t;
    }
    lineno++;
    process_line();

    fclose(dbi);
}

int main()
{

    time_t time0 = time(0);

    for(int i = 0; i < 164; i++)
    {
        //lineno = 0;
        printf("::%d::\n", i);
        process_file(i);
    }

    printf("Time = %lu\n", time(0) - time0);

    return 0;
}