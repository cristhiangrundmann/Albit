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
#define TITLE      printf("\033[7;36m")
#define BOTTOM     printf("\033[7;36m")
#define WORD       printf("\033[1;35m")
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

bool Bin_Find(const int* data, int size, int value) {

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

vector<byte> Convert_ISO(const char* word, char end = '\0') {

    vector<byte> word_converted;

    for (const byte* c = (byte*)word; *c != end; c++) {
        byte id = ISO_8859[*c];
        if (id != NON)
            word_converted.push_back(id);
    }

    word_converted.push_back(END);
    return word_converted;

}

void show_article(int id, vector<vector<byte>> target_words) {

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    
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
        if (first_line > lines.size() - w.ws_row)
            first_line = lines.size() - w.ws_row;

        for (int line_i = first_line; line_i < first_line + w.ws_row - 2; line_i++) {

            int line_begin = lines[line_i];
            int line_end = lines[line_i + 1];

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
        const char text[] = "U up - D down - E exit >> ";

        printf(text);
        for (int i = sizeof(text) - 1; i < w.ws_col; i++)
            printf(" ");
        
        GOTO(int(sizeof(text)), w.ws_row);

        char c = getchar();

        switch(c) {

            case('d'):
            case('D'):
                first_line++;
                continue;
            case('U'):
            case('u'):
                first_line--;
                continue;

            case('E'):
            case('e'):
            case('Q'):
            case('q'):
                return;

            default:
                continue;

        }
        
    }




}

int main(int argc, char** argv) {

    uint32_t id;
    vector<vector<byte>> words;


    if (argc <= 2) {

        id = 1232793;
        words.push_back(Convert_ISO("the"));
        words.push_back(Convert_ISO("pokemon"));

    } else {

        // Has to converto word to byte array
        id = atoi(argv[1]);

        for (int i = 2; i < argc; i++)
            words.push_back(Convert_ISO(argv[i]));

    }

    titles_data = (Title_Data*) Load_on_RAM("titles/titles_data");
    titles_names = (char*) Load_on_RAM("titles/titles_names");

    load_article(id);
    show_article(id, words);
    //Print_UTF8(&titles_names[titles_data[id].offset]);

}