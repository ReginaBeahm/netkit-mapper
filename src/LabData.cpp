#include "LabData.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <beahm/isinteger.h>

namespace fs = std::filesystem;   // Rename to fs for code brevity

LabData::LabData(std::string labDir) : labDir(labDir)
{

}

LabData::~LabData() 
{
    for (struct LabMachine* lm : machines)  // Free dynamic allocations
    {
        for (struct MachineInterface* mi : lm->interfaces) 
        {
            delete mi;
        }

        delete lm;
    }
}

void LabData::EnumerateLabDir() 
{
    machines.reserve(20);    // Reserve space for 20 machines

    struct LabMachine* machine;

    for (const fs::directory_entry& dirEnt : fs::directory_iterator(labDir))  // Iterate through files in labDir
    {
        if (dirEnt.is_directory())    // If entry is a directory then it represents a lab machine
        {
            std::string path = dirEnt.path();
            std::string name;

            int i;
            for (i = path.length() - 1; i >= 0; i--) 
            {
                if (path[i] == '/') 
                {
                    name = path.substr(i + 1);  // Extract machine name from path
                    break;
                }
            }

            machine = new LabMachine();
            machine->machineName = name;

            machines.push_back(machine);
        }
    }

    ReadConf(labDir + "/lab.conf");  // Parse lab.conf
}

void LabData::ReadConf(std::string labConf) 
{
    std::cout << "Parsing lab.conf ..." << std::endl;

    std::fstream conf;
    conf.open(labConf, std::ios::in);  // Open lab.conf file for reading

    if (conf.is_open())
    {
        std::string line;
        struct MachineInterface* mi;
        while (getline(conf, line))  // Read config line by line
        {
            int fieldNum = 0;
            bool leave = false;   // Set if machine argument is irrelevant 
            std::string fields[3];
            for (std::string s : fields) 
            {
                s.reserve(50);   // Reserve space for fields
            }
            
            for (const char c : line) 
            {
                if (leave) 
                {
                    break;
                }

                switch (c) 
                {
                    case '#':
                        leave = true;    // Comment. Don't read remainder of line
                        break;
                    case '=':
                        fieldNum++;
                        break;
                    case '[':
                        fieldNum++;
                        break;
                    case ']':
                        if (!beahm::isInteger(fields[fieldNum])) 
                        {
                            leave = true;   // Argument is not an interface
                        }
                        break;
                    default:
                        fields[fieldNum] += c; // Append char to correct string
                        break;
                }
            }

            if (fieldNum == 2)  // Detect collision domain specification
            {
                mi = new MachineInterface();
                mi->id = std::stoi(fields[1]);
                mi->segment = fields[2];

                int i;
                int len = machines.size();
                for (i = 0; i < len; i++)    // Search for relevant LabMachine struct
                {
                    if (!machines[i]->machineName.compare(fields[0]))  // If names match ...
                    {
                        machines[i]->interfaces.push_back(mi);   // Add interface
                        break;
                    }
                }

                if (i == len)   // Machine name is not found
                {
                    std::cout << "Invalid machine name: " << fields[0] << std::endl;
                }
            }
        }
        conf.close();
    }

    std::cout << "Finished parsing lab.conf" << std::endl;
}