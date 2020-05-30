#include <iostream>
#include<string.h>
using namespace std;

int main(int argc, char *argv[]){
    string titulos[30];
    for(int i=0;i<30; i++){
        titulos[i]="Título do texto " + to_string(i);
    }
    cout << 18 << endl;
    int index[30]={10,9,8,7,6,5,4,3,2,1,20,19,18,17,16,15,14,13,12,11,30,29,28,27,26,25,24,23,22,21};
    for(int i=0; i<30; i++){
        cout << index[i] << " => ";
        cout << titulos[index[i]-1] << endl;
    }
    return 0;
}