# Adaptive Simulated Annealing with Greedy Search for the Circle Bin Packing Problem

## Data Set(benchmark for CBPP-CI)
benchmark_for_CBPP-CI.zip

## Get Packing Solution
use Visual Studio 2019 to open project ASA_circular_packing.sln and build the executable file ASA_ circular_ packing.exe (platforms:x86 configurations:release).
Then, run the following command to attain packing solution:
``` 
./ASA_circular_packing.exe output.solution < dataPath/benchmark_for_CBPP-CI/fixed_i/cbpp_circular_fixed_8_40.benchmark
```
## Visualization
```
python plotcircle.py output.solution 1
```

## Related Paper
[Adaptive Simulated Annealing with Greedy Search for the Circle Bin Packing Problem](https://www.sciencedirect.com/science/article/abs/pii/S030505482200106X)

## Citations
If you find this work useful, please consider citing it.
```
@article{YUAN2022105826,
title = {Adaptive simulated annealing with greedy search for the circle bin packing problem},
journal = {Computers & Operations Research},
volume = {144},
pages = {105826},
year = {2022},
issn = {0305-0548},
doi = {https://doi.org/10.1016/j.cor.2022.105826},
url = {https://www.sciencedirect.com/science/article/pii/S030505482200106X},
author = {Yong Yuan and Kevin Tole and Fei Ni and Kun He and Zhengda Xiong and Jingfa Liu},

```