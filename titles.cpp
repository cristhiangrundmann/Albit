#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

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

struct Title_Data {

    uint32_t offset, database, database_offset;

};

// It's just a string that stores three ints
class Title : public string {

	public:

	Title_Data data;

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

                    title.data.database = i;
                    title.data.database_offset = pos;
                    
                    if (title.find("\"") != string::npos) {

                        cout << title << title.data.database << endl;
                    }

                    title_list.push_back(title);

                }

            }

            db.close();
        
        }

    }

    // Salva todos os títulos em um arquivo
    // O id do título é a posição dele nesse arquivo
    cout << "Saving titles file" << endl;
    file_name = "titles/titles_names";
    ofstream titles(file_name);
    for (Title& title : title_list) {
        title.data.offset = titles.tellp();
        titles << title << '\0';
    }
    titles.close();

    // Salva a tabela para recuperar offset, database e database_offset a partir do id do título
    cout << "Saving offsets" << endl;
    file_name = "titles/titles_data";
    ofstream ids(file_name, ios::binary);
    for (Title& title : title_list) {
        ids.write((char*)&title.data, sizeof(Title_Data));
    }
    ids.close();
    
    // Salva um csv com (offset do título, database que ele está, posição no database que ele está) de cada título
    cout << "Saving offsets csv" << endl;
    file_name = "titles/titles_data.csv";
    ofstream ids_csv(file_name);
    ids_csv << "\"offset\", \"database\", \"database offset\"\n";
    for (Title& title : title_list) {
        ids_csv << title.data.offset << "," << title.data.database << "," << title.data.database_offset << endl;
    }
    ids_csv.close();

    return 0;

}
