#ifndef NK_DATA_STRUCTURES_H
#define NK_DATA_STRUCTURES_H

#include <vector>
#include <string>

// A specific interface on a machine (e.g. eth0)
struct MachineInterface 
{
    char id;  // Interface number (e.g. 0 for eth0)

    std::string segment;  // The network segment it is attached to
};

// Data about a lab machine
struct LabMachine 
{
    bool newMachine = false;    // True if machine is new

    std::string machineName;

    std::vector<struct MachineInterface*> interfaces;

    float pos[2];
};

#endif