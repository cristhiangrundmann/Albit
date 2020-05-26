#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

typedef uint8_t byte;

// Node content
#define NODE_CHILDS 40
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

    NON, END
};

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

    '\0', '\0'
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

    word_converted.push_back(END);
    return word_converted;

}

byte Free_Buffer[256];

void Clean_Free_Buffer() {

    memset(Free_Buffer, 256, sizeof(byte));

}

// -1 -> iguais
// Retorta a posição da primeira diferença
int Compare(const byte* v1, const byte* v2) {

    for(int i = 0 ;; i++) {
        if (v1[i] != v2[i])
            return i;
    
        if (v1[i] == END)
            return -1;
    
        i++;
    }

}


int Compare2(const byte* v1, const byte* v2) {

    // Confiando que ela venha zerada
    byte list[NODE_CHILDS];

    for(int i = 0; v1[i] != END; i++)
        list[v1[i]] = 1;
    
    for(int i = 0; v2[i] != END; i++) {

        if(list[v2[i]] == 1)
            return v2[i];
        
        list[v2[i]] = 2;
    
    }
    
    for(int i = 0; v1[i] != END; i++)
        if (list[v1[i]] == 1)
            return -v1[i];

}

struct Node {

    int next[NODE_CHILDS];
    int num = 0;
    int list = 0;

    Node() {

        for (int i = 0; i < NODE_CHILDS; i++)
            next[i] = EMPTY;

    }

};

struct Entry {

    vector<byte> original_word;
    int loop_position = 0;

    //vector<int> titles;
    //vector<int> pesos;

};

struct CTrie {

    vector<Node> nodes;
    vector<Entry> lists;

    void print_entry(const char* word) {

        int id = find(word);
        if (id == EMPTY) {
            cout << word << "\t = **" << endl;
            return;
        }

        Entry& entry = lists[id];
        string name;
        for (byte b : entry.original_word)
            name += TO_ASCII[b];
        
        cout << word << "\t = " << name << endl;

    }

    // Return -1 if it coudn't find word
    int find(const char* word) {

        int current_node = 0;
        
        vector<byte> vec = Convert_ISO(word);
        const byte* data = vec.data();

        for (; *data != END; data++) {

            if(nodes[current_node].next[*data] == EMPTY)
                //return -1;
                return nodes[current_node].next[END];
            
            current_node = nodes[current_node].next[*data];

        }

        return nodes[current_node].next[END];

    }

    void percorre_ate_proximo_vazio(const byte** data, int* node) {

        for (; **data != END; (*data)++) {
            
            if (nodes[*node].next[**data] == EMPTY)
                return;
            
            *node = nodes[*node].next[**data];

        }

        // Aqui **data = END

        // Chegou no fim da lista e tem uma palavra lá
        if(nodes[*node].next[END] != EMPTY)
            *data = nullptr;

    }
    // nodes[node].next[*data] will be empty

    // Arrebenta next[id] de node
    size_t arrebenta(byte id, int node) {

        size_t position = nodes.size();
        nodes.push_back(nodes[node]);
        nodes[node].next[id] = position;

        // Refaz os loops
        for (int i = 0; i < END; i++)
            if (nodes[position].next[i] == node)
                nodes[position].next[i] = position;
        
        return position;

    }

    // Adapta a trie e retorna o nó em que a palavra pode ser inserida
    int insert(const byte* data, int node) {

        const byte* empty_data = data;
        int empty_node = node;
        percorre_ate_proximo_vazio(&empty_data, &empty_node);
        //percorre_ate_proximo_no()

        // Se achou um nó vazio, procura outro até o fim
        if (empty_data) {

            // E esse nó é o fim
            if (*empty_data == END) {
                // Insere a palavra aqui
                return empty_node;
            }
            // Se esse nó não é o fim
            else {

                // Cria loop
                nodes[empty_node].next[*empty_data] = empty_node;

                const byte* next_data = empty_data;
                int next_node = empty_node;
                percorre_ate_proximo_vazio(&next_data, &next_node);

                // Se não tem nó vazio, desiste do loop
                if (!next_data) {
                    nodes[empty_node].next[*empty_data] = nodes.size();
                    nodes.push_back(Node());
                }

                return insert(empty_data, empty_node);

            }

        }
        // Se não encontrou nó vazio, tem que arrebentar ciclos
        else {

            // v2 está em empty node
            int id = nodes[empty_node].next[END];
            Entry& entry = lists[id];

            if (0) {
                string name1;
                for (byte b : entry.original_word)
                    name1 += TO_ASCII[b];
        
                string name2;
                for (const byte* b = data; *b != END; b++)
                    name2 += TO_ASCII[*b];
        
                cout << name1 << " está no caminho de " << name2 << endl;
            }

            // Quero adicionar v1 mas v2 está atrapalhando
            const byte* v1 = data;
            const byte* v2 = entry.original_word.data() + entry.loop_position;
            byte destination = END;


            // Identificar caracteres únicos
            Clean_Free_Buffer();
            for (const byte* b = v2; *b != END; b++) {
                Free_Buffer[*b] = 2;
            }

            for (const byte* b = v1; *b != END; b++) {
                if (Free_Buffer[*b] == 0){
                    destination = *b;
                    break;
                }
                Free_Buffer[*b] = 1;
            }

            // v1 tem um caractere que v2 não tem
            if (destination != END) {

                // Arrebenta nó destination e pôe v1 no destino
                size_t position = arrebenta(destination, empty_node);
                nodes[position].next[END] = EMPTY;

                return position;



            } else {

                for (const byte* b = v2; *b != END; b++) {
                    if (Free_Buffer[*b] == 2){
                        destination = *b;
                        break;
                    }
                }

                // v2 tem um caractere que v1 não tem
                if (destination != END) {

                    size_t position = arrebenta(destination, empty_node);
                    nodes[position].next[END] = nodes[empty_node].next[END];
                    nodes[empty_node].next[END] = EMPTY;

                    return empty_node;

                // v1 e v2 têm os mesmos caracteres
                } else {

                    return -1;

                }
            }
        }

        // Can't reach here
        return -1;
    }

    int max_words = 100;

    void add_word(const char* word, char end = '\0') {

        Entry entry;
        entry.original_word = Convert_ISO(word, end);
        if (entry.original_word.size() <= 1)
            return;
        /*if (max_words <= 0)
            return;*/
        
        max_words--;
        
        int node = insert( entry.original_word.data(), 0 );
        if (node == -1) {
            //cout << word << " not inserted" << endl;
        } else {
            //cout << word << " inserted" << endl;
            nodes[node].next[END] = lists.size();
            lists.push_back(entry);
        }

    }

    CTrie() {

        nodes.push_back(Node());
    
    }

};

/*char Words[][32] = {
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
};*/

char Words[][32] = {
    "a",
    "ab",
    "abb",
    "ac",
    "so",
    "ao",
};

void wait_input(CTrie& trie) {
    
    while(1) {

        string word;
        printf(">> ");
        cin >> word;

        trie.print_entry(word.c_str());

    }
}

int main1() {

    CTrie trie;
    string file_name;

    for (int i = 0; i < 3; i++) {
        
        file_name = "database/db";
        file_name += to_string(i);
        cout << file_name << " ";

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

        cout << trie.nodes.size() << endl;
    
    }

    cout << trie.nodes.size() << " nodes" << endl;
    wait_input(trie);
}


int main2() {

    CTrie trie;

    for (int i = 0; i < sizeof(Words) / 32; i++)
        trie.add_word(Words[i]);

    cout << trie.nodes.size() << " nodes" << endl;

    for (int i = 0; i < sizeof(Words) / 32; i++)
        trie.print_entry(Words[i]);

    return 0;

}

int main() {

    return main2();

}