#include <fstream>
#include <string>
#include <iostream>

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

    int titles = 0;

    for (int i = 0; i < 164; i++) {
        
        string in_name("database2/db");
        in_name += to_string(i);
        cout << in_name << endl;

        string out_name("database3/db");
        out_name += to_string(i);
        
        ifstream db_in(in_name);
        ofstream db_out(out_name);
        
        string line;
        string title;
        if (db_in.is_open()) {

            bool inside_article = false;

            while (getline(db_in, line)) {

                if (line.compare(0, 5, "<doc ") == 0) {

                    title = get_title(line);
                    
                    if (inside_article) {
                        //cout << "ERROR - found <doc inside article - " << title << endl;
                    }

                    db_out << "#doc " << title << '\n';
                    inside_article = true;
                    titles++;
                    continue;

                }
                
                if (line.size() == 0) {
                    db_out << '\n';
                    continue;
                }

                // May never happen
                if (line.compare(0, 5, "</doc") == 0) {
                    //cout << "ERROR - found </doc inside article - " << title << endl;
                    continue;
                }

                if (line.compare(0, 12, "ENDOFARTICLE") == 0) {
                    inside_article = false;
                    continue;
                }

                db_out << line << '\n';

            }

            if (inside_article) {
                //cout << "ERROR - EOF inside article - " << title << endl;
            }

            db_in.close();
            db_out.close();
        
        }

    }

    cout << titles << endl;

}