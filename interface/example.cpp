#include <iostream>
#include<string.h>
using namespace std;

int main(int argc, char *argv[]){
    string titulos[20];
    for(int i=0;i<20; i++){
        titulos[i]="Título do texto " + to_string(i);
    }
    cout << 18 << endl;
    int index[3]={10,16,8};
    for(int i=0; i<3; i++){
        cout << index[i] << " => ";
        cout << titulos[index[i]] << endl;
    }
    return 0;
}