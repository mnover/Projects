NOT DONE
# Bucks Fan Engagement | Python and Matlab

This project was developed for the Milwaukee Bucks Hackathon (Jan–Feb 2025). My team was given two prompts, we chose the second prompt which was to "analyze fan behaviors, segment customers based on their journey stage and build predictive models to determine the likelihood of fans progressing to higher levels of engagement".

We chose Matlab to prepare the data for a clustering approach on skitlearn within Python. It combines:

1. MatLab Data Preperation
   - This is my main contribution
   - Combined four .csv files into one table, and standardized data
   - Once data was standardized, extracted useful data and converted all data types to doubles
   - Sent all data to Python through a new .csv file
2. Python Modeling and Evaluation
   - Implemented Skitlearn
   - Tested Outcomes of Data


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
- 

## Example output:
=== Baseline Performance === 
              precision    recall  f1-score   support 
 
           1       0.41      0.90      0.56       503 
           3       0.08      0.27      0.12       358 
           4       0.01      0.05      0.01        58 
           5       0.23      0.84      0.36      3171 
           6       0.96      0.49      0.65     21068 
 
    accuracy                           0.54     25158 
   macro avg       0.34      0.51      0.34     25158 
weighted avg       0.84      0.54      0.60     25158 
 
Confusion Matrix: 
 [[  451    31     0    10    11] 
 [   40    98    14   142    64] 
 [    3    10     3    20    22] 
 [   22   111    66  2649   323] 
 [  580  1027   358  8757 10346]] 
>> 
