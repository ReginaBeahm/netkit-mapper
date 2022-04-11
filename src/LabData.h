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

    // Returns labDir
    const std::string getLabDir();

    std::vector<std::string> cds;   // List of unique collision domains 
    std::vector<struct LabMachine*> machines;  // Vector of machines

private:
    const std::string labDir;

    // Reads lab.conf to initialise machineConns structure
    void ReadConf(std::string labConf);
};

#endif