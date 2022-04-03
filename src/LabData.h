#ifndef LAB_DATA_H
#define LAB_DATA_H

#include "NkDataStructures.h"
#include <string>

/* A LabData object contains object and position data used to draw
a specific lab */
class LabData 
{
public:
    LabData(std::string labDir);

private:
    std::vector<std::string> machines;  // Vector of machine names
    Matrix2D<int> machineConns;   // Each row is a single connection between machines, identified using their IDs
    Matrix2D<float> machinePos;   // Each row represents a coordinate where a machine is placed in the network map

    // Initialises machineIDs and machinePos. Calls ReadConf()
    void EnumerateLabDir(std::string labDir);

    // Reads lab.conf to initialise machineConns structure
    void ReadConf(std::string labConf);
};

#endif