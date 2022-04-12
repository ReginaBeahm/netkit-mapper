#include "LabData.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <beahm/isinteger.h>
#include <beahm/dirutils.h>

namespace fs = std::filesystem;   // Rename to fs for code brevity

LabData::LabData(std::string labDir) : labDir(labDir)
{

}

LabData::~LabData() 
{
    std::fstream conf;
    conf.open(labDir + "/lab.conf", std::ios::out);  // Open lab.conf file for writing

    for (struct LabMachine* lm : machines)  // Free dynamic allocations
    {
        conf << "### " << lm->machineName << " Collision Domains ###" << std::endl;
        for (struct MachineInterface* mi : lm->interfaces) 
        {
            if (mi->segment.compare("None"))    // Check that interface is connected
            {
                conf << lm->machineName << "[" << std::to_string((int)mi->id) << "]" << "=" << mi->segment << std::endl;
            }
            delete mi;
        }
        conf << std::endl;   // Make space between machines

        delete lm;
    }

    conf.close();
}

void LabData::EnumerateLabDir() 
{
    machines.reserve(20);    // Reserve space for 20 machines

    struct LabMachine* machine;

    for (const fs::directory_entry& dirEnt : fs::directory_iterator(labDir))  // Iterate through files in labDir
    {
        if (dirEnt.is_directory())    // If entry is a directory then it represents a lab machine
        {
            machine = new LabMachine();
            machine->machineName = beahm::getFileName(dirEnt.path());

            machines.push_back(machine);
        }
    }

    ReadConf();  // Parse lab.conf
}

void LabData::ReadConf() 
{
    std::cout << "Parsing lab.conf ..." << std::endl;

    std::fstream conf;
    conf.open(labDir + "/lab.conf", std::ios::in);  // Open lab.conf file for reading

    std::fstream confBackup;    // Creats lab.backup file with original lab.conf
    confBackup.open(labDir + "/lab.backup", std::ios::out);

    if (conf.is_open())
    {
        std::string line;
        struct MachineInterface* mi;
        while (getline(conf, line))  // Read config line by line
        {
            confBackup << line << std::endl;  // Write to backup

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
                else    // Add collision domain to cds if it is new
                {
                    bool found = false;
                    for (const std::string s : cds) 
                    {
                        if (!mi->segment.compare(s))   // If collision domain is already in list ...
                        {
                            found = true;
                        }
                    }  
                    if (!found) 
                    {
                        cds.push_back(mi->segment);
                    }
                }
            }
        }
        confBackup.close();
        conf.close();
        
        std::cout << "Finished parsing lab.conf" << std::endl;
    }
    else   // Couldn't open lab.conf
    {
        std::cout << "Failed to open lab.conf" << std::endl;
    }
}

const std::string LabData::getLabDir() 
{
    return labDir;
}