# Adaptive Simulated Annealing with Greedy Search for the Circle Bin Packing Problem

## data set(benchmark for CBPP-CI)
benchmark_for_CBPP-CI.zip

## Get Packing Solution
Use Visual Studio 2019 to open Project ASA_circular_packing.sln and build the executable file ASA_ circular_ packing.exe (platforms:x86 configurations:release).
Then, run the following command to attain packing solution:
``` 
./ASA_circular_packing.exe output.solution < dataPath/benchmark_for_CBPP-CI/fixed_i/cbpp_circular_fixed_8_40.benchmark
```
## Visualization
```
python plotcircle.py output.solution 1
```