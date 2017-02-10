//
// Created by Anusha on 12/13/2016.
//

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <assert.h>
#include <random>
#include <math.h>
#include <algorithm>
#include "cache.h"
#include "MurmurHash3.h"

using namespace std;

typedef list<object_t>::iterator list_iterator_t;

/*
  LRU: Least Recently Used eviction
*/

class LRUCache : public Cache {
public:
    // construct and destroy LRU
    LRUCache() : Cache() {}

    ~LRUCache() {}

    // normal cache functions
    virtual bool lookup(const long long cur_req) const {
        return (cache_map.count(cur_req) > 0);
    }

    virtual void evict(const long long cur_req) {
        if (lookup(cur_req)) {
            list_iterator_t lit = cache_map[cur_req];
            cache_map.erase(cur_req);
            current_size -= get<1>(*lit);
            cache_list.erase(lit);
        }
    }

    virtual bool request(const long long cur_req, const long long size) {
        unordered_map<long long, list_iterator_t>::const_iterator it;
        it = cache_map.find(cur_req);
        if (it != cache_map.end()) {
            if (size == get<1>(*(it->second))) {
                // hit and consistent size
                LOG("h", 0, cur_req, size);
                hit(it, size);
                return (true);
            } else { // inconsistent size -> treat as miss and delete inconsistent entry
                evict(cur_req);
            }
        }
        miss(cur_req, size);
        return (false);
    }

protected:
    // list for recency order
    list<object_t> cache_list;
    // map to find objects in list
    unordered_map<long long, list_iterator_t> cache_map;

    // main functionality: deal with hit and miss
    virtual void hit(unordered_map<long long, list_iterator_t>::const_iterator it, const long long size) {
        cache_list.splice(cache_list.begin(), cache_list, it->second);
        Cache::hit(size);
    }

    virtual void miss(const long long cur_req, const long long size) {
        // object feasible to store?
        if (size >= cache_size) {
            LOG("L", 0, size, cache_size);
            return;
        }
        list_iterator_t lit;
        // check eviction needed
        while (current_size + size > cache_size) {
            // evict least popular (i.e. last element)
            lit = cache_list.end();
            lit--;
            long long esize = get<1>(*lit);
            LOG("e", current_size, get<0>(*lit), esize);
            current_size -= esize;
            cache_map.erase(get<0>(*lit));
            cache_list.erase(lit);
        }
        // admit new object
        cache_list.push_front(object_t(cur_req, size));
        cache_map[cur_req] = cache_list.begin();
        current_size += size;
        LOG("a", current_size, cur_req, size);
    }
};

static Factory<LRUCache> factoryLRU("LRU");


/*
  FIFO: First-In First-Out eviction
*/

class FIFOCache : public LRUCache {
public:
    FIFOCache() : LRUCache() {}

    ~FIFOCache() {}

protected:
    virtual void hit(unordered_map<long long, list_iterator_t>::const_iterator it, const long long size) {
        Cache::hit(size);
    }
};

static Factory<FIFOCache> factoryFIFO("FIFO");


/*
  S2LRU
*/

class S2LRUCache : public LRUCache {
public:
    S2LRUCache() : previous(), LRUCache() {}

    ~S2LRUCache() {}

    virtual void setSize(long long cs) {
        overall_cache_size = cs;
        // default parameter choice
        LRUCache::setSize(ceil(overall_cache_size / 2));
        previous.setSize(overall_cache_size - ceil(overall_cache_size / 2));
    }

    virtual void setPar(string parName, string parValue) {
        if (parName == "seg1") {
            const double pC1 = stof(parValue);
            assert(pC1 < 1.0);
            assert(pC1 > 0.0);
            LRUCache::setSize(ceil(overall_cache_size * pC1));
            previous.setSize(overall_cache_size - ceil(overall_cache_size * pC1));
        } else if (parName == "seg2") {
            const double pC2 = stof(parValue);
            assert(pC2 < 1.0);
            assert(pC2 > 0.0);
            previous.setSize(ceil(overall_cache_size * pC2));
            LRUCache::setSize(overall_cache_size - ceil(overall_cache_size * pC2));
        } else {
            cerr << "unrecognized parameter: " << parName << endl;
        }
    }

    virtual void evict(const long long cur_req) {
        previous.evict(cur_req);
        LRUCache::evict(cur_req);
    }

    virtual bool lookup1st(const long long cur_req) {
        return (previous.lookup(cur_req));
    }

    virtual bool lookup2nd(const long long cur_req) {
        return (LRUCache::lookup(cur_req));
    }

    virtual bool request(const long long cur_req, const long long size) {
        if (previous.lookup(cur_req)) { // found in previous layer
            previous.evict(cur_req); //delete in previous
            miss(cur_req, size); //admit to current
            LOG("h", 101, cur_req, size);
            return (LRUCache::request(cur_req, size)); //hit in current
        } else if (LRUCache::lookup(cur_req)) {// found in current
            LOG("h", 102, cur_req, size);
            return (LRUCache::request(cur_req, size)); //hit in current
        }
        if (previous.request(cur_req, size)) { //request in all prev. layers
            Cache::hit(size); // hidden hit
            return (true);
        }
        return (false);
    }

protected:
    long long overall_cache_size;
    LRUCache previous;

    virtual void miss(const long long cur_req, const long long size) {
        // object feasible to store?
        if (size >= cache_size) {
            LOG("error", 0, size, cache_size);
            return;
        }
        list_iterator_t lit;
        // check eviction needed
        while (current_size + size > cache_size) {
            // evict least popular (i.e. last element)
            lit = cache_list.end();
            lit--;
            long long esize = get<1>(*lit);
            LOG("e", current_size, get<0>(*lit), esize);
            previous.request(get<0>(*lit), esize); // move to front of previous cache
            current_size -= esize;
            cache_map.erase(get<0>(*lit));
            cache_list.erase(lit);
        }
        // admit new object
        cache_list.push_front(object_t(cur_req, size));
        cache_map[cur_req] = cache_list.begin();
        current_size += size;
        LOG("a", current_size, cur_req, size);
    }

};

static Factory<S2LRUCache> factoryS2LRU("S2LRU");


/*
  FilterCache (admit only after N requests)
*/

class FilterCache : public LRUCache {
public:
    FilterCache() : npar(2), LRUCache(), reqs(0) {}

    ~FilterCache() {}

    virtual void setPar(string parName, string parValue) {
        if (parName == "n") {
            const long long n = stol(parValue);
            assert(n > 0);
            npar = n;
        } else {
            cerr << "unrecognized parameter: " << parName << endl;
        }
    }

protected:
    uint64_t reqs;
    long long npar;
    unordered_map<long long, long long> filter;

    virtual bool request(const long long cur_req, const long long size) {
        reqs++;
        if(reqs > 10000000)
            std::cerr << cur_req << " c: " << filter[cur_req] << " s " << size << "\n";
        filter[cur_req]++;
        return (LRUCache::request(cur_req, size));
    }

    virtual void miss(const long long cur_req, const long long size) {
        if (filter[cur_req] <= npar)
            return;
        LRUCache::miss(cur_req, size);
    }
};

static Factory<FilterCache> factoryFilter("Filter");


/*
  ThLRU: LRU eviction with a size admission threshold
*/

class ThLRUCache : public LRUCache {
public:
    ThLRUCache() : sthreshold(524288), LRUCache() {}

    ~ThLRUCache() {}

    virtual void setPar(string parName, string parValue) {
        if (parName == "t") {
            const double t = stof(parValue);
            assert(t > 0);
            sthreshold = pow(2.0, t);
        } else {
            cerr << "unrecognized parameter: " << parName << endl;
        }
    }

protected:
    long long sthreshold;

    virtual void miss(const long long cur_req, const long long size) {
        // admit if size < threshold
        bool dec_adm = size < sthreshold;
        if (dec_adm)
            LRUCache::miss(cur_req, size);
    }
};

static Factory<ThLRUCache> factoryThLRU("ThLRU");


/*
  ExpLRU: LRU eviction with size-aware probabilistic cache admission
*/

class ExpLRUCache : public LRUCache {
public:
    ExpLRUCache() : cpar(262144), LRUCache() {} //default value
    ~ExpLRUCache() {}

    virtual void setPar(string parName, string parValue) {
        if (parName == "c") {
            const double c = stof(parValue);
            assert(c > 0);
            cpar = pow(2.0, c);
        } else {
            cerr << "unrecognized parameter: " << parName << endl;
        }
    }

protected:
    default_random_engine generator;
    double cpar;

    virtual void miss(const long long cur_req, const long long size) {
        // admit to cache with probablity that is exponentially decreasing with size
        double admissionprob = exp(-size / cpar);
        bernoulli_distribution admissioncoin(admissionprob);
        const bool dec_adm = (admissioncoin(generator));
        if (dec_adm)
            LRUCache::miss(cur_req, size);
    }
};

static Factory<ExpLRUCache> factoryExpLRU("ExpLRU");

/* Basic Bloom Filter */
class BloomFilter {
public:
    BloomFilter() {}

    void bloomFilterInsert(long long key) {
        uint64_t out[2];

        MurmurHash3_x64_128(&key, sizeof(key), 0x6384BA69, &out);
        uint64_t h1 = out[0];
        uint64_t h2 = out[1];
        //        std::cerr << "key " << key << " h1 " << h1 << " h2 " << h2 << std::endl;
        for (uint i = 0; i < k; i++) {
            uint hash1 = (h1 + i * h2) % l;
            if (array[hash1] < 30)
                array[hash1] += 1;
        }
    }

    int bloomFilterRetrieve(long long key) {
        uint64_t out[2];
        uint8_t result[k];

        MurmurHash3_x64_128(&key, sizeof(key), 0x6384BA69, &out);
        uint64_t h1 = out[0];
        uint64_t h2 = out[1];
        uint8_t minCount = 30; // maxcount see line 347 above
        //        std::cerr << "key " << key << " h1 " << h1 << " h2 " << h2 << std::endl;
        for (uint i = 0; i < k; i++) {
            const uint hash1 = (h1 + i * h2) % l;
            const uint8_t x = array[hash1];
            minCount = x < minCount ? x : minCount;
        }
        return minCount;
    }

    void setL(uint l) {
        BloomFilter::l = l;
        array = new uint8_t[l];
        for (uint i = 0; i < l; i++) {
            array[i] = 0;
        }
    }

    void setK(uint k) {
        BloomFilter::k = k;
    }

protected:
    uint l, k;
    uint8_t *array;
};


/* FilterBloomCache (admit only after N requests using basic Bloom Filter)*/
class FilterBloomCache : public LRUCache {
public:
    FilterBloomCache() : npar(2), filter(), LRUCache(), reqs(0) {
    }

    ~FilterBloomCache() {}

    virtual void setPar(string parName, string parValue) {
        if (parName == "n") {
            const int n = stoi(parValue);
            assert(n > 0);
            npar = n;
        } else if (parName == "k") {
            int k = stoi(parValue);
            assert(k > 0);
            filter.setK(k);
        } else if (parName == "l") {
            int l = stoi(parValue);
            assert(l > 0);
            filter.setL(l);
        } else {
            cerr << "unrecognized parameter: " << parName << endl;
        }
    }

protected:
    uint64_t reqs;
    int npar;
    BloomFilter filter;

    virtual bool request(const long long cur_req, const long long size) {
        reqs++;
        if(reqs > 10000000) {
            const auto reqCount = filter.bloomFilterRetrieve(cur_req);
            std::cerr << cur_req << " c: " << reqCount << " s " << size << "\n";
        }
        filter.bloomFilterInsert(cur_req);
        return (LRUCache::request(cur_req, size));
    }

    virtual void miss(const long long cur_req, const long long size) {
        const auto reqCount = filter.bloomFilterRetrieve(cur_req);
        if (reqCount <= npar)
            return;
        LRUCache::miss(cur_req, size);
    }
};

static Factory<FilterBloomCache> factoryFilterBloom("FilterBloom");
