#  UAV Path Optimization Challenge 
## Overview

This project is a cross-platform UAV path optimization tool developed for the Northrop Grumman Path Optimization Challenge. It combines:
1. A Python GUI (Tkinter) for user-friendly mission input
   - Input mission coordinates (bounding box + UAV starting point)
   - Configure UAV flight specs (height, speed, sensor width, battery life)

2. A MATLAB backend for complex path-planning and performance analytics
   - Coverage pattern planning using uavCoveragePlanner
   - Multi-UAV workload distribution with sweep efficiency modeling
   - Performance statistics include: coverage time, optimized path, 3D kinematics model. (Note: Performance metric code is not included, as it was developed by a teammate)

Once inputs are submitted via the GUI, the data is transmitted to MATLAB for advanced processing, real-time path generation, and performance analysis.


##  File Structure
├── main.py # GUI for mission input
├── main.m # MATLAB script for path planning and calculations
├── README.md # This file

## Examples
[Test Run 1 (YouTube)](https://www.youtube.com/watch?v=bdUL_DCKCQI)  
[Test Run 2 (YouTube)](https://www.youtube.com/watch?v=2d4qDLctE7w)
