# Wave Simulator
## Description:
### A wave simulator that works by simulating a 2D array of springs, that are all linked to each other with other "springs". When one spring is pushed down on with a cursor, it brings the other ones down with it, creating an effect that very nearly simulates a wave. 
## How are all the springs simulated?
### The system is represented by a multi thousand element vector. Then, the differential equation involving that vector can be solved using Runge-Kutta's method for numerically solving differential equations. There's an option to toggle the parallelization of this computation in the physics.h file, which for a larger system may add some benefits in terms of the framerate.
## What libraries were used to make this?
### Raylib for the visualization, all the physics was implemented by me.
![](https://github.com/DmitriiPavlov/waveSimulator/blob/main/wavesimulation.png)
