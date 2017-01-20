//
// Created by Anusha on 12/2/2016.
//

/**
 * The class provides function and struct to compute the mean, minimum, maximum and the hit ratio when 10GB data is stored in the cache.
 */

#ifndef SAMPLEPROJECT_SORTVALUES_H
#define SAMPLEPROJECT_SORTVALUES_H

#include <fstream>
#include <sstream>
#include <unordered_map>
#include<iostream>
#include <set>

using namespace std;

class processTraceFile {

public:
    static int processInput(char *inputFile, char *outputMem);
    static int processPopular(char *inputFile, char *inputFile1, char *outputMem);
    static int processLeastPopular(char *inputFile);

    /* struct to contain all the properties of the object*/
    struct Object {
        string key;
        long count;
        long size;

        Object(const string &k, long c, long s) : key(k), count(c), size(s) {}
    };

    /* Sorting function */
    struct greaterThanCount {
        inline bool operator()(const Object &struct1, const Object &struct2) {
            return (struct1.count > struct2.count);
        }
    };

    struct lesserThanCount {
        inline bool operator()(const Object &struct1, const Object &struct2) {
            return (struct1.count < struct2.count);
        }
    };
};

#endif //SAMPLEPROJECT_SORTVALUES_H
