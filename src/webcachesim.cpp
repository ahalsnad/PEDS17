//
// Created by Anusha on 12/14/2016.
//

#include <fstream>
#include <regex>
#include <iostream>
#include "policies/lru_variants.cpp"
#include "policies/gd_variants.cpp"

class webcachesim {
public:
    int cacheSim(int argc, char *argv[]) {
        // output help if insufficient params
        if (argc < 5) {
            cerr << "webcachesim warmUp cacheType log2CacheSize traceFiles" << endl;
            return 1;
        }

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
        cout << "cache Size : " << cache_size;
        webcache->setSize(cache_size);

        ifstream infile;
        long long reqs = 0, bytes = 0;
        long long t, id, size;

        cerr << "running..." << endl;

        /* Process all the input trace files in LRU */
        for (int i = 4; i < argc; i++) {
            const char *path = argv[i];
            bool logStatistics = false;
            long long sub_reqs = 0, sub_bytes = 0;

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
        cout << "done." << endl << "-------" << endl
             << "FILE : " << argv[i] << endl
             << "cache policy: " << cacheType << endl
             << "size (log2): " << sizeExp << endl
             << "requests processed: " << reqs << endl
             << "object hit ratio: " << double(webcache->getHits()) / reqs << endl
             << "byte hit ratio: " << double(webcache->getBytehits()) / bytes << endl;
    }
};
