# Wave Simulator
## Description:
### A wave simulator that works by simulating a 2D array of springs, that are all linked to each other with other "springs". When one spring is pushed down on with a cursor, it brings the other ones down with it, creating an effect that very nearly simulates a wave. 
## How are all the springs simulated?
### The system is represented by a multi thousand element vector. Then, the differential equation involving that vector can be solved using Runge-Kutta's approximation.
![](https://github.com/DmitriiPavlov/waveSimulator/blob/main/wavesimulation.png)
