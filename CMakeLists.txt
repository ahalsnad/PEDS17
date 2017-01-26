cmake_minimum_required(VERSION 3.6)
project(SampleProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/webcachesim.cpp src/policies/lru_variants.cpp src/policies/cache.h src/policies/gd_variants.cpp src/processTraceFile.cpp src/processTraceFile.h src/lruCache.h src/main.cpp src/policies/MurmurHash3.cpp src/policies/MurmurHash3.h)
add_executable(SampleProject ${SOURCE_FILES})