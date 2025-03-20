// On my honor, I have neither given nor received unauthorized aid on this assignment

#include <iostream>
#include <fstream>
#include <sstream>
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
    
        //Places();
        void updatePlaces();
        std::string printPlaces() const;
        void readInstruction(const std::string& filename);
        void readRegisterFile(const std::string& filename);
        void readDataFile(const std::string& filename);
        uint32_t readRegister(uint32_t reg);
        void writeRegister(uint32_t reg, uint32_t value);
        uint32_t readData(uint32_t loc);
        void writeData(uint32_t loc, uint32_t value);
    
        // transitions
        bool step();
        void decode();
        void issue();
        void alu();
        void addr_calc();
        void load();
        void write();
    
    };

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

std::string Places::printPlaces() const {
    std::string file_text;
    for(int i = 0; i < 8; i++) {
        if(i == 0)  {
            file_text += "INM:";
            //std::cout << "INM:";
        }
        if(i == 1) {
            file_text += "INB:";
            //std::cout << "INB:"; 
        }
        if(i == 2) {
            file_text += "AIB:";
            //std::cout << "AIB:";
        }
        if(i == 3) {
            file_text += "LIB:";
            //std::cout << "LIB:";
        }
        if(i == 4) {
            file_text += "ADB:";
            //std::cout << "ADB:";
        }
        if(i == 5) {
            file_text += "REB:";
            //std::cout << "REB:";
        }
        if(i == 6) {
            file_text += "RGF:";
            //std::cout << "RGF:";
        }
        if(i == 7) {
            file_text += "DAM:";
            //std::cout << "DAM:";
        }
        int count = 0;
        for(const auto& line : places[i]) {
            file_text += line;
            //std::cout << line;
            if(count < places[i].size() - 1) {
                file_text += ',';
                //std::cout << ',';
            }
            count++;
        }
        file_text += "\n";
        //std::cout << std::endl;
    }
    file_text += "\n";
    //std::cout << std::endl;

    return file_text;
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

void Places::decode() {
    if(!inm.empty()) {
        std::string inst = inm.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while(std::getline(ss, item, ',')) {
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

void Places::issue() {
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

void Places::alu() {
    if(!aib.empty()) {
        std::string inst = aib.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while(std::getline(ss, item, ',')) {
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
            result = op1_value - op2_value;
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

void Places::addr_calc() {
    if(!lib.empty()) {
        std::string inst = lib.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while(std::getline(ss, item, ',')) {
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

void Places::load() {
    if(!adb.empty()) {
        std::string inst = adb.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while(std::getline(ss, item, ',')) {
            code.push_back(item);
        }

        uint32_t mem_value = static_cast<uint32_t>(std::stoul(code[1]));

        std::string load_string = code[0] + "," + std::to_string(readData(mem_value)) + ">";

        reb.push_back(load_string);
        adb.erase(adb.begin());
    }
}


void Places::write() {
    if(!reb.empty()) {
        std::string inst = reb.front();
        std::vector<std::string> code;
        std::stringstream ss(inst);
        std::string item;

        while(std::getline(ss, item, ',')) {
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

int main() {
    Places places;

    places.readInstruction("instructions.txt");
    places.readRegisterFile("registers.txt");
    places.readDataFile("datamemory.txt");
    std::ofstream outFile("simulation.txt");

    std::string file_text;
    if(outFile.is_open()) {
        //std::cout << "check write" << std::endl;
        outFile << "STEP 0:" << std::endl; 
        //std::cout << "write complete" << std::endl;
    }

    //std::cout << "STEP 0:" << std::endl;
    places.updatePlaces();
    file_text = places.printPlaces();

    if(outFile.is_open()) {
        //std::cout << "check write" << std::endl;
        outFile << file_text; 
        //std::cout << "write complete" << std::endl;
    }
    
    int step = 0;
    while(true) {
        std::string file_text ="";
        bool step_done = places.step();
        if(!step_done) break;
        step++;
        outFile << "STEP " << step << ":" << std::endl; 
        //std::cout << "STEP " << step << ":" << std::endl;
        places.updatePlaces();
        file_text = places.printPlaces();
        outFile << file_text;
    } 

    outFile.close();
    //std::cout << "DONE!!";

    return 0;
}
