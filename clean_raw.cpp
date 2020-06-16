#include "albit.h"

int main() {

    int titles = 0;

    for (int i = 0; i < 164; i++) {
        
        string in_name("database_raw/db");
        in_name += to_string(i);
        cout << in_name << endl;

        string out_name("database_clean/db");
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