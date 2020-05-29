// Albit - PSE

// Gera os arquivos:
// - titles_names -> os títulos em ordem
// - titles_data  -> a posição do título nesse arquivo e nos db

#include "albit.h"

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
        
        file_name = "database3/db";
        file_name += to_string(i);
        cout << file_name << endl;

        ifstream db(file_name);
        string line;
        if (db.is_open()) {
            
            while (getline(db, line)) {

                if (line.compare(0, 4, "#doc") == 0) {
                    
                    int pos = db.tellg();

                    //Title title(get_title(line));
                    Title title(line.substr( 5 , string::npos));

                    title.data.database = i;
                    title.data.database_offset = pos;

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
        titles << title << '\n';
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

    return 0;

}
