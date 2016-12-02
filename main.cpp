#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include<iostream>
#include <set>
#include "sortValues.h"

using namespace std;

int main(int argc, char *argv[]) {

    // parameters
    if (argc != 3) {
        return 1;
    }

    const char *inputFile = argv[1];
    const char *outputMem = argv[2];

    cout << "running..." << endl;

    int x = sortValues::sortInput(argv[1],argv[2]);


    return 0;
}