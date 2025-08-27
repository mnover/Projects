#  UAV Path Optimization Challenge 
This project is a cross-platform UAV path optimization tool developed for the Northrop Grumman Path Optimization Challenge. We leveraged Python for a data input system and Matlab to process data.


## Workflow
1. A Python GUI (Tkinter) for user-friendly mission input
   - Input mission coordinates (bounding box + UAV starting point)
   - Configure UAV flight specs (height, speed, sensor width, battery life)

2. A MATLAB backend for complex path-planning and performance analytics
   - Coverage pattern planning using uavCoveragePlanner
   - Multi-UAV workload distribution with sweep efficiency modeling
   - Performance statistics include: coverage time, optimized path, 3D kinematics model. (Note: Performance metric code is not included)


##  File Structure
```
Northrop Hackathon/
├── main.py # GUI for mission input
├── main.m # Matlab script for path planning and calculations
└── README.md # This file
```

## Implementation Wishlist
   - Timelapse of path the UAV takes
   - Battery prediction system

## Output Videos
[Test Run 1 (YouTube)](https://www.youtube.com/watch?v=bdUL_DCKCQI)  
[Test Run 2 (YouTube)](https://www.youtube.com/watch?v=2d4qDLctE7w)
