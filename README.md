# nFWA simulation code for my PhD dissertation

I used this code to research nFWA algorithm in my PhD dissertation which can predict the heat transfer coefficient function "more accurate" then the other implemented sort of PSO (particle swarm optimization) or FWA (Fireworks) algorithms in my dissertation. The code is incomplete to the effect that some parts are not possible to publish for licensing reason but not influence the nFWA algorithm. These parts are the test cases and the cooling curve calculation code on Nvidia GPU. On the other hand I attached an independent rewritten cooling curve calculation code on Nvidia GPU but this code is not part of the nFWA yet.

Sooner, I will attach my PhD dissertation as well.

### Dependencies:
- Ubuntu 22.04
- gcc
- make
- libconfig++
- nvcc 12 an nvidia gpu driver
- nvidia gpu card with computation capability

### Compile:
- \# make clean
- \# make

### Run to calculate a reference cooling curve:
\# ./bin/cli -p REF -a config/cpu_for_dim_50.cfg -c config/cooling_curve_time.dat -t config/htc_gen_time_50.dat -T config/htc_gen_value_50.dat -o config/cooling_curve_50.dat

### Run the nFWA algorithm:
\# ./bin/cli -p HCP -s nFWA -a ./config/cpu_for_dim_50.cfg -l bests -r 2

The results will be generated at the "results" directory.

Unfortunately there is not any written documentation for the results format yet.
The most important columns are the "F" as Fitness and "Spark" as the calculated HTC function points in the dat files.
