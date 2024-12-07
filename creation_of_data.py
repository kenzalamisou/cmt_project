# Creation of the data class wich will stock the informations about the differente type of plants
#Clarification for the units: absorption rate is in kg of CO2/m2/day, growth rate is in m/day, isolation rate is in m2K/W, thermal coef is in W/m2K
from dataclasses import dataclass
@dataclass
class Plant:
    name: str
    absorption_rate: float
    growth_rate: float
    isolation_rate: float
    thermal_coef: float

# Filling the data class for each type of plant 
plant1 = Plant(name="Lierre", absorption_rate=0.034, growth_rate=0.0027, isolation_rate=0.85, thermal_coef=0.85)
plant2 = Plant(name="Clematite", absorption_rate=0.018, growth_rate=0.0021, isolation_rate=0.45, thermal_coef=0.45)
plant3 = Plant(name="Passiflore", absorption_rate=0.027, growth_rate=0.0041, isolation_rate=0.5, thermal_coef=0.50)
plant4 = Plant(name="Jasmin", absorption_rate=0.022, growth_rate=0.0021, isolation_rate=0.65, thermal_coef=0.65)

# Creation of the CSV file with the plants data: each plant will have a line with it's specific information ( separated by a coma)
import csv
# List of the plants
plants = [plant1, plant2, plant3, plant4]
# Path of the CSV file( ask the assitant how can i store it if i want to use it in c while running the code once( do we use relative paths as chat suggested  "./data/plants_data.csv" attention we will have to add folder in the repository))
# For now I want to check if the CSV file is correct so i will put the path of my own device 
csv_file_path = "C:/Users/kalam/Desktop/INFO/python/fnct/plants_data.csv"
# Adding data to the file
with open(csv_file_path, mode='w', newline='') as file:
    writer = csv.writer(file)
    # Adding the name of the variable 
    writer.writerow(["Name", "Absorption Rate (kg CO2/m2/day)", "Growth Rate (m/day)", "Isolation Rate (m2K/W)", "Thermal Coefficient (W/m2K)"])
    # Adding the data of the plants
    for plant in plants:
        writer.writerow([plant.name, plant.absorption_rate, plant.growth_rate, plant.isolation_rate, plant.thermal_coef])

# Creation of the data for the tempetature
# Different variable are taken into account to generate this data: the season and the climate
# Function to generate random values for four seasons, we will generate many times the same value to have an average value of the randum temperature generated in the range 
import random
def generate_values(spring, summer, autumn, winter, days_per_season):
    spring_values = [random.uniform(*spring) for _ in range(days_per_season)]
    summer_values = [random.uniform(*summer) for _ in range(days_per_season)]
    autumn_values = [random.uniform(*autumn) for _ in range(days_per_season)]
    winter_values = [random.uniform(*winter) for _ in range(days_per_season)]
    return spring_values + summer_values + autumn_values + winter_values

# Number of days per season
days_per_season = 91  # Approx. 1/4 of a year

# Temperature ranges for different climates
temperature_ranges = {
    "Tropical": {"spring": (25, 30), "summer": (30, 35), "autumn": (25, 20), "winter": (20, 16)},
    "Mediterranean": {"spring": (20, 25), "summer": (25, 30), "autumn": (20, 15), "winter": (10, 15)},
    "Continental": {"spring": (10, 20), "summer": (20, 25), "autumn": (10, -10), "winter": (-10, -5)},
}

# Rainfall ranges (mm/day)
rainfall_ranges = {
    "Tropical": {"spring": (8, 12), "summer": (10, 15), "autumn": (8, 12), "winter": (5, 8)},
    "Mediterranean": {"spring": (1.5, 2.5), "summer": (0.5, 1), "autumn": (2.5, 3.5), "winter": (1.5, 2.5)},
    "Continental": {"spring": (1, 2), "summer": (2, 3), "autumn": (1.5, 2.5), "winter": (0.5, 1.5)},
}

# Sunlight ranges (hours/day)
sunlight_ranges = {
    "Tropical": {"spring": (6, 7), "summer": (5, 6), "autumn": (6, 7), "winter": (4, 5)},
    "Mediterranean": {"spring": (6, 8), "summer": (9, 10), "autumn": (5, 7), "winter": (4, 6)},
    "Continental": {"spring": (5, 7), "summer": (7, 9), "autumn": (3, 5), "winter": (1, 3)},
}

# Generate data for each climate
data = {}
for climate in ["Tropical", "Mediterranean", "Continental"]:
    temps = generate_values(
        temperature_ranges[climate]["spring"],
        temperature_ranges[climate]["summer"],
        temperature_ranges[climate]["autumn"],
        temperature_ranges[climate]["winter"],
        days_per_season
    )
    rain = generate_values(
        rainfall_ranges[climate]["spring"],
        rainfall_ranges[climate]["summer"],
        rainfall_ranges[climate]["autumn"],
        rainfall_ranges[climate]["winter"],
        days_per_season
    )
    sun = generate_values(
        sunlight_ranges[climate]["spring"],
        sunlight_ranges[climate]["summer"],
        sunlight_ranges[climate]["autumn"],
        sunlight_ranges[climate]["winter"],
        days_per_season
    )
    data[climate] = {"temperatures": temps, "rainfall": rain, "sunlight": sun}

# Creation of the csv file 
csv_file_path = "C:/Users/kalam/Desktop/INFO/python/fnct/climate_data.csv"

# Extract data for the CSV file structure
rows = zip(
    data["Tropical"]["temperatures"], data["Tropical"]["sunlight"], data["Tropical"]["rainfall"],
    data["Mediterranean"]["temperatures"], data["Mediterranean"]["sunlight"], data["Mediterranean"]["rainfall"],
    data["Continental"]["temperatures"], data["Continental"]["sunlight"], data["Continental"]["rainfall"]
)

# Write the data into the CSV file
with open(csv_file_path, mode='w', newline='') as file:
    writer = csv.writer(file)
    # Write the header row
    writer.writerow([
        "Tropical Temperature", "Tropical Sun Rate", "Tropical Rain Rate",
        "Mediterranean Temperature", "Mediterranean Sun Rate", "Mediterranean Rain Rate",
        "Continental Temperature", "Continental Sun Rate", "Continental Rain Rate"
    ])
    # Write the data rows
    writer.writerows(rows)

# Plot to visualize the data in the two csv files 
import matplotlib.pyplot as plt
# Extract plant names and rates
plant_names = [plant.name for plant in plants]
absorption_rates = [plant.absorption_rate for plant in plants]
growth_rates = [plant.growth_rate for plant in plants]
isolation_rates = [plant.isolation_rate for plant in plants]
thermal_coefs = [plant.thermal_coef for plant in plants]
# Climate data (already generated)
climates = ["Tropical", "Mediterranean", "Continental"]
temperature_data = [data[climate]["temperatures"] for climate in climates]
sunlight_data = [data[climate]["sunlight"] for climate in climates]
rainfall_data = [data[climate]["rainfall"] for climate in climates]

# Plot 1: Plant data (adjust scales)
plt.figure(figsize=(10, 6))
x = range(len(plant_names))
plt.bar(x, [rate * 100 for rate in absorption_rates], width=0.2, label="Absorption Rate (x100)", align='center')
plt.bar([i + 0.2 for i in x], [rate * 1000 for rate in growth_rates], width=0.2, label="Growth Rate (x1000)", align='center')
plt.bar([i + 0.4 for i in x], isolation_rates, width=0.2, label="Isolation Rate", align='center')
plt.bar([i + 0.6 for i in x], thermal_coefs, width=0.2, label="Thermal Coefficient", align='center')
plt.xticks([i + 0.3 for i in x], plant_names)
plt.title("Plant Data: Rates and Coefficients (Adjusted Scales)")
plt.xlabel("Plant Names")
plt.ylabel("Values (Scaled)")
plt.legend()
plt.tight_layout()
plt.show()

# Plot 2: Comparison of temperatures across climates
plt.figure(figsize=(12, 6))
for i, climate in enumerate(climates):
    days = range(1, len(temperature_data[i]) + 1)
    plt.plot(days, temperature_data[i], label=f"{climate} Temperature")
plt.title("Temperature Comparison Across Climates")
plt.xlabel("Days")
plt.ylabel("Temperature (°C)")
plt.legend()
plt.tight_layout()
plt.show()

# Plot 3: Comparison of sunlight and rainfall rates across climates
# Combined plot for sunlight and rainfall comparison across all climates
plt.figure(figsize=(12, 6))
days = range(1, len(sunlight_data[0]) + 1)

# Plot sunlight and rainfall for each climate on the same plot
for i, climate in enumerate(climates):
    plt.plot(days, sunlight_data[i], label=f"{climate} Sunlight (hours)", linestyle="-")
    plt.plot(days, rainfall_data[i], label=f"{climate} Rainfall (mm)", linestyle="--")

# Add title, labels, and legend
plt.title("Sunlight and Rainfall Comparison Across Climates")
plt.xlabel("Days")
plt.ylabel("Rates (Sunlight in hours / Rainfall in mm)")
plt.legend()
plt.tight_layout()
plt.show()