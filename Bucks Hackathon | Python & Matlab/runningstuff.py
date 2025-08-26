import sys
import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.pipeline import Pipeline
from sklearn.compose import ColumnTransformer
from sklearn.impute import SimpleImputer
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix

# inputs
csv_path = sys.argv[1]

# load data
data = pd.read_csv(csv_path, header=0, low_memory=False)
data.replace(-1, np.nan, inplace=True) # i used -1 to represent missing values.

X = data.drop(columns=['AccountNumber', 'CurrentTier'])
y = data['CurrentTier']
mask = y.notna()
X = X.loc[mask]
y = y.loc[mask].astype(int)

# training + split
X_train, X_test, y_train, y_test = train_test_split(
    X, y,
    test_size=0.2,
    stratify=y,
    random_state=42
)
numeric_cols = X.select_dtypes(include=['int64', 'float64']).columns.tolist()
preprocessor = ColumnTransformer(
    transformers=[
        ('num_imp_scale', Pipeline([
            ('imputer', SimpleImputer(strategy='median')),
            ('scaler', StandardScaler())
        ]), numeric_cols),
    ],
    remainder='drop'
)

pipeline = Pipeline([
    ('preprocessor', preprocessor),
    ('classifier', RandomForestClassifier(
        n_estimators=200,
        max_depth=10,
        class_weight='balanced',
        random_state=42
    ))
])

# evaluating
pipeline.fit(X_train, y_train)
y_pred = pipeline.predict(X_test)


print("=== Baseline Performance ===")
print(classification_report(y_test, y_pred))
print("Confusion Matrix:\n", confusion_matrix(y_test, y_pred))

# get specific data
data = data[data['CurrentTier'].notna()]
data['CurrentTier'] = data['CurrentTier'].astype(int)
numeric_cols = data.select_dtypes(include=['int64', 'float64']).columns.tolist()
numeric_cols = [col for col in numeric_cols if col not in ['AccountNumber', 'CurrentTier']]
grouped_means = data.groupby('CurrentTier')[numeric_cols].mean()
grouped_means.to_csv("tier_feature_averages.csv", index=True)
