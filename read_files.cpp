#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstdlib>

using namespace std;

void* get_file(string name) {

    ifstream file(name, ios::binary | ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    void* data = malloc(size);
    if (!data)
        return nullptr;

    if (file.read((char*)data, size)) {
    
        return data;        
    
    }

    free(data);
    return nullptr;

}

struct Title_Data {

    int32_t offset, database, database_offset;

};

Title_Data* titles_data;
char* titles_names;


string get_title_name(int id) {

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

    titles_data = (Title_Data*) get_file("titles/titles_data");
    titles_names = (char*) get_file("titles/titles_names");

    if (!titles_data || !titles_names)
        return -1;

    int n = 123;
    if (argc > 1)
        n = atoi(argv[1]);

    string title_n = get_title_name(n);
    printf("%s\n", title_n.c_str());

    free(titles_data);
    free(titles_names);

    return 0;

}