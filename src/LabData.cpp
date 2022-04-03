#include "LabData.h"
#include <filesystem>

namespace fs = std::filesystem;   // Rename to fs for code brevity

LabData::LabData(std::string labDir)
{
    EnumerateLabDir(labDir);
}

void LabData::EnumerateLabDir(std::string labDir) 
{
    machines.reserve(20);    // Reserve memory for 20 machines to prevent several allocations

    for (const fs::directory_entry& dirEnt : fs::directory_iterator(labDir))  // Iterate through files in labDir
    {
        if (dirEnt.is_directory())    // If entry is a directory then it represents a lab machine
        {
            machines.push_back(dirEnt.path());
        }
    }

    machines.shrink_to_fit();  // Shrink memory allocation to occupied memory
}

void LabData::ReadConf(std::string labConf) 
{

}