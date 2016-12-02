//
// Created by Anusha on 12/2/2016.
//
#include "sortValues.h"

int sortValues::sortInput(char *inputFile, char *outputMem) {
    ifstream infile(inputFile);
    ofstream outfile(outputMem);
    string line[13], key;
    long size, t = 0;
    long min = 2147483647, max = 0, sum = 0, highest = 0;
    unordered_map<string, long> dSimpleId;

    const char row_delim = '\n';
    const char field_delim = ' ';
    string row;

    getline(infile, row, row_delim);

    for (; getline(infile, row, row_delim);) {

        // parse row
        istringstream ss(row);
        string id1;
        string id2;
        // 2nd and 3rd field hold id
        getline(ss, id1, field_delim);
//        getline(ss, id1, field_delim);
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

    for (auto it = dSimpleId.begin(); it != dSimpleId.end(); ++it)
        if (highest < it->second) {
            key = it->first;
            highest = it->second;
        }

    outfile << "Mean = " << sum / t << "    Minimum = " << min << "     Maximum = " << max << "        Popular object : " << key
            << "        Accessed " << highest << " times";
    infile.close();

    cout << "completed successfully " << endl;
    cout << "Mean = " << sum / t << endl << "Minimum = " << min << endl << "Maximum = " << max << endl
         << "Popular object : " << key << endl
         << "Accessed " << highest << " times" << endl;
    return 0;
}