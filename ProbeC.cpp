#include <iostream>
#include <stdlib.h>
#include <chrono>

using namespace std;

int alpha = 997;
int beta = 257;
int rho = 251;

int value;

//need to change so that it returns a num divisible by its magic seed (rho)
int generateValue() {
    int num;
    num = rand();
    return num;
}

void sendToHub(int num) {

}

//change so that it will terminate using kill patch file
bool terminate(int num) {
    if (num < 50)
        return true;
    return false;
}

int main() {
    while (!terminate(100)) {
        //produce reading
        value = generateValue();
        sendToHub(value);
        
    }
}