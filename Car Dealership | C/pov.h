// given by Dr. Qu
#ifndef pov
#define pov

#define NEW(x) (x*)malloc(sizeof(x))

typedef struct vehicle{
    char VIN[18];			// a 17-character vehicle identification number
    char color[10];         // color of the vehicle
    int mileage;			// mileage of the vehicle
    float price;			// price of the vehicle
    void * info;			// you can define additional information about
} VEHICLE;

typedef struct model{
    char code[4];			// a 3-character manufacturer identifier
    char name[40];		    // manufacturer (model) name
    int stock;			    // number of vehicles in the POV database with
    void * info;			// optional information (e.g. next pointer to the "next" model)
} MODEL;

MODEL * All_Models;		    // master list of all manufacturer models
VEHICLE * All_Vehicles;		// master list of all vehicles in the database

void search_and_buy (FILE *);	// searches POV database

void compare_and_sell (FILE *); // compares vehicles in POV

MODEL * GetModel (FILE *); 	    // reads from input file manufacturer.txt & returns all models

VEHICLE * GetVehicle (FILE *);	// read from an input file of car database & returns all vehicles

void PrintVehicle (VEHICLE *, FILE *);
// print the information of the given vehicle in the format:
// 1999 Red Dodge car, 180495 miles, 2750.00

void PrintAllVehicles (VEHICLE *, FILE *);
// Prints all vehicles

#endif
