//
// Created by Anusha on 12/2/2016.
//
#include <vector>
#include <algorithm>
#include "processTraceFile.h"

/**
 * Function to process the input trace. Computes min, max, mean, hit-ratio and writes it into an output file.
 * @param inputFile trace file input.
 * @param outputMem output file.
 * @return zero on successful execution.
 */
int processTraceFile::processInput(char *inputFile, char *outputMem) {
    /* variables to hold input and output files */
    ifstream infile(inputFile);
    ofstream outfile(outputMem);
    /* Variables to hold the values from input file */
    string key;
    long size;
    /* Variables to store the mean, minimum, maximum and hit-ratio */
    long t = 0, res_size = 0, noReq = 0;
    long min = 2147483647, max = 0, sum = 0, highest = 0;
    double hitRatio = 0.000;
    /* Unordered map to hold key-value pair from the input file */
    unordered_map<string, long> dSimpleId, sizeMap;
    /* Delimiters */
    const char row_delim = '\n';
    const char field_delim = ' ';
    string row;
    /* Vector to store the triple */
    vector<Object> vec;

    getline(infile, row, row_delim);

    /* Populate the maps and compute the mean, min and max */
    for (; getline(infile, row, row_delim);) {

        // parse row
        istringstream ss(row);
        string id1;
        string id2;
        // 2nd field holds id
        getline(ss, id1, field_delim);
        getline(ss, id2, field_delim);
        string id = id2;

        string field;
        for (int i = 3; i <= 10; i++)
            getline(ss, field, field_delim);
        stringstream fieldstream(field);
        fieldstream >> size;

        if (size < 1)
            continue;

        if (dSimpleId.count(id) == 0) {
            dSimpleId[id] = 1;
            sizeMap[id] = size;
            sum += size;
            if (min > size)
                min = size;
            if (max < size)
                max = size;
        } else {
            dSimpleId[id] += 1;
        }
        t++;
    }

    /* Populate the triple into the vector */
    for (auto it = dSimpleId.begin(), it1 = sizeMap.begin(); it != dSimpleId.end(); ++it, ++it1)
        vec.push_back(Object(it->first, it->second, it1->second));
    /*Vector sorting on the count*/
    sort(vec.begin(), vec.end(), greaterThanCount());

    key = vec.at(0).key;
    highest = vec.at(0).count;

    /* Determining 10GB cache objects */
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if ((res_size + it->size) < 10737418240) {
            res_size += it->size;
            noReq += it->count;
        } else {
            break;
        }
    }

    /* Compute hit-ratio */
    hitRatio = (double) noReq / (double) t;

    /* Write results into output file */
    outfile << "Mean = " << sum / t << "    Minimum = " << min << "     Maximum = " << max
            << "        Popular object : " << key
            << "        Accessed " << highest << " times" << "      Hit-rate = " << hitRatio;

    cout << "completed successfully " << endl;

    return 0;
}