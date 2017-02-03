//
// Created by Anusha on 12/14/2016.
//

#include <fstream>
#include <regex>
#include <iostream>
#include "policies/lru_variants.cpp"
#include "policies/gd_variants.cpp"
#include <chrono>

class webcachesim {
public:
    int cacheSim(int argc, char *argv[]) {
        // output help if insufficient params
        if (argc < 5) {
            cerr << "webcachesim warmUp cacheType log2CacheSize cacheParams traceFiles" << endl;
            return 1;
        }

        int index = 4;
        // trace properties
        const long warmUp = atol(argv[1]);
        assert(warmUp >= 0);

        // create cache
        const string cacheType = argv[2];
        unique_ptr<Cache> webcache = move(Cache::create_unique(cacheType));
        if (webcache == nullptr)
            return 1;

        // configure cache size
        const double sizeExp = atof(argv[3]);
        const long long cache_size = pow(2.0, sizeExp);
        webcache->setSize(cache_size);


        // parse cache parameters
        regex opexp("(.*)=(.*)");
        cmatch opmatch;
        string paramSummary;

        if (cacheType == "Filter") {
            regex_match(argv[4], opmatch, opexp);
            if (opmatch.size() != 3) {
                cerr << "each cacheParam needs to be in form name=value" << endl;
                return 1;
            }
            webcache->setPar(opmatch[1], opmatch[2]);
            paramSummary += opmatch[2];
            index = 5;
        }

        if (cacheType == "FilterBloom") {
            for (int i = 4; i < 7; i++) {
                regex_match(argv[i], opmatch, opexp);
                if (opmatch.size() != 3) {
                    cerr << "Needs 3 cacheParam: admitValue, noOfHashes and filterLength " << endl;
                    cerr << "each cacheParam needs to be in form name=value" << endl;
                    return 1;
                }
                webcache->setPar(opmatch[1], opmatch[2]);
                paramSummary += opmatch[2];
            }
            index = 7;
        }


        long long reqs = 0, bytes = 0;
        long long t, id, size;

        // record start time
        auto start = std::chrono::high_resolution_clock::now();

        cerr << "running..." << endl;

        /* Process all the input trace files */
        for (int i = index; i < argc; i++) {
            const char *path = argv[i];
            bool logStatistics = false;
            long long sub_reqs = 0, sub_bytes = 0;
            ifstream infile;
            infile.open(path);
            while (infile >> t >> id >> size) {
                // start statistics after warm up
                if (!logStatistics && t > warmUp) {
                    cerr << "gathering statistics..." << endl;
                    logStatistics = true;
                    webcache->startStatistics();
                }

                // log statistics
                if (logStatistics) {
                    reqs++;
                    sub_reqs++;
                    bytes += size;
                    sub_bytes += size;
                }

                // request
                webcache->request(id, size);
            }

            infile.close();

        }
        // record end time
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        long throughput = reqs / diff.count();

        cout << "done." << endl << "-------" << endl
             << "cache policy: " << cacheType << endl
             << "size (log2): " << sizeExp << endl
             << "requests processed: " << reqs << endl
             << "running time: " << diff.count() << endl
             << "object hit ratio: " << double(webcache->getHits()) / reqs << endl
             << "byte hit ratio: " << double(webcache->getBytehits()) / bytes << endl
             << "throughput: " << throughput << endl; //Task 3

        return 0;
    }


};
