#include "albit.h"
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string>
#include <sys/ioctl.h>

int *lists;
MULTINODE *multi;
BASICNODE *basic;

#define GOTO(x, y) printf("\033[%d;%dH", y, x)
#define CLEAN      printf("\e[1;1H\e[2J")
#define TITLE      printf("\033[7;36m")
#define BOTTOM     printf("\033[7;36m")
#define WORD       printf("\033[1;35m")
#define RESET      printf("\033[0m")

static Title_Data* titles_data;
static char* titles_names;
static vector<byte> article;

static bool Bin_Find(const int* data, int size, int value) {

    if (size == 0)
        return false;
    if (size == 1)
        return data[0] == value;
    
    int div = size/2;

    if (data[div] == value)
        return true;
    if (data[div] > value)
        return Bin_Find(data, div, value);
    if (data[div] < value)
        return Bin_Find(&data[div+1], size - div - 1, value);

}

static void Load_Article(int id) {

    article.clear();

    Title_Data& doc = titles_data[id];

    ifstream file(string("database_clean/db" + to_string(doc.database)));
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

void Show_Article(int id, vector<vector<byte>> target_words) {

    Load_Article(id);

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    // Aumenta o tamanho do buffer do stdout
    char* stdout_buff = (char*)malloc(w.ws_col * w.ws_row * 2);
    setvbuf(stdout, stdout_buff, _IOFBF, w.ws_col * w.ws_row * 2);

    vector<int> lines;
    lines.push_back(0);

    vector<int> shiny_words;

    const byte* data = article.data();
    int data_size = article.size();

    int position = 0;
    int cursor_x = 0;

    while(1) {

        if (position >= data_size)
            break;

        const byte* word = &data[position];
        int word_size = 0;

        vector<bool> is_word;
        is_word.resize(target_words.size(), true);
        int compp = 0;

        for(; word_size < data_size - position; word_size++) {
            
            if (ISO_8859[word[word_size]] != NON) {

                for (int i = 0; i < target_words.size(); i++) {

                    if (ISO_8859[word[word_size]] != target_words[i][compp])
                        is_word[i] = false;

                }

                compp++;
            }

            if (word[word_size] == ' ') break;
            if (word[word_size] == '\n') break;
        }

        bool is_shiny = false;
        
        for (int i = 0; i < target_words.size(); i++) {

            if (is_word[i] && compp == target_words[i].size() - 1)
                is_shiny = true;

        }

        if (is_shiny)
            shiny_words.push_back(position);

        if (word_size == 0) {
            if (word[0] == '\n') {
                lines.push_back(position+1);
                cursor_x = 0;
            }
            if (word[0] == ' ') {
                cursor_x++;
            }
            position++;
            continue;
        }

        // Palavra desce
        if (word_size > w.ws_col - cursor_x) {

            lines.push_back(position);
            position += word_size;
            cursor_x = word_size;
            continue;

        }

        // Espaço ou newline vira quebra
        if (word_size == w.ws_col - cursor_x) {

            position += word_size + 1;
            cursor_x = 0;
            lines.push_back(position);
            continue;

        }

        if (word_size < w.ws_col - cursor_x) {

            position += word_size;
            cursor_x += word_size;

        }

    }

    int first_line = 0;
    bool going_up = false;
    
    while(1) {

        CLEAN;
        GOTO(1,1);

        TITLE;
        int offset = titles_data[id].offset;
        char* title = &titles_names[offset];

        int title_size = 0;
        for(;title[title_size] != '\n'; title_size++)
            Print_UTF8(title[title_size]);
        
        for (int i = title_size; i < w.ws_col; i++)
            printf(" ");
        
        RESET;

        
        printf("\n");

        if (first_line < 0)
            first_line = 0;
        if (int(lines.size()) - w.ws_row <= 0)
            first_line = 0;
        if (first_line > lines.size() - w.ws_row)
            first_line = lines.size() - w.ws_row + 1;

        for (int line_i = first_line; line_i < first_line + w.ws_row - 2; line_i++) {

            if (line_i >= lines.size())
                break;
            
            int line_begin = lines[line_i];

            int line_end = 0;
            if (line_i == lines.size() - 1)
                line_end = data_size;
            else
                line_end = lines[line_i + 1];

            for (int i = line_begin; i < line_end; i++) {

                // Método feio
                if (Bin_Find(shiny_words.data(), shiny_words.size(), i)) {
                    WORD;
                }

                if (data[i] == ' ' || data[i] == '\n') {

                    RESET;
                    if (i == line_end-1 && (data[i] == ' ' || data[i] == '\n'))
                        break;

                }
                
                
                Print_UTF8(data[i]);
            }

            printf("\n");

        }

        GOTO(0, w.ws_row);
        BOTTOM;
        const char text[] = "U up - D down - Q quit";
        const char going_down_s[] = " - Enter down >> ";
        const char going_up_s[]   = " - Enter up   >> ";
        
        int s_size = sizeof(text) + sizeof(going_down_s) - 1;
        printf(text);

        if (going_up)
            printf(going_up_s);
        else
            printf(going_down_s);
        
        
        for (int i = s_size - 1; i < w.ws_col; i++)
            printf(" ");
        
        GOTO(s_size, w.ws_row);

        fflush(stdout);

        char c = getchar();

        switch(c) {

            case('d'):
            case('D'):
                going_up = false;
                continue;
            case('U'):
            case('u'):
                going_up = true;
                continue;
            case(10):
                if (going_up)
                    first_line--;
                else 
                    first_line++;
                continue;
            case('Q'):
            case('q'):
                free(stdout_buff);
                return;

            default:
                continue;

        }
        
    }

    free(stdout_buff);

}

const byte *find_cur;
vector<vector<byte>> words_cur;

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

    words_cur.clear();

    while(1)
    {
        const byte* word_end = find_cur;
        for(;*word_end < SKP; word_end++);
        vector<byte> word(find_cur, word_end);
        word.push_back(END);

        words_cur.push_back(word);

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


bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int main()
{
    start();   
    multi = (MULTINODE*)Load_on_RAM(string("trie/trie_multi"));
    basic = (BASICNODE*)Load_on_RAM(string("trie/trie_basic"));
    lists = (int*)Load_on_RAM(string("trie/lists"));
    titles_data = (Title_Data*)Load_on_RAM(string("titles/titles_data"));
    titles_names = (char*)Load_on_RAM(string("titles/titles_names"));
    printf("Load time: %f seconds\n", stop());

    while(1)
    {
        string word;
        printf("\033[0;34m"); 
        printf("Search >> ");
        printf("\033[1;34m"); 
        cin.sync();
        getline(std::cin, word);
        start();

        int n = 0;

        vector<byte> conv = Convert_ISO(word.c_str(), '\0', true);
        find_cur = conv.data();
        find_multi();
        
        float t_search = stop();

        int results = mresults_size;
        
        if(results > 0)
        {
            SHOW_AGAIN:
            
            printf("\033[1;32m");
            
            printf("Found %d results (%f seconds)\n", results, t_search);
            
            int page = 20;

            for(int i = 0; i < results; i++)
            {
                int __ID__ = mresults[i];
                printf("\033[0;32m");
                printf("Result %d: \033[0m", i+1);
                char *t = &titles_names[titles_data[__ID__].offset];
                for(; *t != 0xa; t++) Print_UTF8(*t);
                printf("\n");
                page--;
                if(page == 0 || i == results - 1)
                {
                    ASK_AGAIN:
                    printf("\033[0;33m"); 
                    printf("Do you want more results? (Y/N)\033[0m\n");
                    string in;
                    std::cin >> in;
                    
                    int cc = ISO_8859[(unsigned char)in[0]];
                    if(cc == C_Y | cc == C_S) page = 20;
                    else if(cc == C_N) break;

                    else if (is_number(in))
                    {
                        int result = std::stoi(in);

                        if (result - 1 >= results)
                            goto ASK_AGAIN;

                        Show_Article(mresults[result-1], words_cur);

                        CLEAN;
                        RESET;
                        goto SHOW_AGAIN;

                    }
                    else goto ASK_AGAIN;
                }
            }
        }
        else printf("\033[0;31mNo matches\n");
    }

    return 0;
}