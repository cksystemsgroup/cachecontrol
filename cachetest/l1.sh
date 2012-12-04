#!/bin/bash

perf stat -e L1-dcache-loads -e L1-dcache-load-misses -e L1-dcache-store-misses ./cachetest
