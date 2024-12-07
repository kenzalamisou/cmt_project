#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NOM_MAX 50 // maximum number of plants for use in functions
#define MAX_COMPOSITIONS 1000 // maximum number of compositions
#define MAX_PLANTES 100

// function that reads our CSV file with the different plants
int readCharacteristics(char *filename, double *absorptionArray, char plants[][NOM_MAX], double *uPlantArray, int length) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: unable to open the file '%s'\n", filename);
        exit(1);
    }
    // Read and ignore the header
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fprintf(stderr, "Error: empty file or incorrect format.\n");
        fclose(file);
        return -1;
    }
    // Read line by line
    int n = 0;
    while (fgets(buffer, 100, file) != NULL) {
        if (n >= length) break;
        // Split the columns by commas
        char *plant = strtok(buffer, ","); // if the plant name is the first element
        char *absorptionStr = strtok(NULL, ","); // if absorption is the second element
        char *growth = strtok(NULL, ",");
        char *isolation = strtok(NULL, ",");
        char *thermal = strtok(NULL, ",");

        if (plant && absorptionStr && thermal) {
            // Store the plant name
            strncpy(plants[n], plant, NOM_MAX - 1);
            plants[n][NOM_MAX - 1] = '\0'; // Ensure termination

            absorptionArray[n] = atof(absorptionStr); // Convert absorption to double
            uPlantArray[n] = atof(thermal);
            n++;
        } else {
            fprintf(stderr, "Error: malformed line.\n");
        }
    }

    // Close the file and return the number of lines read
    fclose(file);
    return n;
}

// Function to read compositions and calculate percentages
int readCompositions(const char *filename, char knownPlants[MAX_PLANTES][NOM_MAX], 
                     int numKnownPlants, double compositionSurfaces[MAX_COMPOSITIONS][MAX_PLANTES], 
                     int *numCompositions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: unable to open the file '%s'\n", filename);
        return -1;
    }

    char buffer[100];
    int currentLine = *numCompositions; // New composition for this file

    // Initialize the surface for the new composition
    for (int i = 0; i < numKnownPlants; i++) {
        compositionSurfaces[currentLine][i] = 0.0;
    }

    // Read each line of the file and aggregate values
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *value = strtok(buffer, ",");
        int column = 0;

        while (value != NULL && column < MAX_PLANTES) {
            int plantType = atoi(value);
            if (plantType >= 1 && plantType <= numKnownPlants) {
                compositionSurfaces[currentLine][plantType - 1] += 1.0; // Aggregation
            } else {
                fprintf(stderr, "Error: invalid plant type in file %s.\n", filename);
            }
            value = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    *numCompositions += 1; // Add only one composition for this file
    return 0;
}
//Function that calculates 
double CO2Absorption(double *absorptionArray, double compositionSurfaces[MAX_COMPOSITIONS][MAX_PLANTES], 
                     int compositionIndex, int numKnownPlants) {
    double totalAbsorption = 0.0;

    for (int i = 0; i < numKnownPlants; i++) {
        totalAbsorption += absorptionArray[i] * compositionSurfaces[compositionIndex][i];
    }

    return totalAbsorption;
}

double thermalFluxSaved(double surface, double deltaT, double uConcrete, double uPlant) {
    // Check that uConcrete is greater than uPlant
    if (uConcrete <= uPlant) {
        fprintf(stderr, "Error: uConcrete must be greater than uPlant for thermal gain.\n");
        return 0.0;
    }

    // Calculate the saved thermal flux
    double flux = (uConcrete - uPlant) * surface * deltaT;

    return flux;
}

// Function to calculate energy savings in kWh
double calculateSavings(double thermalFlux, double durationHours, double costPerKWh) {
    double powerKW = thermalFlux / 1000.0; // Convert thermal power (W) to kW
    double energySaved = powerKW * durationHours; // Calculate saved energy (in kWh)
    double savings = energySaved * costPerKWh; // Calculate savings (in CHF)
    return savings;
}
// Function to calculate CO2 saved
double CO2Saved(double energySaved) {
    // Use fixed emission factor (25 gCO₂/kWh)
    double emissionFactor = 25.0; // in gCO₂/kWh
    // Convert emission factor from gCO₂/kWh to kgCO₂/kWh
    double emissionFactorKg = emissionFactor / 1000.0;
    // Calculate avoided CO2 in kg
    double avoidedCO2 = energySaved * emissionFactorKg;
    return avoidedCO2;
}

void saveResultsToCSV(const char *filename, int numCompositions, 
                      double *CO2AvoidedD, double *CO2AvoidedM, double *CO2AvoidedY, 
                      double *ExpensesSavedD, double *ExpensesSavedM, double *ExpensesSavedY) {
    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to create the file '%s'\n", filename);
        return;
    }

    // Write the header
    fprintf(file, "Composition,CO2 Avoided (kg/day),CO2 Avoided (kg/month),CO2 Avoided (kg/year),");
    fprintf(file, "Expenses Saved (CHF/day),Expenses Saved (CHF/month),Expenses Saved (CHF/year)\n");

    // Write each composition's data
    for (int i = 0; i < numCompositions; i++) {
        fprintf(file, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", 
                i + 1, CO2AvoidedD[i], CO2AvoidedM[i], CO2AvoidedY[i], 
                ExpensesSavedD[i], ExpensesSavedM[i], ExpensesSavedY[i]);
    }

    // Close the file
    fclose(file);
    printf("Results successfully saved to '%s'\n", filename);
}



int main() {
    double absorption[MAX_PLANTES]; // Array for absorptions
    char plants[MAX_PLANTES][NOM_MAX]; // Array of strings for plant names
    // Parameters for compositions
    double compositionSurfaces[MAX_COMPOSITIONS][MAX_PLANTES] = {0};
    int numCompositions = 0;
    // Parameters for thermal flux
    double uPlants[MAX_PLANTES];
    double dailySavings[MAX_COMPOSITIONS] = {0};
    double monthlySavings[MAX_COMPOSITIONS] = {0};
    double annualSavings[MAX_COMPOSITIONS] = {0};
    double oneDay = 24;
    double oneMonth = 732;
    double oneYear = 8766;
    double costPerKWh = 0.29; // 0.29 CHF/kWh
    double CO2AvoidedD[MAX_COMPOSITIONS] = {0};
    double CO2AvoidedM[MAX_COMPOSITIONS] = {0};
    double CO2AvoidedY[MAX_COMPOSITIONS] = {0};
    double energySavedD[MAX_COMPOSITIONS] = {0};
    double energySavedM[MAX_COMPOSITIONS] = {0};
    double energySavedY[MAX_COMPOSITIONS] = {0};

    // Read data from the file
    int numKnownPlants = readCharacteristics("plants_data.csv", absorption, plants, uPlants, NOM_MAX);
    if (numKnownPlants < 0) {
        return 1; // Error reading
    }
    
    // Display the plant names and their absorptions
    printf("Plant data and absorptions:\n");
    for (int i = 0; i < numKnownPlants; i++) {
        printf("Plant: %s, Absorption: %.2f\n", plants[i], absorption[i]);
    }

    // Array of file names to read
    const char *compositionFiles[] = {"matrice_1.csv", "matrice_2.csv", "matrice_3.csv", "matrice_4.csv", "matrice_5.csv", "matrice_6.csv"};
    const int numFiles = 6;

    // Read all compositions
    for (int fileIndex = 0; fileIndex < numFiles; fileIndex++) {
        int compositionsRead = 0; // Local variable for each file
        if (readCompositions(compositionFiles[fileIndex], plants, numKnownPlants, 
                             compositionSurfaces + numCompositions, &compositionsRead) != 0) {
            fprintf(stderr, "Error reading file: %s\n", compositionFiles[fileIndex]);
            return 1;
        }
        printf("File: %s, compositions read: %d\n", compositionFiles[fileIndex], compositionsRead);
        numCompositions += compositionsRead; // Update the total count
    }
    printf("Total number of compositions after reading: %d\n", numCompositions);

    // Calculate and display CO2 absorption for each composition
    printf("\nCO2 absorption for each composition:\n");
    for (int i = 0; i < numCompositions; i++) {
        double totalAbsorption = CO2Absorption(absorption, compositionSurfaces, i, numKnownPlants);
        printf("Composition %d: %.2f\n", i + 1, totalAbsorption);
    }

    // Thermal coefficients (in W/m²·K)
    double uConcrete = 2.0; // Thermal coefficient for concrete walls

    // Temperature difference between inside and outside (in °C)
    double deltaT = 10.0; // Example: 10 °C

    // Calculate and display thermal flux saved for each composition
    for (int i = 0; i < numCompositions; i++) {
        printf("\nComposition %d\n", i + 1);
        printf("\nThermal flux saved thanks to plants:\n");
        double totalFlux = 0.0;

        // Calculate the flux for each plant in this composition
        for (int j = 0; j < numKnownPlants; j++) {
            totalFlux += thermalFluxSaved(compositionSurfaces[i][j], deltaT, uConcrete, uPlants[j]);
        }
        printf("Thermal flux saved = %.2f W\n", totalFlux);

        // Calculate the savings
        printf("\nHeating and cooling savings for each composition:\n");
        dailySavings[i] = calculateSavings(totalFlux, oneDay, costPerKWh);
        printf("Savings in one day = %.2f CHF\n", dailySavings[i]);
        monthlySavings[i] = calculateSavings(totalFlux, oneMonth, costPerKWh);
        printf("Savings in one month = %.2f CHF\n", monthlySavings[i]);
        annualSavings[i] = calculateSavings(totalFlux, oneYear, costPerKWh);
        printf("Savings in one year = %.2f CHF\n", annualSavings[i]);

        // Energy saved in kWh 
        energySavedM[i] = totalFlux / 1000.0 * oneMonth; // Convert W to kW and multiply by hours
        energySavedD[i] = totalFlux / 1000.0 * oneDay; // Convert W to kW and multiply by hours
        energySavedY[i] = totalFlux / 1000.0 * oneYear; // Convert W to kW and multiply by hours
        // CO2 saved in kg
        CO2AvoidedD[i] = CO2Saved(energySavedD[i]);
        CO2AvoidedM[i] = CO2Saved(energySavedM[i]);
        CO2AvoidedY[i] = CO2Saved(energySavedY[i]);
        printf("CO2 avoided in a day = %.2f kg\n", CO2AvoidedD[i]);
        printf("CO2 avoided in a month= %.2f kg\n", CO2AvoidedM[i]);
        printf("CO2 avoided in a year= %.2f kg\n", CO2AvoidedY[i]);

        // Save results to a CSV file
        saveResultsToCSV("Savings.csv", numCompositions, CO2AvoidedD, CO2AvoidedM, CO2AvoidedY, dailySavings, monthlySavings, annualSavings);
    }

    return 0;
}
