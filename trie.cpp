#include <iostream>
#include <fstream>

struct Trie {

    struct Node {

        int weight = 0;
        int child_count = 0;
        void* data = nullptr;
        Node* children[256];

        Node() {

            for (int i = 0; i < 256; i++)
                children[i] = nullptr;

        }

        ~Node() {

            for (int i = 0; i < 256; i++)
                if (children[i])
                    delete children[i];

        }

        void size(int* s) {

            (*s)++;

            for (int i = 0; i < 256; i++)
                if (children[i])
                    children[i]->size(s);

        }

    };
    
    Node* root;
    int words = 0;
    
    Trie() {

        root = new Node();

    }

    ~Trie() {

        delete root;

    }

    int count_nodes() {

        int s = 0;
        root->size(&s);
        return s;

    }

    int add_word(const char* word) {

        const unsigned char* c = (const unsigned char*)(const void*)word;

        Node** current = &root;

        for (; *c != '\0'; c++) {

            if (!(*current)->children[*c]) {

                (*current)->children[*c] = new Node();
                (*current)->child_count++;
            
            }

            current = &((*current)->children[*c]);

        }
        
        if ((*current)->weight == 0)
            this->words++;
        
        (*current)->weight++;

    }

};

int main2() {

    Trie trie;

    trie.add_word("batata");
    trie.add_word("batata");
    trie.add_word("banana");

    trie.add_word("pikachu");
    trie.add_word("pokemon");
    trie.add_word("pirapora");

    std::cout << trie.words << std::endl;
    std::cout << trie.root->child_count << std::endl;
    std::cout << trie.root->children['p']->child_count << std::endl;

    std::cout << trie.count_nodes() << std::endl;

    return 0;

}

int main() {

    Trie trie;

    std::ifstream file("database_tag/db0");
    std::string line;
    while (std::getline(file, line)) {
        
        std::string word = line.substr(0, line.find(" "));;
        //std::cout << word << std::endl;
        trie.add_word(word.c_str());
    
    }
    file.close();

    std::cout << trie.words << " words" << std::endl;
    std::cout << trie.count_nodes() << " nodes" << std::endl;

    return 0;

}