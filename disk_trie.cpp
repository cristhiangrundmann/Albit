#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

typedef uint8_t byte;
typedef uint32_t u32;
typedef uint64_t u64;

// Node content
#define NODE_CHILDS 10 + 26 + 1
#define EMPTY -1

// Node index
enum NODE_ID { 
    C_0, C_1, C_2, C_3,
    C_4, C_5, C_6, C_7,
    C_8, C_9,
    
    C_A, C_B, C_C, C_D, C_E,
    C_F, C_G, C_H, C_I, C_J,
    C_K, C_L, C_M, C_N, C_O,
    C_P, C_Q, C_R, C_S,
    C_T, C_U, C_V, C_W,
    C_X, C_Y, C_Z,

    NON
};

// O que significam os filhos de Node
const char TO_ASCII[NODE_CHILDS] {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9',

    'a', 'b', 'c', 'd', 'e',
    'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's',
    't', 'u', 'v', 'w',
    'x', 'y', 'z',

    '\0'
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

vector<byte> Convert_ISO(const char* word, char end = '\0') {

    vector<byte> word_converted;

    for (const byte* c = (byte*)word; *c != end; c++) {
        byte id = ISO_8859[*c];
        if (id != NON)
            word_converted.push_back(id);
    }

    word_converted.push_back(NON);
    return word_converted;

}

struct Node {

    int next[NODE_CHILDS];

    Node() {

        for (int i = 0; i < NODE_CHILDS; i++)
            next[i] = EMPTY;

    }

} __attribute__((packed));

struct Entry {

    vector<byte> original_word;
    //int loop_position = 0;

    int titles = 0;
    //vector<int> titles;
    //vector<int> pesos;

    Entry(vector<byte> word) : original_word(word) {}

};

#define FLAGS ios::out | ios::binary

struct DTrie {

    string directory;

    vector<FILE*> files;
    vector<int> nodes_count;

    vector<Entry> entries;

    const Node Empty_Node;

    int get_node_data(int id, int level, byte next) {

        if (files.size() <= level)
            return EMPTY;

        int ret;

        // Max 4GB
        fseek(files[level], id * sizeof(Node) + next * sizeof(int), SEEK_SET);
        fread(&ret, sizeof(int), 1, files[level]);

        return ret;

    }

    // Escreve 1 byte no disco
    void set_node_data(int id, int level, byte next, int data) {

        fseek(files[level], id * sizeof(Node) + next * sizeof(int), SEEK_SET);
        fwrite(&data, sizeof(int), 1, files[level]);

    }

    int add_node(int level) {

        if (files.size() <= level) {
            string name = directory + "level" + to_string(level);
            files.push_back(fopen(name.c_str(), "w+"));
            nodes_count.push_back(0);
        }
        
        int position = nodes_count[level];
        //fseek(files[level], 0, SEEK_END);
        fseek(files[level], position * sizeof(Node), SEEK_SET);
        fwrite(&Empty_Node, sizeof(Empty_Node), 1, files[level]);
        
        nodes_count[level]++;
        return position;

    }

    void print_entry(const char* word) {

        int id = find(word);
        if (id == EMPTY) {
            cout << word << "\t = **" << endl;
            return;
        }

        Entry& entry = entries[id];
        string name;
        for (byte b : entry.original_word)
            name += TO_ASCII[b];
        
        cout << word << "\t = " << name << endl;

    }

    // Return -1 if it coudn't find word
    int find(const char* word) {

        int level = 0;
        int current_node = 0;
        
        vector<byte> vec = Convert_ISO(word);
        const byte* data = vec.data();

        for (; *data != NON; data++) {

            byte content = get_node_data(current_node, level, *data);
            if(content == EMPTY)
                return -1;
                //return get_node_data(current_node, level, NON);
            
            current_node = content;
            level++;

        }

        return get_node_data(current_node, level, NON);

    }

    void add_word(const char* word, char end = '\0') {

        vector<byte> word_b = Convert_ISO(word, end);
        if (word_b.size() <= 1)
            return;

        size_t current_node = 0;
        size_t level = 0;
        
        const byte* data = word_b.data();
        for (; *data != NON; data++) {
            
            int content = get_node_data(current_node, level, *data);
            if (content == EMPTY) {
                content = add_node(level + 1);
                set_node_data(current_node, level, *data, content);
            }

            current_node = content;
            level++;

        }
        //level--; //?

        int list = get_node_data(current_node, level, NON);
        if (list == EMPTY) {
            list = entries.size();
            set_node_data(current_node, level, NON, list);
            entries.push_back(Entry(word_b));
        }

        entries[list].titles++;
    }

    DTrie(string dir) : directory(dir) {

        add_node(0);
    
    }

    ~DTrie() {

        for(FILE* f : files)
            fclose(f);

    }

};

char Words[][32] = {
    "abacate",
    "abacaxi",
    "aba",//
    "balao",
    "bala",
    "batata",//
    "tomate",
    "cebola",
    "carne",
    "tombo",
    "trem",
    "trator",
    "pokemon",
    "pirao",
    "panela",
    "pancada",
    "porto",
    "queijo"
};

void wait_input(DTrie& trie) {
    
    while(1) {

        string word;
        printf(">> ");
        cin >> word;

        trie.print_entry(word.c_str());

    }
}

int main1() {

    DTrie trie("trie/");
    string file_name;

    for (int i = 0; i < 1; i++) {
        
        file_name = "database/db";
        file_name += to_string(i);
        cout << file_name << endl;

        ifstream db(file_name);
        string line;
        if (db.is_open()) {
            while (getline(db, line)) {
                if (line.compare(0, 5, "<doc ") == 0) {
                    //title here
                } else {
                    trie.add_word(line.c_str(), ' ');
                }
            }
        }
        db.close();
    
    }

    //cout << trie.nodes.size() << " nodes" << endl;
    wait_input(trie);
}


int main2() {

    DTrie trie("trie/");

    for (int i = 0; i < sizeof(Words) / 32; i++)
        trie.add_word(Words[i]);

    for (int i = 0; i < sizeof(Words) / 32; i++)
        trie.print_entry(Words[i]);

    return 0;

}

int main() {

    return main1();

}
