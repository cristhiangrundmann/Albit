#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;






// GET TITLE ###################################

int find_nth(string& str, char pat, int n) {

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

    int begin = find_nth(line, '"', 3);
    int end = find_nth(line, '"', -7);

    return line.substr(begin + 1, end - begin - 1);

}

// #############################################












// LIST ########################################

class Link_List {

    class Node {

        public:

        Node(string val, Node* nxt): value(val), next(nxt){}
        
        ~Node() {
            if (next)
                delete next;
        }

        string value;
        Node* next;

    };

    Node* begin = nullptr;

    public:

    void insert(string value) {

        Node** pos = &begin;

        while (1) {

            if (*pos == nullptr || (*pos)->value > value) {

                *pos = new Node(value, *pos);
                return;

            }

            pos = &(*pos)->next;

        }

    }

    void print() {

        Node** pos = &begin;

        while (1) {

            if (*pos == nullptr)
                return;

            cout << (*pos)->value << endl;
            pos = &(*pos)->next;
            
        }

    }

    ~Link_List() {
        if (begin)
            delete begin;
    }

};

// #############################################









int main() {

    Link_List title_list;

    string line;
    ifstream db("database/db68");
    if (db.is_open()) {

        while (getline(db, line)) {
            
            if (line.compare(0, 5, "<doc ") == 0)
                title_list.insert(get_title(line));

        }

        db.close();
    }

    title_list.print();

    return 0;

}