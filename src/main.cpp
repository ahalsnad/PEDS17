#include "processTraceFile.h"
#include "webcachesim.cpp"
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {

    /*  // Check number of parameters
     if (argc < 3) {
          return 1;
      }

      cout << "running..." << endl;*/

    /**
     * TASK 1
     */

    /* Process the input trace file */
    /*int x = processTraceFile::processInput(argv[1], argv[2]); */

    /***********************************************END OF TASK 1****************************************************/


    /**
     *  TASK 2 A
     *  Process and compute the differences and values for plotting the graphs.
     */

    /*   char *inputFile = argv[1];
       char *outputMem = argv[2];
       char *inputFile1 = nullptr;
       if(argc == 4) {
           inputFile1 = argv[3];
       }

       int x = processTraceFile::processPopular(inputFile, inputFile1, outputMem);
   */

    /**********************************************END OF TASK 2A *****************************************************/

    /**
     *  TASK 2 B
     *  Simulate LRU cache and evaluate 1day and 7days traces
     */

    webcachesim cache;
    cache.cacheSim(argc, argv);

    return 0;
}