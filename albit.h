#ifndef ALBIT_H
#define ALBIT_H

#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>



using namespace std;

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

    NON, SKP, END
};

int ISO_8859[256] = {
    END, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON,
    NON, NON, NON, NON, NON, NON, NON, SKP, NON, NON, NON, NON, NON, NON, NON, NON,
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


#define ALPHSIZE 36

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

// Title class #################################


#define PACKED __attribute__((packed))

typedef uint8_t byte;

struct Title_Data {

    uint32_t offset, database, database_offset;

} PACKED;

struct timespec t_start, t_end;
void start()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_start);   
}

float stop()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
    return ((t_end.tv_sec - t_start.tv_sec) * 1000000000 + 
    (t_end.tv_nsec - t_start.tv_nsec)) / 1000000000.f;
}

// GET TITLE ###################################
size_t find_nth(string& str, char pat, int n) {

    int begin, end, delta;
    
    if (n == 0)
        return -1;

    if (n < 0) {

        begin = str.size() - 1;
        end = 0;
        delta = -1;
        n = -n;

    } else {

        begin = 0;
        end = str.size();
        delta = 1;

    }

    const char* str_c = str.c_str();

    int count = 0;

    for (int pos = begin; pos != end; pos += delta) {

        if (str_c[pos] == pat)
            count++;
        
        if (count == n)
            return pos;

    }

    return -2;

}

string get_title(string& line) {

    size_t begin = find_nth(line, '"', 3);
    size_t end = find_nth(line, '"', -7);

    return line.substr(begin + 1, end - begin - 1);

}
// #############################################


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




#endif