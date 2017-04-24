#include "debugging.h"
#include <FEHLCD.h>
#include <FEHBattery.h>
#include <cstring>

Debugging DEBUGGER;

Debugging::Debugging()
{

}

/**
 * @brief Debugging::PrintTelemetry     Prints boolean-typed telemetry values to the screen
 * @param titles                        Names of components for which the telemetry data is being gathered from
 * @param values                        Values which will be printed to the screen
 */
void Debugging::PrintTelemetry(const char* titles[], bool values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], 4 + i, 3);
        LCD.WriteRC(values[i], 4 + i, 3 + std::strlen(titles[i]));
    }
}

/**
 * @brief Debugging::PrintTelemetry     Prints integer-typed telemetry values to the screen
 * @param titles                        Names of components for which the telemetry data is being gathered from
 * @param values                        Values which will be printed to the screen
 */
void Debugging::PrintTelemetry(const char* titles[], int values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], i + 4, 3);
        LCD.WriteRC(values[i], i + 4, 3 + std::strlen(titles[i]));
    }
}

/**
 * @brief Debugging::PrintTelemetry     Prints floating point-typed telemetry values to the screen
 * @param titles                        Names of components for which the telemetry data is being gathered from
 * @param values                        Values which will be printed to the screen
 */
void Debugging::PrintTelemetry(const char* titles[], float values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(unsigned int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], 4 + i, 3);
        LCD.WriteRC(values[i], 4 + i, 3 + std::strlen(titles[i]));
    }
}

/**
 * @brief Debugging::PrintTelemetry     Prints double precision floating point-typed telemetry values to the screen
 * @param titles                        Names of components for which the telemetry data is being gathered from
 * @param values                        Values which will be printed to the screen
 */
void Debugging::PrintTelemetry(const char* titles[], double values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(unsigned int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], 4 + i, 3);
        LCD.WriteRC(values[i], 4 + i, 3 + std::strlen(titles[i]));
    }
}
