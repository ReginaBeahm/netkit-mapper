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
    ~LabData();

    // Initialises machineIDs and machinePos. Calls ReadConf()
    void EnumerateLabDir();

private:
    std::string labDir;
    std::vector<struct LabMachine*> machines;  // Vector of machines

    // Reads lab.conf to initialise machineConns structure
    void ReadConf(std::string labConf);
};

#endif