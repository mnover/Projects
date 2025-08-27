errorData = {'<unknown>', '', 'null', 'u', 'unknown', '<missing>'};


% initialize + general data cleanse
food = cleanData(readtable("Prompt2F&B.csv"), errorData);
merch = cleanData(readtable("Prompt2Retail.csv"), errorData);
tickets = cleanData(readtable("Prompt2Tickets.csv"), errorData);
demographic = cleanData(readtable("Prompt2Demographics.csv"), errorData);

% remove duped info
[~, mask] = unique(demographic(:, {'AccountNumber'}), 'rows', 'stable');
demographic = demographic(mask, :);

[~, mask] = unique(food(:, {'GuestCheckID'}), 'rows', 'stable');
food = food(mask, :);

[~, mask] = unique(merch(:, {'OrderID'}), 'rows', 'stable');
merch = merch(mask, :);

% remove unnecessary columns
tickets.SeasonKey = [];
food.GuestCheckID = [];
merch.OrderID = [];
merch.ProductTitle = []; % eventually figure out how to extract player name or team name

% demographic manipulation
demographic.Gender(contains(demographic.Gender, 'f')) = {'f'};
demographic.Gender(contains(demographic.Gender, 'm')) = {'m'};
demographic.PresenceOfChildren(contains(demographic.PresenceOfChildren, 'n')) = {'n'};
demographic.PresenceOfChildren(contains(demographic.PresenceOfChildren, 'y')) = {'y'};

% ticket manipulation
firstSpaceIndex = cellfun(@(c) c(1), strfind(cellstr(tickets.MasterEventName), ' '));
teamNames = extractAfter(tickets.MasterEventName, firstSpaceIndex);
tickets.MasterEventName = strtrim(teamNames);

[G, accountIDs] = findgroups(tickets.AccountNumber);
totalDates = splitapply(@(x) strjoin(string(x), ", "), tickets.PurchaseDate, G);
ticketType = splitapply(@(x) strjoin(string(x), ", "), tickets.ProductGroup, G);
teamPlays = splitapply(@(x) strjoin(string(x), ", "), tickets.MasterEventName, G);
totalCost = splitapply(@sum, tickets.InventoryNetAmount, G);
totalUsed = splitapply(@sum, tickets.ScannedFlag, G);
totalBought = splitapply(@sum, tickets.InventoryCount, G);
tickets = table(accountIDs, totalDates, teamPlays, ticketType, totalCost, totalUsed, totalBought, ...
    'VariableNames', {'AccountNumber', 'TicketPurchaseDates', 'OtherTeams', ...
    'TypeOfTicket', 'TotalTicketCost', 'AmountOfTicketsUsed', 'AmountOfTicketsBought'});

% food manipulation
splitWords = cellfun(@(c) strsplit(c), cellstr(food.RevenueCenterName), 'UniformOutput', false);
trimmedNames = cellfun(@(c) strjoin(c(3:end), ' '), splitWords, 'UniformOutput', false);
food.RevenueCenterName = strtrim(trimmedNames);

[G, accountIDs] = findgroups(food.AccountNumber);
totalDates = splitapply(@(x) strjoin(string(x), ", "), food.PurchaseDate, G);
totalRest = splitapply(@(x) strjoin(string(x), ", "), food.RevenueCenterName, G);
totalCost = splitapply(@sum, food.ReportLineAmount, G);
totalItems = splitapply(@sum, food.ReportLineQuantity, G);
food = table(accountIDs, totalDates, totalRest, totalCost, totalItems, ...
    'VariableNames', {'AccountNumber', 'FoodPurchaseDates', 'Restaurants', ...
    'FoodPurchaseCost', 'AmountOfFood'});

% merch manipulation
[G, accountIDs] = findgroups(merch.AccountNumber);
totalDates = splitapply(@(x) strjoin(string(x), ", "), merch.PurchaseDate, G);
totalCat = splitapply(@(x) strjoin(string(x), ", "), merch.CategoryName, G);
totalCost = splitapply(@sum, merch.LineItemTotalPrice, G);
totalItems = splitapply(@sum, merch.LineItemQuantity, G);
merch = table(accountIDs, totalDates, totalCat, totalCost, totalItems, ...
    'VariableNames', {'AccountNumber', 'MerchPurchaseDates', 'MerchCategory', ...
    'MerchPurchaseCost', 'AmountOfMerch'});

% making customer database
clear childrenStr firstSpaceIndex G genderStr mask teamNames teamPlays ticketType ...
        totalBought totalCat totalCost totalDates totalItems totalRest totalUsed accountIDs...
        splitWords trimmedNames

userDataBase = outerjoin(demographic, food, 'Keys', 'AccountNumber');
userDataBase.Properties.VariableNames{'AccountNumber_demographic'} = 'AccountNumber';

userDataBase = outerjoin(userDataBase, merch, 'Keys', 'AccountNumber');
userDataBase.Properties.VariableNames{'AccountNumber_userDataBase'} = 'AccountNumber';

userDataBase = outerjoin(userDataBase, tickets, 'Keys', 'AccountNumber');
userDataBase.Properties.VariableNames{'AccountNumber_userDataBase'} = 'AccountNumber';

userDataBase.AccountNumber_food = [];
userDataBase.AccountNumber_merch = [];
userDataBase.AccountNumber_tickets = [];

% dissected data
% ticket ratio
mask = userDataBase.AmountOfTicketsBought ~= -1 & userDataBase.AmountOfTicketsUsed ~= -1;
AttendanceRate = table(userDataBase.AccountNumber(mask), ...
    userDataBase.AmountOfTicketsUsed(mask) ./ userDataBase.AmountOfTicketsBought(mask), ...
    'VariableNames', {'AccountNumber', 'UsageRatio'});

% impulse amounts
maskFoodOnly = userDataBase.FoodPurchaseCost ~= -1;
maskMerchOnly = userDataBase.MerchPurchaseCost ~= -1;

foodTbl = table(userDataBase.AccountNumber(maskFoodOnly), ...
    userDataBase.FoodPurchaseCost(maskFoodOnly), ...
    'VariableNames', {'AccountNumber', 'TotalPurchase'});

merchTbl = table(userDataBase.AccountNumber(maskMerchOnly), ...
    userDataBase.MerchPurchaseCost(maskMerchOnly), ...
    'VariableNames', {'AccountNumber', 'TotalPurchase'});

totalPurchases = outerjoin(foodTbl, merchTbl, 'Keys', 'AccountNumber', 'MergeKeys', true);

combinedPurchase = sum(table2array(totalPurchases(:, 2:end)), 2, 'omitnan');
totalPurchases.totalSpending = combinedPurchase;
totalPurchases.TotalPurchase_foodTbl = [];
totalPurchases.TotalPurchase_merchTbl = [];

% years with activity
ActivityYears = table(userDataBase.AccountNumber, 'VariableNames', {'AccountNumber'});
foodTokens  = regexp(userDataBase.FoodPurchaseDates, '\d{2}/\d{2}/(\d{4})', 'tokens');
merchTokens = regexp(userDataBase.MerchPurchaseDates, '\d{2}/\d{2}/(\d{4})', 'tokens');
tickTokens  = regexp(userDataBase.TicketPurchaseDates, '\d{2}/\d{2}/(\d{4})', 'tokens');

ActivityYears.Food  = cellfun(@(c) unique(str2double([c{:}])), foodTokens,  'UniformOutput', false);
ActivityYears.Merch = cellfun(@(c) unique(str2double([c{:}])), merchTokens, 'UniformOutput', false);
ActivityYears.Tick  = cellfun(@(c) unique(str2double([c{:}])), tickTokens,  'UniformOutput', false);

cleanYears = @(x) x(~isnan(x));
combineYears = @(f, m, t) unique([cleanYears(f), cleanYears(m), cleanYears(t)]);

ActivityYears.AllYears = cellfun(combineYears, ...
    ActivityYears.Food, ActivityYears.Merch, ActivityYears.Tick, ...
    'UniformOutput', false);

ActivityYears.Food = [];
ActivityYears.Merch = [];
ActivityYears.Tick = [];
ActivityYears.AllYears = string(cellfun(@(c) strjoin(string(c), ","), ActivityYears.AllYears, 'UniformOutput', false));
ActivityYears.AllYears = cellfun(@(x) strrep(x, "", '-1'), ActivityYears.AllYears, 'UniformOutput', false);

% days of activity
A = cellfun(@length, merchTokens);
B = cellfun(@length, foodTokens);

A = table(userDataBase.AccountNumber(userDataBase.FoodPurchaseDates ~= "-1"), ...
    A, 'VariableNames', {'AccountNumber', 'NumFood'});
B = table(userDataBase.AccountNumber(userDataBase.MerchPurchaseDates ~= "-1"), ...
    B, 'VariableNames', {'AccountNumber', 'NumMerch'});

tempJoin = outerjoin(A, B, 'Keys', 'AccountNumber', 'MergeKeys', true);
tempJoin.numTrans = sum(table2array(tempJoin(:, 2:end)), 2, 'omitnan');
tempJoin.NumFood = [];
tempJoin.NumMerch = [];

% eventually add favorite team

clear A B combinedPurchase combTbl commaACount commaBCount mask maskBoth...
    maskFoodOnly maskMerchOnly merchTbl cleanYears combineYears foodTokens ...
    merchTokens tickTokens;

addBase = outerjoin(tempJoin, totalPurchases, 'Keys', 'AccountNumber');
addBase.Properties.VariableNames{'AccountNumber_tempJoin'} = 'AccountNumber';

addBase = outerjoin(addBase, AttendanceRate, 'Keys', 'AccountNumber');
combinedIDs = addBase.AccountNumber_addBase;
combinedIDs(isnan(addBase.AccountNumber_addBase)) = addBase.AccountNumber_AttendanceRate(isnan(addBase.AccountNumber_addBase));
addBase.Properties.VariableNames{'AccountNumber_addBase'} = 'AccountNumber';

addBase.AccountNumber = combinedIDs;
addBase.AccountNumber_totalPurchases = [];
addBase.AccountNumber_AttendanceRate = [];

% combining extracted data with given data
userDataBase = outerjoin(userDataBase, addBase, 'Keys', 'AccountNumber');
userDataBase.Properties.VariableNames{'AccountNumber_userDataBase'} = 'AccountNumber';
userDataBase.AccountNumber_addBase = [];
userDataBase.YearsOfAction = ActivityYears.AllYears;
userDataBase = cleanData(userDataBase, errorData);

% convert everyting to double for easier compilation of clustering
processedDataBase = userDataBase;

processedDataBase.FoodPurchaseDates = [];
processedDataBase.Restaurants = [];
processedDataBase.MerchPurchaseDates = [];
processedDataBase.MerchCategory = [];
processedDataBase.TicketPurchaseDates = [];
processedDataBase.OtherTeams = [];
processedDataBase.TypeOfTicket = [];

numYears = count(processedDataBase.YearsOfAction, ',') + 1;
numYears(processedDataBase.YearsOfAction == "-1") = 0;
processedDataBase.YearsOfAction = numYears;

vars = processedDataBase.Properties.VariableNames;
for v = 1:numel(vars)
    
    if isa(processedDataBase.(vars{v}), 'string')
        [~, temp] = ismember(userDataBase.(vars{v}), unique(userDataBase.(vars{v})));
        processedDataBase.(vars{v}) = temp - 2;
    end
end

clear ActivityYears addBase AttendanceRate combinedIDs demographic food foodTbl ...
    merch tempJoin tickets totalPurchases temp v vars numYears

% python section/ AI imlementation
pyFile = 'C:\Users\mvnov\Hackathon\pythononMatlab\runningstuff.py';
tempFile = 'rawData.csv';

writetable(processedDataBase, tempFile);
cmd = sprintf('python "%s" "%s"', pyFile, tempFile);
system(cmd);

% helper functions
function fintab = cleanData(data, inef) % cleans up missing variables
    vars = data.Properties.VariableNames;

    for v = 1:numel(vars)
        columnData = data.(vars{v});
    
        % missing data
        if iscell(columnData)
            columnData = lower(string(columnData));
            columnData(ismember(columnData, inef)) = {"-1"};
        elseif isnumeric(columnData)
            columnData(isnan(columnData)) = -1;
        elseif isstring(columnData)
            columnData(ismissing(columnData)) = {"-1"};
        end
    
        data.(vars{v}) = columnData;
    end

    fintab = data;
end

function [matrix, u_col1, u_col2] = FindMatrix(col1, col2) % used to test stuff
    col1 = string(col1);
    col2 = string(col2);
    mask = (col1 ~= "-1" & col2 ~= "-1");
    col1 = col1(mask);
    col2 = col2(mask);

    u_col1 = unique(col1);
    u_col2 = unique(col2);

    if size(u_col1) < 7 & size(u_col2) < 7
        count = zeros(numel(u_col1), numel(u_col2));
    
        for i = 1:numel(u_col1)
            for j = 1:numel(u_col2)
                count(i,j) = sum(col1 == u_col1(i) & col2 == u_col2(j));
            end
        end
    
        matrix = count';
    else

        matrix = [col1, col2];
    end
end
