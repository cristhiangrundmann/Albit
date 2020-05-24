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




// Title class #################################
// It's just a string that stores three ints
class Title : public string {

	public:

	int file, pos, id;

    Title(string name) {

        this->assign(name);

    }

};
// #############################################




int main() {

    vector<Title> title_list;
    string file_name;

    title_list.reserve(10000 * 160);

    // Armazena todos os títulos em um vetor de strings
	for (int i = 0; i < 164; i++) {
        
        file_name = "database/db";
        file_name += to_string(i);
        cout << file_name << endl;

        ifstream db(file_name);
        string line;
        if (db.is_open()) {
            
            while (getline(db, line)) {
            
                if (line.compare(0, 5, "<doc ") == 0) {
                    
                    int pos = db.tellg();

                    Title title(get_title(line));
                    title.file = i;
                    title.pos = pos;
                    
                    title_list.push_back(title);

                }

            }

            db.close();
        
        }

    }

    // Ordena eles
    /*cout << "Sorting titles" << endl;
    sort(title_list.begin(), title_list.end());*/

    // Salva todos os títulos em um arquivo
    // O id do título é a posição dele nesse arquivo
    cout << "Saving titles file" << endl;
    file_name = "titles/all_titles";
    ofstream titles(file_name);
    for (Title& title : title_list) {
        title.id = titles.tellp();
        titles << title << endl;
    }
    titles.close();

    // Salva um csv com (id do título, database que ele está, posição no database que ele está) de cada título
    cout << "Saving ids file" << endl;
    file_name = "titles/titles_id";
    ofstream ids(file_name);
    for (Title& title : title_list) {
        ids << title.id << "," << title.file << "," << title.pos << endl;
    }
    ids.close();

    return 0;

}
