# Bucks Fan Engagement | Python and Matlab
This project was developed for the Milwaukee Bucks Hackathon (Jan–Feb 2025). Our team chose the following prompt: 

"analyze fan behaviors, segment customers based on their journey stage and build predictive models to determine the likelihood of fans progressing to higher levels of engagement"

We leveraged Matlab for data preperation and Python (scikit-learn) for machine learning.


## Workflow
1. MatLab Data Preperation
   - Combined four .csv files into one table with data being associated to user IDs
   - Cleaned and deduplicated records, standardized missing values, and extracted new data like attendance ratio and impulse spending
   - Converted all data to numbers for ease of implementation in Python
   - Exported data to a .csv file
2. Python Modeling and Evaluation
   - Loaded and preprocessed data
   - Implemented Skitlearn
   - Tested outcomes of data


## File Structure
```
Bucks Hackathon/
├── Matlab_dataPacking.m          # The Matlab script
├── README.md                     # You're here
├── rawData.csv                   # Sample dataset from MATLAB
├── runningstuff.py               # The Python script
└── tier_feature_averages.csv     # Tier-wise feature summary for clustering
```


## Implementation Wishlist
- Infer who a user's favorite team & player is through ticket sales and merch purchases
- Fan's growth timeline
- Better data input system
- Attribute impact testing


## Sample Output:
```
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
```
