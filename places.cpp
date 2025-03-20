#include "places.h"
#include <fstream>
#include <sstream>
#include <iostream>

Places::Places() {
    for(int i = 0; i < 8; i++) {
        places[i] = std::vector<std::string>();
    }
}

void Places::updatePlaces() {
    places[0] = inm;
    places[1] = inb;
    places[2] = aib;
    places[3] = lib;
    places[4] = adb;
    places[5] = reb;
    places[6] = rgf;
    places[7] = dam;
}

void Places::printPlaces() const {
    for(int i = 0; i < 8; i++) {
        if(i == 0) std::cout << "INM:";
        if(i == 1) std::cout << "INB:";
        if(i == 2) std::cout << "AIB:";
        if(i == 3) std::cout << "LIB:";
        if(i == 4) std::cout << "ADB:";
        if(i == 5) std::cout << "REB:";
        if(i == 6) std::cout << "RGF:";
        if(i == 7) std::cout << "DAM:";

        int count = 0;
        for(const auto& line : places[i]) {
            std::cout << line;
            if(count < places[i].size() - 1) std::cout << ',';
            count++;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Places::readInstruction(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while(std::getline(file, line)) {
        if(!line.empty()) {
            inm.push_back(line);
        }
    }
    file.close();
}

void Places::readRegisterFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    int reg = 0;

    while(std::getline(file, line)) {
        if(!line.empty()) {
            rgf.push_back(line);
            size_t comma_pos = line.find(',');
            std::string number = line.substr(comma_pos + 1);
            number = number.substr(0, number.length() - 1);
            writeRegister(reg, std::stoi(number));
            reg++;
        }
    }
    file.close();
}

void Places::readDataFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    int loc = 0;

    while(std::getline(file, line)) {
        if(!line.empty()) {
            dam.push_back(line);
            size_t comma_pos = line.find(',');
            std::string number = line.substr(comma_pos + 1);
            number = number.substr(0, number.length() - 1);
            writeData(loc, std::stoi(number));
            loc++;
        }
    }
    file.close();
}

uint32_t Places::readRegister(uint32_t reg) {
    return registers[reg];
}

void Places::writeRegister(uint32_t reg, uint32_t value) {
    registers[reg] = value;
}

uint32_t Places::readData(uint32_t loc) {
    return memory[loc];
}

void Places::writeData(uint32_t loc, uint32_t value) {
    memory[loc] = value;
}

bool Places::step() {
    if(inm.size() == 0 & 
       inb.size() == 0 & 
       aib.size() == 0 & 
       lib.size() == 0 & 
       adb.size() == 0 & 
       reb.size() == 0) return false;
    write();
    load();
    addr_calc();
    alu();
    issue();
    decode();
    return true;
}

bool Places::decode() {
    if(!inm.empty()) {
        std::string inst = inm.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while (std::getline(ss, item, ',')) {
            code.push_back(item);
        }

        std::string op1 = code[2];
        std::string op2 = code[3];

        uint32_t op1_loc = op1[1] - '0';
        uint32_t op2_loc = op2[1] - '0';

        code[2] = std::to_string(readRegister(op1_loc));
        code[3] = std::to_string(readRegister(op2_loc));

        std::string decoded;
        bool first = true;

        for(const auto& item : code) {
            if(!first) decoded += ",";
            decoded += item;
            first = false;
        }

        decoded += '>';
        inb.push_back(decoded);
        inm.erase(inm.begin());
    }
}

bool Places::issue() {
    if(!inb.empty()) {
        if(inb[0][1] == 'L') {
            lib.push_back(inb.front());
            inb.erase(inb.begin());
        }
        else {
            aib.push_back(inb.front());
            inb.erase(inb.begin());
        }
    }
}

bool Places::alu() {
    if(!aib.empty()) {
        std::string inst = aib.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while (std::getline(ss, item, ',')) {
            code.push_back(item);
        }

        std::string func = code[0].substr(1); 

        std::string op1 = code[2];
        std::string op2 = code[3];
        op2.pop_back();

        uint32_t op1_value = static_cast<uint32_t>(std::stoul(op1));
        uint32_t op2_value = static_cast<uint32_t>(std::stoul(op2));

        uint32_t result = 10;

        if(func == "ADD") {
            result = op1_value + op2_value;
        }

        else if(func == "SUB") {
            result = op1_value + op2_value;
        }

        else if(func == "AND") {
            result = op1_value & op2_value;
        }

        else {
            result = op1_value | op2_value;
        }

        std::string result_string = "<" + code[1] + "," + std::to_string(result) + ">";
        reb.push_back(result_string);
        aib.erase(aib.begin());
    }
}

bool Places::addr_calc() {
    if(!lib.empty()) {
        std::string inst = lib.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while (std::getline(ss, item, ',')) {
            code.push_back(item);
        }

        std::string op1 = code[2];
        std::string op2 = code[3];
        op2.pop_back();

        uint32_t op1_value = static_cast<uint32_t>(std::stoul(op1));
        uint32_t op2_value = static_cast<uint32_t>(std::stoul(op2));

        uint32_t mem_loc = op1_value + op2_value;

        std::string mem_string = "<" + code[1] + "," + std::to_string(mem_loc) + ">";

        adb.push_back(mem_string);
        lib.erase(lib.begin());
    }
}

bool Places::load() {
    if(!adb.empty()) {
        std::string inst = adb.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while (std::getline(ss, item, ',')) {
            code.push_back(item);
        }

        uint32_t mem_value = static_cast<uint32_t>(std::stoul(code[1]));

        std::string load_string = code[0] + "," + std::to_string(readData(mem_value)) + ">";

        reb.push_back(load_string);
        adb.erase(adb.begin());
    }
}


bool Places::write() {
    if(!reb.empty()) {
        std::string inst = reb.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while (std::getline(ss, item, ',')) {
            code.push_back(item);
        }

        uint32_t reg_loc = static_cast<uint32_t>(std::stoul(code[0].substr(2)));

        std::string write_string = inst;

        std::string  value_string = code[1];
        value_string.pop_back();
        uint32_t value = static_cast<uint32_t>(std::stoul(value_string));

        writeRegister(reg_loc, value);
        rgf[reg_loc] = write_string;
        reb.erase(reb.begin());
    }
}
