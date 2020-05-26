#include <iostream>
#include <vector>
#include <string>

using namespace std;

typedef uint8_t byte;

// Node content
#define EMPTY -1

// Node index
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

byte ISO_8859[256] = {
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

vector<byte> Convert_ISO(const char* word) {

    vector<byte> word_converted;

    for (const byte* c = (byte*)word; *c != '\0'; c++) {
        byte id = ISO_8859[*c];
        if (id != NON)
            word_converted.push_back(id);
    }

    word_converted.push_back(END);
    return word_converted;

}

#define NODE_CHILDS 40

struct Node {

    int next[NODE_CHILDS];
    int num = 0;
    int list = 0;

    Node() {

        for (int i = 0; i < NODE_CHILDS; i++)
            next[i] = EMPTY;

    }

};

struct CTrie {

    vector<Node> nodes;
    vector<string> lists;

    void print_entry(const char* word) {

        int id = find(word);
        if (id == EMPTY)
            cout << word << "\t = **" << endl;
        else
            cout << word << "\t = " << lists[id] << endl;

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

    // Adapta a trie e retorna o nó em que a palavra pode ser inserida
    int insert(const byte* data, int node) {

        const byte* empty_data = data;
        int empty_node = node;
        percorre_ate_proximo_vazio(&empty_data, &empty_node);

        // Se achou um nó vazio
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

                // Não tem nó vazio, desiste do loop
                if (!next_data) {
                    nodes[empty_node].next[*empty_data] = nodes.size();
                    nodes.push_back(Node());
                }

                return insert(empty_data, empty_node);

            }

        }
        else {

            return -1;

        }

    }



    void add_word(const char* word) {

        vector<byte> vec = Convert_ISO(word);
        int node = insert( vec.data(), 0 );
        if (node == -1) {
            printf("Can't insert %s\n", word);
        } else {
            nodes[node].next[END] = lists.size();
            lists.push_back(word);
        }

    }

    CTrie() {

        nodes.push_back(Node());
    
    }

};

char Words[][32] = {
    "abacate",
    "abacaxi",
    "aba",//
    "balao",
    "batata",//
    "tomate",
    "cebola",
    "carne",
    "tombo",
    "trem",
    "trator",
    "pokemon"
};

int main() {

    CTrie trie;

    for (int i = 0; i < sizeof(Words) / 32; i++)
        trie.add_word(Words[i]);

    cout << trie.nodes.size() << " nodes" << endl;

    for (int i = 0; i < sizeof(Words) / 32; i++)
        trie.print_entry(Words[i]);

    return 0;

}