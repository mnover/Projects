# Car Database | C Programming

This project is a car sales tracking system built in C using linked lists and file handling. It was developed as part of a university coursework project 
to explore data structures, dynamic memory allocation, and user interaction in terminal-based applications. 

The codebase was split into main.c (containing the main function) and pov2.c (containing all other functions). For simplicity and easier compilation, all code has been consolidated into main.c. which makes the pov.h redundant.

## Information
- Written in C Programming
- Add, remove, update, and list car entries
- Search cars by model, year, or price
- Save and load data from external .txt file
- Uses linked lists for dynamic memory management
- Includes basic input validation and error handling

## File Structure

```bash
car-database/
├── README.md            # You're here
├── example.md           # The results of a test I ran with manufacturer.txt and pov_database.txt
├── main.c               # The main function
├── manufacturer.txt     # Data file for all the models
├── pov.h                # Header file for the main.c function
└── pov_database.txt     # Data file for all the individual vehicles
```

## What I wish I could have implemented
- Better error capitalization error handling. For example the code thinks "Tesla" and "tesla" are different models. I wanted to make capitalization not matter.
- Whenever you sell your car, you could input the ZIN number or individual characteristics.
- A smart purchasing system. The computer compares the car you are selling to other cars in the database to see if it's a good or bad deal.
