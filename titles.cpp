#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;






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









int main() {
    
	for (int i = 0; i < 164; i++) {

        vector<string> title_list;
        
        string name("database/db");
        name += to_string(i);
        cout << name << endl;

        ifstream db(name);
        string line;
        if (db.is_open()) {

            title_list.reserve(10000);
            
            while (getline(db, line)) {
            
                if (line.compare(0, 5, "<doc ") == 0)
                    title_list.push_back(get_title(line));

            }

            db.close();
        
        }

        sort(title_list.begin(), title_list.end());

        string name2("titles/titles");
        name2 += to_string(i);

        ofstream titles(name2);
        for (string title : title_list)
            titles << title << endl;
        
        titles.close();


    }

    return 0;

}
