#include "processTraceFile.h"

using namespace std;

int main(int argc, char *argv[]) {

    // Check number of parameters
    if (argc != 3) {
        return 1;
    }

    cout << "running..." << endl;

    /* Process the input trace file */
    int x = processTraceFile::processInput(argv[1], argv[2]);


    return x;
}