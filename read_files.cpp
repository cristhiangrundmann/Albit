#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstdlib>

using namespace std;

void* Load_on_RAM(string filename) {

    ifstream file(filename, ios::binary | ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (!file.good())
        return nullptr;
    
    void* data = malloc(size);
    if (!data)
        return nullptr;

    if (file.read((char*)data, size)) {   
        file.close();
        return data;
    }

    file.close();
    free(data);
    return nullptr;

}

struct Title_Data {

    int32_t offset, database, database_offset;

};

Title_Data* titles_data;
char* titles_names;

string Get_Title_Name(int id) {

    Title_Data data = titles_data[id];
    
    char* start = &titles_names[data.offset];

    char* end = start;
    while (*end != '\n')
        end++;
    
    string result;
    result.assign(start, end - start);

    return result;

}

int main(int argc, char** argv) {

    titles_data = (Title_Data*) Load_on_RAM("titles/titles_data");
    titles_names = (char*) Load_on_RAM("titles/titles_names");

    if (!titles_data || !titles_names)
        return -1;

    int n = 123;
    if (argc > 1)
        n = atoi(argv[1]);

    string title_n = Get_Title_Name(n);
    printf("%s\n", title_n.c_str());

    free(titles_data);
    free(titles_names);

    return 0;

}