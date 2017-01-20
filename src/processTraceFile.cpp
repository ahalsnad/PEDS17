//
// Created by Anusha on 12/2/2016.
//
#include <vector>
#include <algorithm>
#include "math.h"
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

/**
 * Process the input trace files to find the 100 most popular objects.
 * Computes the intersection between the popular objects of two input trace files.
 * @param inputFile First input trace file.
 * @param inputFile1 Second input trace file.
 * @param outputMem File to store the 100 popular objects from first trace file and intersected objects from second.
 * @return zero when successful
 *  */
int processTraceFile::processPopular(char *inputFile, char *inputFile1, char *outputMem) {
    /* variables to hold input and output files */
    ifstream infile(inputFile);
    ofstream outfile(outputMem);
    /* Variables to hold the values from input file */
    long size;
    /* To store the intersected objects*/
    int intersectedObjs = 0;
    /* Unordered map to hold key-value pair from the input file */
    unordered_map<string, long> dSimpleId, sizeMap;
    /* Delimiters */
    const char row_delim = '\n';
    const char field_delim = ' ';
    string row;
    /* Vector to store the triple */
    vector<Object> vec, vec1;

    getline(infile, row, row_delim);

    /* Populate the maps with count and size */
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
        } else {
            dSimpleId[id] += 1;
        }
    }

    /* Populate the triple into the vector */
    for (auto it = dSimpleId.begin(), it1 = sizeMap.begin(); it != dSimpleId.end(); ++it, ++it1)
        vec.push_back(Object(it->first, it->second, it1->second));
    /*Vector sorting on the count*/
    sort(vec.begin(), vec.end(), greaterThanCount());
    sizeMap.clear();

    /* Processing the second input file for count values of most popular 100 objects */
    if (inputFile1 != nullptr) {
        ifstream infile1(inputFile1);
        dSimpleId.clear();

        getline(infile1, row, row_delim);

        /* Populate the maps with count */
        for (; getline(infile1, row, row_delim);) {

            // parse row
            istringstream ss(row);
            string id1;
            string id2;
            // 2nd field holds id
            getline(ss, id1, field_delim);
            getline(ss, id2, field_delim);
            string id = id2;

            if (dSimpleId.count(id) == 0) {
                dSimpleId[id] = 1;
            } else {
                dSimpleId[id] += 1;
            }
        }

        /* Populate the triple into the vector */
        for (auto it = dSimpleId.begin(); it != dSimpleId.end(); ++it)
            vec1.push_back(Object(it->first, it->second, 0));
        /*Vector sorting on the count*/
        sort(vec1.begin(), vec1.end(), greaterThanCount());
    }
    dSimpleId.clear();

    /* Determine and write 100 most popular objects into output file
     * This output file results are used to plot the graphs.
     * Also, Computing the intersection among the requests of 2 days
     * */
    if (inputFile1 != nullptr) {
        for (auto it = vec.begin(); it != vec.begin() + 100; ++it) {
            outfile << it->key << ", " << it->count << ", " << it->size << endl;
            for (auto it1 = vec1.begin(); it1 != vec1.begin() + 100; ++it1) {
                if (it->key.compare(it1->key) == 0) {
                    intersectedObjs++;
                    break;
                }
            }
        }
    } else {
        for (auto it = vec.begin(); it != vec.begin() + 100; ++it) {
            outfile << it->key << ", " << it->count << ", " << it->size << endl;
        }
    }

    outfile << "Intersected Objects : " << intersectedObjs << endl;

    cout << "completed successfully " << endl;
    cout << "Intersected Objects : " << intersectedObjs << endl;

    return 0;
}

/**
 * Process and print out the least popular objects
 * @param inputFile input trace file
 * @return 0 on success
 **/
int processTraceFile::processLeastPopular(char *inputFile) {
    /* Vector to hold objects */
    vector<Object> vec;

    /* Unordered map to hold key-value pair from the input file */
    unordered_map<string, long> dSimpleId;

    string row;
    string id;
    long long t, size;

    ifstream infile1(inputFile);

    /* Read through the trace file */
    while (infile1 >> t >> id >> size) {

        if (dSimpleId.count(id) == 0) {
            dSimpleId[id] = 1;
        } else {
            dSimpleId[id] += 1;
        }
    }

    /* Populate the triple into the vector */
    for (auto it = dSimpleId.begin(); it != dSimpleId.end(); ++it)
        vec.push_back(Object(it->first, it->second, 0));
    dSimpleId.clear();

    /*Vector sorting on the count*/
    sort(vec.begin(), vec.end(), lesserThanCount());

    /* Print out the number of least popular objects */
    cout << "Number of Objects requested only : " << endl;

    int i = 0;
    while (i < 8) {
        int count = 0;
        int times = pow(2.0, i);
        cout << times << " times : ";
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (it->count == times) {
                count++;
            } else if (it->count > times) {
                i++;

                break;
            }
        }
        cout << count << endl;

    }
    return 0;
}