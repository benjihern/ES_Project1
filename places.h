#ifndef PLACES_H
#define PLACES_H

#include <string>
#include <vector>

class Places {
private: 
    uint32_t registers[8];
    uint32_t memory[8];

    std::vector<std::string> places[8];
    std::vector<std::string> inm;
    std::vector<std::string> inb;
    std::vector<std::string> aib;
    std::vector<std::string> lib;
    std::vector<std::string> adb;
    std::vector<std::string> reb;
    std::vector<std::string> rgf;
    std::vector<std::string> dam;

public:

    Places();
    void updatePlaces();
    void printPlaces() const;
    void readInstruction(const std::string& filename);
    void readRegisterFile(const std::string& filename);
    void readDataFile(const std::string& filename);
    uint32_t readRegister(uint32_t reg);
    void writeRegister(uint32_t reg, uint32_t value);
    uint32_t readData(uint32_t loc);
    void writeData(uint32_t loc, uint32_t value);

    // transitions
    bool step();
    bool decode();
    bool issue();
    bool alu();
    bool addr_calc();
    bool load();
    bool write();

};

#endif
