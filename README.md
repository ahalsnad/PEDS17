# PEDS17
Project: Performance Evaluation of Distributed Systems

https://disco.informatik.uni-kl.de/index.php/teaching/project

<h3> Task 1: </h3>

a). Analyze one day of wikipedia request:
- Find most popular object
- Max, Min, Mean object size

b). Determine hit ratios of caching the 10GB most-frequently requested objects.


<h3>Task 2: </h3>

a). Analyze 7 days of (anonymized) Wikipedia requests
- Difference between daily 100 most-popular objects
- Popularity Plot
- Object size histogram
- Object size CDF over time

b). Implement and evaluate Least-Recently-Used (LRU)
- Get the Object Hit Ratio and Byte Hit Ratio for a 10GB LRU cache both for 1 day and 7 days of wikipedia requests
- Common implementation: list + unordered_map
- List keeps order of cached objects
- Map allows to lookup position in list, when object is requested


<h3>Task 3: </h3>

a). Evaluate common caching policies
- Policies: LRU, FIFO, GDSF, LFU-DA, GDS, S2LRU
- Cache capacities: 1GB, 10GB, 30GB, 50GB, 100GB, 500GB, 1000GB
- Measure Object/Byte Hit ratio
- Measure throughput: how many requests per second is each policy able to process? 

b). Literature research: Some more cache policies


<h3>Task 4: </h3>

a). Quantify the problem of one-hit-wonders:
- How many requests go to objects that are only ever requested once
- .. only ever requested twice
- ... only ever requested four times
- ... only ever requested 8, 16, 32, 64, 128 times.


b). Evaluate the FILTER policy to fight the one-hit-wonders problem
- When a miss happens: FILTER admits only if this object has received N requests so far
- For admitted objects: standard LRU policy
- Evaluate hit ratios and throughput
- Analyze throughput and memory consumption

Memory Consumption: Used memusg script which measures peak memory usage 
- https://github.com/jhclark/memusg


<h3>Task 5: </h3>

a). Research bloom filters and counting bloom filters

b). Implement a bloom filter to solve the overhead problem
- Incorporate bloom filter into your filter
- Evaluate memory overhead and hit ratio for different parameters

Start with l=1MB and n=8
- Also check
     i. l=512KB, 2MB, 4MB, 8MB
     ii. n=4, n=16, n=32
