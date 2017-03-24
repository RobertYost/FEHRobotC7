#ifndef DEBUGGING_H
#define DEBUGGING_H


class Debugging
{
public:
    Debugging();
    void PrintTelemetry(const char* titles[], bool values[]);
    void PrintTelemetry(const char* titles[], int values[]);
    void PrintTelemetry(const char* titles[], float values[]);
    void PrintTelemetry(const char* titles[], double values[]);
};

extern Debugging DEBUGGER;

#endif // DEBUGGING_H
