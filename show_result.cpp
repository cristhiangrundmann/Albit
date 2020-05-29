#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>

#include <chrono>
#include <thread>

#include <sys/ioctl.h>

#define GOTO(x, y) printf("\033[%d;%dH", y, x)
#define CLEAN      printf("\e[1;1H\e[2J")
#define TITLE      printf("\033[1;34m")
#define BOTTOM     printf("\033[1;34m")
#define WORD       printf("\033[0;35m")
#define RESET      printf("\033[0m")

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

using namespace std;

typedef uint8_t byte;

vector<byte> article;

struct Title_Data {

    uint32_t offset, database, database_offset;

};

Title_Data* titles_data;
char* titles_names;

enum {
    C_0, C_1, C_2, C_3,
    C_4, C_5, C_6, C_7,
    C_8, C_9,
    
    C_A, C_B, C_C, C_D, C_E,
    C_F, C_G, C_H, C_I, C_J,
    C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S,
    C_T, C_U, C_V, C_W,
    C_X, C_Y, C_Z,

    NON, END
};

const byte ISO_8859[256] = {
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    C_0, C_1, C_2, C_3, C_4, C_5, C_6, C_7, C_8, C_9, NON, NON, NON, NON, NON, NON,
    C_A, C_A, C_B, C_C, C_D, C_E, C_F, C_G, C_H, C_I, C_J, C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S, C_T, C_U, C_V, C_W, C_X, C_Y, C_Z, NON, NON, NON, NON, NON,
    NON, C_A, C_B, C_C, C_D, C_E, C_F, C_G, C_H, C_I, C_J, C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S, C_T, C_U, C_V, C_W, C_X, C_Y, C_Z, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, C_C, C_L, C_C, C_Y, C_S, NON, C_S, C_C, C_A, NON, NON, NON, C_R, NON,
    C_O, NON, C_2, C_3, C_Z, C_U, NON, NON, C_Z, C_1, C_O, NON, C_O, C_O, C_Y, NON,
    C_A, C_A, C_A, C_A, C_A, C_A, C_A, C_C, C_E, C_E, C_E, C_E, C_I, C_I, C_I, C_I,
    C_D, C_N, C_O, C_O, C_O, C_O, C_O, C_X, C_O, C_U, C_U, C_U, C_U, C_Y, C_P, C_S,
    C_A, C_A, C_A, C_A, C_A, C_A, C_A, C_C, C_E, C_E, C_E, C_E, C_I, C_I, C_I, C_I,
    C_D, C_N, C_O, C_O, C_O, C_O, C_O, NON, C_O, C_U, C_U, C_U, C_U, C_Y, C_P, C_Y
};

void* Load_on_RAM(string filename) {

    ifstream file(filename, ios::binary | ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (!file.good())
        return nullptr;
    
    void* data = malloc(size);
    if (!data)
        return nullptr;

    if (file.read((char*)data, size)) {   
        file.close();
        return data;
    }

    file.close();
    free(data);
    return nullptr;

}

const byte error[] = {0xF0, 0x9F, 0x96, 0x95, 0xEF, 0xB8, 0x8F, 0x00};

void Print_UTF8(byte ch) {
    
    if (ch > 0x7F) {
        // C2
        if (ch >= 0xA0 && ch < 0xC0) {
            switch(ch) {
                case(0xA0):
                    printf(" ");
                    return;
                case(0xA4):
                    printf("%c%c%c", 0xE2, 0x82, 0xAC);
                    return;
                case(0xA6):
                    printf("%c%c", 0xC5, 0xA0);
                    return;
                case(0xA8):
                    printf("%c%c", 0xC5, 0xA1);
                    return;
                case(0xB4):
                    printf("%c%c", 0xC5, 0xBD);
                    return;
                case(0xB8):
                    printf("%c%c", 0xC5, 0xBE);
                    return;
                case(0xBC):
                    printf("%c%c", 0xC5, 0x92);
                    return;
                case(0xBD):
                    printf("%c%c", 0xC5, 0x93);
                    return;
                case(0xBE):
                    printf("%c%c", 0xC5, 0xB8);
                    return;
                default:
                    printf("%c%c", 0xC2, ch);
                    return;
            }
        }
        //C3
        if (ch >= 0xC0 && ch <= 0xFF) {
            printf("%c%c", 0xC3, (ch) - 0x40);
            return;
        }
        //ERROR
        printf("%s", error);
        return;
    }

    printf("%c", ch);

}

void load_article(int id) {

    article.empty();

    Title_Data& doc = titles_data[id];

    ifstream file(string("database3/db" + to_string(doc.database)));
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

void show_article(int id, const char* target_word) {

    struct winsize w;
    vector<int> newline_stack;
    int page_position = 0;
    
    while(1) {

        vector<int> newlines;

        ioctl(0, TIOCGWINSZ, &w);

        CLEAN;
        GOTO(0,0);

        TITLE;
        int offset = titles_data[id].offset;
        for(char* title = &titles_names[offset]; *title != '\n'; title++)
            Print_UTF8(*title);
        RESET;

        const byte* data = article.data();
        int data_size = article.size();
        
        int array_position = page_position;

        int cursor_x = 0, cursor_y = 2;
        GOTO(cursor_x, cursor_y);

        int ended = false;

        loop:

            //fflush(stdout);
            //this_thread::sleep_for(std::chrono::milliseconds(7));

            const byte* word;
            int word_size = 0;
            bool is_target_word = true;
            int compare_position = 0;

            if (array_position >= data_size) {
                ended = true;
                goto endloop;
            }

            word = &data[array_position];

            for (; word_size < data_size - array_position; word_size++) {

                if (word[word_size] == ' ') break;
                
                if (ISO_8859[word[word_size]] != NON) {
                    if (ISO_8859[word[word_size]] != ISO_8859[target_word[compare_position]])
                        is_target_word = false;
                    compare_position++;
                }
                
            }

            if (is_target_word) {
                WORD;
            } else {
                RESET;
            }

            if (word_size == 0) {
                
                Print_UTF8(' ');
                cursor_x++;
                array_position++;
                goto loop;

            }

            if (word_size < w.ws_col - cursor_x) {
                for (int i = 0; i < word_size; i++) {
                    if (word[i] == '\n'){
                        
                        cursor_x = 0;
                        cursor_y++;
                        newlines.push_back(array_position);

                        if (cursor_y >= w.ws_row)
                            goto endloop;
                        GOTO(cursor_x, cursor_y);
                    } else {
                        Print_UTF8(word[i]);
                        cursor_x++;
                    }
                    array_position++;
                }
            }
            else {
                cursor_x = 0;
                cursor_y++;
                GOTO(0,cursor_y);
                newlines.push_back(array_position);

                if (cursor_y >= w.ws_row)
                    goto endloop;
                
                for (int i = 0; i < word_size; i++) {
                    if (word[i] == '\n'){

                        cursor_x = 0;
                        cursor_y++;
                        newlines.push_back(array_position);

                        if (cursor_y >= w.ws_row)
                            goto endloop;
                        GOTO(cursor_x, cursor_y);
                    } else {
                        Print_UTF8(word[i]);
                        cursor_x++;
                    }
                    array_position++;
                }
            }

            //Print_UTF8(' ');
            //cursor_x++;
        
        goto loop;
        endloop:

        GOTO(0, w.ws_row);
        BOTTOM;
        printf("U up - D down - E exit >>");

        getcharagain:

        char c = getchar();

        switch(c) {

            case('d'):
            case('D'):
                if (!ended) {
                    newline_stack.push_back(page_position);
                    page_position = newlines.front() + 1;
                    continue;
                }
            case('U'):
            case('u'):
                if (newline_stack.size() > 0) {
                    page_position = newline_stack.back();
                    newline_stack.pop_back();
                }
                continue;
            case('E'):
            case('e'):
                return;
            default:
                continue;
                


        }
        
    }




}

int main(int argc, char** argv) {

    uint32_t id;
    const char* word;

    if (argc <= 2) {

        id = 1232793;
        word = "pokemon";

    } else {

        // Has to converto word to byte array
        id = atoi(argv[1]);
        word = argv[2];

    }

    titles_data = (Title_Data*) Load_on_RAM("titles/titles_data");
    titles_names = (char*) Load_on_RAM("titles/titles_names");

    load_article(id);
    show_article(id, word);
    //Print_UTF8(&titles_names[titles_data[id].offset]);

}