/*
 The following is a program for managing a vehicle database. It allows users to search for vehicles based on
 criteria like year, color, mileage, and price. Users can also simulate buying and selling transactions. To use it the
 format is "a.out pov_database.txt manufacturer.txt transaction.txt". The code uses linked lists for organizing
 vehicle and model data and includes helper functions for tasks like VIN code mappings. Overall, it is a solution for
 vehicle data management and transaction simulation.

 Created by Matthew Nover for ENEE 150
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pov.h"

int transactionNum = 0;
int CodeForYear(char key);
int YearForCode(int year);
char* CodeForBrand(char *VIN3, MODEL *Model_data);
char* BrandForCode(char *Name, MODEL *Model_data);
VEHICLE * VehicleYear(VEHICLE *p, int k);

// MAIN //
int main(int argc, char *argv[]){
    FILE *pov_input, *manu_input, *out;

    pov_input = fopen(argv[1], "r");
    manu_input = fopen(argv[2], "r");
    out = fopen(argv[3], "w");

    // checking files
    if (argc != 4){
        printf("Usage: a.out pov.txt manufacturer.txt transaction.txt\n");
        exit(0);
    } if (pov_input == NULL){
        printf("POV database \"%s\" not found ...\n", argv[1]);
        exit(0);
    } if (manu_input == NULL){
        printf("manufacturer identifier file \"%s\" cannot open ...\n", argv[2]);
        exit(0);
    } if (out == NULL){
        printf("transaction record file \"%s\" cannot open ...\n", argv[3]);
        exit(0);
    }

    // creating lists
    All_Models = GetModel(manu_input);
    fclose(manu_input);

    All_Vehicles = GetVehicle(pov_input);
    fclose(pov_input);

    // menu selection
    int selection;
    do{
        printf("Welcome to my Pre-Owned Vehicle Database!\n\n");
        printf("1: search and buy\n2: compare and sell\n0: EXIT\n\n");
        printf("Enter your choice (0-2): ");
        scanf("%d", &selection);

        if (selection == 1)
            search_and_buy(out); // customer is buying
        if (selection == 2)
            compare_and_sell(out); // customer is selling
    } while(selection != 0);

    printf("Thank you for visiting my Pre-Owned Vehicle Database!\n");

    fclose(out);
    return 0;
}


// HELPER FUNCTIONS //
// creates a new pointer for vehicles
VEHICLE* new_vehicle(){
    VEHICLE* ptr = NEW(VEHICLE);
    if (ptr == NULL)
        exit(0);

    ptr->info = NULL;
    return ptr;
}

MODEL* new_model(){
    MODEL* crt = NEW(MODEL);
    if (crt == NULL)
        exit(0);

    crt->info = NULL;
    return crt;
}

// the following data allows me to find the year correlated to C10 in vin
typedef struct{
    char key;
    int year;
} YearMapping;

YearMapping years[] = {
        {'S', 1995}, {'T', 1996}, {'V', 1997}, {'W', 1998}, {'X', 1999}, {'Y', 2000}, {'1', 2001}, {'2', 2002},
        {'3', 2003}, {'4', 2004}, {'5', 2005}, {'6', 2006}, {'7', 2007}, {'8', 2008}, {'9', 2009}, {'A', 2010},
        {'B', 2011}, {'C', 2012}, {'D', 2013}, {'E', 2014}, {'F', 2015}, {'G', 2016}, {'H', 2017}, {'J', 2018},
        {'K', 2019}, {'L', 2020}, {'M', 2021}, {'N', 2022}, {'P', 2023}, {'R', 2024}
};

// all vehicle colors
const char Color[12][10] = {"Red", "Blue", "White", "Silver", "Orange", "Yellow",
                            "Black", "Lime", "Green", "Oliver", "Purple", "Gray"};

// uses the vin C10 to get the year using the above table
int CodeForYear(char key){
    int i;

    for (i = 0; i < 30; i++){
        if (years[i].key == key)
            return years[i].year;
    }

    return -1;
}

// returns the vin number according to year
int YearForCode(int year){
    int i;

    for (i = 0; i < 30; i++){
        if (years[i].year == year)
            return years[i].key;
    }

    return 'I';
}

// breaks down vin, uses the first 3 numbers and gets a brand name from the Model pointers
char* CodeForBrand(char *VIN3, MODEL *Model_data){

    while (Model_data != NULL){
        if (Model_data->code[0] == VIN3[0] && Model_data->code[1] == VIN3[1] && Model_data->code[2] == VIN3[2])
            return Model_data->name;

        Model_data = Model_data->info;  // moving to the next node
    }

    return NULL;
}

// this returns a vin if there is a match in name
char* BrandForCode(char *Name, MODEL *Model_data){

    while (Model_data != NULL){
        if (strcmp(Name, Model_data->name) == 0)
            return Model_data->code;

        Model_data = Model_data->info;  // moving to the next node
    }

    return NULL;
}


//REQUIRED FUNCTIONS
// this function creates a linked list for 3 letter abbreviation with their names
MODEL * GetModel(FILE *fp){
    static MODEL *manu;
    MODEL *tmp, *cur;
    int i, j;
    char ch;

    i = 0;
    while(fscanf(fp, "%c", &ch)!= EOF){
        tmp = new_model();

        tmp->code[0] = ch;
        fscanf(fp, "%c%c", &(tmp->code[1]), &(tmp->code[2]));
        tmp->code[3] = '\0';

        fscanf(fp, "%c", &ch);	// skip the space after code
        j = 0;			        // gent model name[]
        do{
            if (fscanf(fp, "%c", &ch) == EOF)
                break;

            if (isalpha(ch) || ch == ' ') {
                tmp->name[j] = ch;
                j++;
            }
        } while(ch != '\n');

        tmp->name[j] = '\0';
        tmp->info = NULL;		// last MODEL in the list

        if (i==0)
            manu = tmp;	        // manu is the head of the list
        else
            cur->info = tmp;
        cur = tmp;			    // stay at the current tail of the list
        i++;
    }

    return manu;
}

// goes through the car database and creates linked lists
VEHICLE * GetVehicle(FILE *fp){
    static VEHICLE *manu;
    int i, j;
    char ch;
    VEHICLE *tmp, *cur;
    tmp = new_vehicle();

    i = 0;
    while((ch = fgetc(fp))!= EOF){

        // saving vin number
        j = 0;
        do {
            tmp->VIN[j++] = ch;
        } while((ch = fgetc(fp))!= ';');
        tmp->VIN[j] = '\0';

        // saving color
        j = 0;
        while((ch = fgetc(fp))!= ';')
            tmp->color[j++] = ch;
        tmp->color[j] = '\0';

        // saving mileage
        fscanf(fp, "%d", &(tmp->mileage));
        ch = fgetc(fp); ch = fgetc(fp); // skips the $ and ;

        // saving price
        j = 0;
        char buffer[60];
        while((ch = fgetc(fp))!= '\n' && ch != EOF){
            if (isdigit(ch) || ch == '.')
                buffer[j++] = ch;
        }
        buffer[j] = '\0';
        tmp->price = strtof(buffer, NULL);

        if (i == 0)
            manu = tmp;         // manu is the head of the list
        else
            cur->info = tmp;
        cur = tmp;              // stay at the current tail of the list

        i++;
        tmp = new_vehicle();
    }

    return manu;
}

// prints vehicle information to file and output
void PrintVehicle(VEHICLE *p, FILE *fp){
    int year = CodeForYear(p->VIN[10]);
    char *CarName = CodeForBrand(p->VIN, All_Models);

    // format should be: 1999 Red Dodge car, 180495 miles, 2750.00
    printf("%d %s %s, %d, %.2f\n", year, p->color, CarName, p->mileage, p->price);
    fprintf(fp, "%d %s %s, %d, %.2f\n", year, p->color, CarName, p->mileage, p->price);
}

// uses a loop and the printvehicle function to print every vehicle in the database
void PrintAllVehicles(VEHICLE *p, FILE *fp){

    while (p != NULL){
        PrintVehicle(p, fp);
        p = p->info;  // moving to the next node
    }
}

// returns a linked list of all vehicles that are a certain year
VEHICLE * VehicleYear(VEHICLE *p, int k){
    VEHICLE* manu = new_vehicle();
    VEHICLE *cur = NULL, *tmp = NULL;

    while (p != NULL){
        int year = CodeForYear(p->VIN[10]);

        if (year == k){
            tmp = new_vehicle();
            *tmp = *p;
            tmp->info = NULL;

            if (cur == NULL){
                *manu = *tmp;
                cur = manu;
            } else{
                cur->info = tmp;
                cur = tmp;
            }
        }

        p = p->info;
    }
    return manu;
}

void search_and_buy(FILE *fp){
    int SearchKey, listotal, i;
    char ch;
    VEHICLE *tmp = NULL, *cur = NULL, *result_head = new_vehicle(), *temphead;
    *result_head = *All_Vehicles;

    // gathering filter information
    do {
        printf("Choose a following filter by its corresponding digit:\n\n\
           1. year of make: What year the vehicle was made in (ex. 2006).\n\
           2. color: What color the vehicle is (ex. Red).\n\
           3. mileage: How many miles are on the vehicle (ex. 20000 miles).\n\
           4. price: How much the vehicle costs (ex. $10,000).\n\
           5. manufactorer: What brand the vehicle is (ex. Ford truck).\n");
        scanf("%d", &SearchKey);

        switch (SearchKey){
            // establishing the filter year
            case 1:
                printf("Enter a year between 1995-2024: ");
                int year;
                scanf("%d", &year);

                // check if input is invalid
                if (year < 1995 || year > 2024) {
                    printf("\nInvalid entry.\n");
                    exit(0);
                }

                // making changes to list
                result_head = VehicleYear(result_head, year);
                break;

                // establishing the color filter
            case 2:
                printf("Choose color from:\n");

                for (i = 0; i < 12; i++)
                    printf("%d. %s ", i, Color[i]);
                printf("\n");
                scanf("%d", &SearchKey);

                // check if input is invalid
                if (Color[SearchKey] == NULL){
                    printf("Invalid entry");
                    exit(0);
                }

                // making change to list
                temphead = new_vehicle();
                tmp = result_head, cur = NULL;
                for (; tmp->info != NULL; tmp = tmp->info){
                    if (strcmp(tmp->color, Color[SearchKey]) == 0){
                        VEHICLE *cpy = new_vehicle();
                        *cpy = *tmp;
                        cpy->info = NULL;

                        if (cur == NULL){
                            *temphead = *cpy;
                            cur = temphead;
                        } else{
                            cur->info = cpy;
                            cur = cpy;
                        }
                    }
                }
                *result_head = *temphead;
                free(temphead);
                break;

                // establishing the mile filter
            case 3:
                printf("Choose the option by its corresponding digit:\n\
            1. Range of miles (ex. 1000-20000)\n\
            2. Over a certain amount (ex. more than 4000 miles)\n\
            3. Under a certain amount (ex. less than 50000 miles)\n");
                scanf("%d", &SearchKey);
                int min = 0, max = 99999999;

                if (SearchKey == 1){
                    printf("Please enter your range following the format (lower-higher): ");
                    scanf("%d%c%d", &min, &ch, &max);
                } if (SearchKey == 2){
                    printf("Please enter the minimium amount of miles of the vehicle: ");
                    scanf("%d", &min);
                } if (SearchKey == 3){
                    printf("Please enter the maximium amount of miles of the vehicle: ");
                    scanf("%d", &max);
                }

                // check if input is invalid
                if (max < min){
                    printf("\nInvalid Entry\n");
                    exit(0);
                }

                // making change to list
                temphead = new_vehicle();
                tmp = result_head, cur = NULL;
                for (; tmp->info != NULL; tmp = tmp->info){
                    if (tmp->mileage >= min && tmp->mileage <= max){
                        VEHICLE *cpy = new_vehicle();
                        *cpy = *tmp;
                        cpy->info = NULL;

                        if (cur == NULL){
                            *temphead = *cpy;
                            cur = temphead;
                        } else{
                            cur->info = cpy;
                            cur = cpy;
                        }
                    }
                }
                *result_head = *temphead;
                free(temphead);
                break;

                // establishing the price filter
            case 4:
                printf("Choose the option by its corresponding digit:\n\
            1. Range of price (ex $1000 - $2000)\n\
            2. Over a certain amount (ex more than $4000)\n\
            3. Under a certain amount (ex less than $50000)\n");
                scanf("%d", &SearchKey);
                int minf = 0, maxf = 9999999;

                if (SearchKey == 1){
                    printf("Please enter your range following the format (lower-higher): ");
                    scanf("%d%c%d", &minf, &ch, &maxf);
                } if (SearchKey == 2){
                    printf("Please enter the minimium price of the vehicle: ");
                    scanf("%d", &minf);
                } if (SearchKey == 3){
                    printf("Please enter the maximium price of the vehicle: ");
                    scanf("%d", &maxf);
                }

                // check if input is invalid
                if (maxf < minf){
                    printf("\nInvalid Entry\n");
                    exit(0);
                }

                // making change to list
                temphead = new_vehicle();
                tmp = result_head, cur = NULL;
                for (; tmp->info != NULL; tmp = tmp->info){
                    if (tmp->price >= minf && tmp->price <= maxf){
                        VEHICLE *cpy = new_vehicle();
                        *cpy = *tmp;
                        cpy->info = NULL;

                        if (cur == NULL){
                            *temphead = *cpy;
                            cur = temphead;
                        } else{
                            cur->info = cpy;
                            cur = cpy;
                        }
                    }
                }
                *result_head = *temphead;
                free(temphead);
                break;

                // establishing manufacturer filter
            case 5:
                printf("\nEnter the car's manufacturer: ");
                i = 0;
                char c = getchar(), carmanu[40];

                while ((c = getchar()) != '\n'){
                    carmanu[i] = c;
                    i++;
                }
                carmanu[i] = '\0';

                // making change to list
                temphead = new_vehicle();
                tmp = result_head, cur = NULL;
                for (; tmp != NULL; tmp = tmp->info){
                    if (strcmp(CodeForBrand(tmp->VIN, All_Models), carmanu) == 0){
                        VEHICLE *cpy = new_vehicle();
                        *cpy = *tmp;
                        cpy->info = NULL;

                        if (cur == NULL){
                            *temphead = *cpy;
                            cur = temphead;
                        } else{
                            cur->info = cpy;
                            cur = cpy;
                        }
                    }
                }

                *result_head = *temphead;
                free(temphead);
                break;

                // the user input didn't fall within range
            default:
                printf("Invalid entry.\n");
                exit(0);
        }

        // check if the list is under 6 results, if not need to narrow results
        tmp = result_head, listotal = 0;
        for (; tmp->info != NULL; tmp = tmp->info)
            ++listotal;

        if (result_head == NULL){
            printf("\nNo results match your filters.\n");
            exit(0);
        } else if (listotal > 5){
            PrintAllVehicles(result_head, fp);
            printf("\n\nThere are too many results, Please ");
        }
    } while (listotal > 5);

    // print out all of the results
    tmp = result_head, listotal = 1;
    if (tmp->info == NULL){
        printf("\nOption %d: ", listotal);
        PrintVehicle(tmp, fp);
    }else do{
        printf("\nOption %d: ", listotal++);
        PrintVehicle(tmp, fp);
        tmp = tmp->info;
    } while (tmp != NULL);

    printf("\nPlease enter the number that correspondes with the vehicle you want to buy. \nIf you do not wish to buy any, enter 0.\n");
    scanf("%d", &SearchKey);

    // remove the car purchased from the list.
    if (SearchKey > listotal || SearchKey < 0){
        printf("\nInvalid entry.\n");
        exit(0);
    } else if(SearchKey != 0){
        if (SearchKey >= 2){
            i = 1;

            if (result_head->info != NULL){
                do{
                    i++;
                    result_head = result_head->info;
                } while (i < SearchKey);
            }
        }

        VEHICLE *prev = NULL, *save = NULL;
        tmp = All_Vehicles;

        // loops to check if anything in the string matches the criteria given
        i = 0;
        for (; tmp->info != NULL; tmp = tmp->info){
            VEHICLE *next = tmp->info;

            if (i == 0){
                if (strcmp(tmp->VIN, result_head->VIN) == 0){
                    prev = tmp;
                    break;
                }
            } else{
                if (strcmp(next->VIN, result_head->VIN) == 0)
                    prev = tmp;
            }
            i++;
        }

        // removes the value that is the same as others
        if (prev == NULL)
            printf("No car found\n\n");
        else{
            VEHICLE *next;
            if (i == 0)
                next = prev;
            else
                next = prev->info;

            // print all of the information required
            int year = CodeForYear(next->VIN[10]);
            char *CarName = CodeForBrand(next->VIN, All_Models);

            transactionNum++;
            printf("\nA %d %s %s was successfully sold for $%.2f\n", year, next->color, CarName, next->price);
            fprintf(fp, "\nTransaction %d: %d %s %s was sold for $%.2f\n", transactionNum, year, next->color, CarName, next->price);

            if (next->info != NULL){
                save = next->info;
                prev->info = save;
            } else
                prev->info = NULL;
        }
    }
}

void compare_and_sell(FILE *fp){
    int year, mileage, selling;
    char carmanu[40];

    // gathering information for car.
    printf("\nEnter the car's year of make: ");
    scanf("%d", &year);

    printf("\nEnter the car's manufacturer: ");
    int i = 0;
    char c = getchar();
    while ((c = getchar()) != '\n'){
        carmanu[i] = c;
        i++;
    }
    printf("\nEnter the car's mileage: ");
    scanf("%d", &mileage);

    // go through list of all cars to see what has 2/3 matches, then print
    VEHICLE *curr;
    curr = All_Vehicles;
    for (; curr != NULL; curr = curr->info){
        i = 0;

        if (CodeForYear(curr->VIN[10]) == year)
            i++;
        if (strcmp(CodeForBrand(curr->VIN, All_Models), carmanu) == 0)
            i++;
        if ((curr->mileage >= mileage - 5000) && (curr->mileage <= mileage + 5000))
            i++;

        if (i >= 2)
            PrintVehicle(curr, fp);
    }

    // how much the car sells for
    printf("Enter the price you are selling your car for: ");
    scanf("%d", &selling);

    // randomly deciding to purchase the car or not
    srand(time(NULL));
    if ((rand() % 2) == 1){
        VEHICLE *insrt, *tail;
        insrt = NEW(VEHICLE);

        char *VINst = BrandForCode(carmanu, All_Models);

        // fill in information for vehicle
        insrt->mileage = mileage;
        insrt->price = selling;
        insrt->VIN[0] = VINst[0]; insrt->VIN[1] = VINst[1]; insrt->VIN[2] = VINst[2];
        for (i = 3; i < 17; i++){
            if (i == 10)
                insrt->VIN[10] = YearForCode(year);
            else
                insrt->VIN[i] = 'A';
        }

        // add to table
        curr = All_Vehicles;
        for (; curr != NULL; curr = curr->info)
            tail = curr;

        tail->info = insrt;
        insrt->info = NULL;

        // print all the information required.
        year = CodeForYear(insrt->VIN[10]);
        char *CarName = CodeForBrand(insrt->VIN, All_Models);
        transactionNum++;
        printf("\nA %d %s %s was successfully purchased for $%.2f\n", year, insrt->color, CarName, insrt->price);
        fprintf(fp, "\nTransaction %d: %d %s %s was purchased for $%.2f\n", transactionNum, year, insrt->color, CarName, insrt->price);
    } else //purchase failed
        printf("\nPurchase denied.\n");
}
