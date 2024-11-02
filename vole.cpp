#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <cstdint>
#include "vole.h"

// Register class
Register::Register() {
    std::fill(std::begin(memory), std::end(memory), 0.0);
}
double Register::getCell(int index) {
    if (index < 0 || index >= size) {
        throw out_of_range("Index out of bounds");
    }
    return memory[index];
}

void Register::setCell(int index, int value) {
    if (index < 0 || index >= size) {
        throw out_of_range("Index out of bounds");
    }
    memory[index] = static_cast<double>(value);
}

void Register::setCell(int index, double value) {
    if (index < 0 || index >= size) {
        throw out_of_range("Index out of bounds");
    }
    memory[index] = value;
}
void Register::reset() {
    std::fill(std::begin(memory), std::end(memory), 0.0);
}
// End of Register class

// Memory class
Memory::Memory() {
    for (int i = 0; i < size; ++i) {
        memory[i] = "00";
    }
}

std::string Memory::getCell(int index) {
    if(index < 0 || index >= size)
    {
        throw out_of_range("Index out of bounds");
    }
    return memory[index];
}

void Memory::setCell(int index, std::string value) {
    for (int i = 0; i < value.length(); i += 2) {
        memory[index] = value.substr(i,2);
        ++index;
    }
//    memory[index] = value;
}
void Memory::reset() {
    std::fill(std::begin(memory), std::end(memory), "00");
    instructionMemory.clear();
}
// End of Memory class

// ALU class
int ALU::hexToDec(const string& hexString, bool positiveOnly = false) {
    int dec;
    stringstream ss;
    ss << hex << hexString;
    ss >> dec;
    if(hexString[0] > '7' and !positiveOnly)
    {
     dec -= 256;
    }
    return dec;
}

double ALU::hexToFloat(const std::string& hexString) {
    long long signBit, exponent;
    int decimal;
    double mantissa = 0;
    decimal = hexToDec(hexString, true);
    std::bitset<8> bit{static_cast<unsigned long long>(decimal)};
    signBit = bit[7];
    std::bitset<3> extractedExponent;
    for (int i = 4; i < 7; ++i) {
        extractedExponent[i - 4] = bit[i];
    }
    exponent = static_cast<unsigned int>(extractedExponent.to_ulong());
    exponent -= 3;
    std::bitset<4> extractedMantissa;
    for (int i = 0; i < 4; ++i) {
        extractedMantissa[i] = bit[i];
    }
    double num = 0.5;
    double divisor = 2;
    for (int i = 3; i >= 0; --i) {
        mantissa += extractedMantissa[i] * num;
        num /= divisor;
    }
    mantissa += 1;
    double value = std::pow(-1, signBit) * std::pow(2, exponent) * mantissa;
    return value;
}

string ALU::decToHex(const double& decNumber) {
    float number = decNumber;
    if(decNumber < 0)
    {
        number += 256;
    }
    stringstream ss;
    ss << hex << uppercase << static_cast<int>(number);
    return ss.str();
}

bool ALU::isValid(const string& hexString) {
    for (char c : hexString) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}


void ALU::addDecimal(int x1, int x2, int resultx, Register& reg) {
    double val1 = reg.getCell(x1);
    double val2 = reg.getCell(x2);
    double sumx = val1 + val2;
    reg.setCell(resultx, sumx);
}

void ALU::addFloat(int x1, int x2, int resultx, Register& reg) {
    std::string num = decToHex(reg.getCell(x1));
    double val1 = hexToFloat(num);
    num = decToHex(reg.getCell(x2));
    double val2 = hexToFloat(num);
    double sumx = val1 + val2;
    reg.setCell(resultx, sumx);
}
// End of ALU class
// CU class
void CU::load(int reg1, int cell, Register& reg, Memory& mem) {
    int num = alu.hexToDec(mem.getCell(cell));
    reg.setCell(reg1, num);
}
void CU::load(int reg1, double num, Register& reg) {
    reg.setCell(reg1, num);
}
void CU::store(int reg1, int loc, Register& reg, Memory& mem) {
    if (loc == 0) {
        std::cout << alu.decToHex(reg.getCell(reg1)) << '\n';
    }
    int num = reg.getCell(reg1);
    mem.setCell(loc,alu.decToHex(num));
}
void CU::move(int reg1, int reg2, Register& reg) {
    reg.setCell(reg2, reg.getCell(reg1));
}
void CU::jump(int reg1, int cell, Register& reg, int& counter) {
    if (reg.getCell(reg1) == reg.getCell(0)) {
        counter = cell;
    }
}
void CU::halt(Register& reg,Memory& mem) {
    reg.reset();
    mem.reset();
    std::cout << "--------------------\n";
    std::cout << "| Program Finished |\n";
    std::cout << "--------------------\n";
}
// Machine class

bool Machine::loadProgramFile(std::string& file) {
    programFile.open(file);
    std::string instruction;
    if(!programFile)
    {
        return false;
    }
    while(!programFile.eof())
    {
        programFile >> instruction;
        memory.instructionMemory.push_back(instruction);
    }
    return true;
}

void Machine::fetch() {
    instructionRegister = memory.instructionMemory[programCounter];
    ++programCounter;
}

int Machine::decode(std::string instruction, bool positiveOnly = false) {
    return alu.hexToDec(instruction,positiveOnly);
}

void Machine::execute() {
    if(memory.instructionMemory.empty())
    {
        std::cerr << "Please load a program first\n";
        return;
    }
    fetch();
    int operation   = decode(string{instructionRegister[0]},true);
    int register1   = decode(string{instructionRegister[1]},true);
    int register2   = decode(string{instructionRegister[2]},true);
    int register3   = decode(string{instructionRegister[3]},true);
    int memoryCell  = decode(instructionRegister.substr(2,2),true);
    int pattern     = decode(instructionRegister.substr(2,2));
    switch (operation) {
        case 1:
            cu.load(register1, memoryCell, registers, memory);
            break;
        case 2:
            cu.load(register1, pattern, registers);
            break;
        case 3:
            cu.store(register1, memoryCell, registers, memory);
            break;
        case 4:
            cu.move(register2, register3, registers);
            break;
        case 5:
            alu.addDecimal(register2, register3, register1, registers);
            break;
        case 6:
             alu.addFloat(register2, register3, register1, registers);
        case 11:
            cu.jump(register1, memoryCell, registers, programCounter);
            break;
        case 12:
            outputState();
            cu.halt(registers,memory);
            return;
    }
}

void Machine::outputState() {
    std::cout << "Program Counter: " << programCounter << '\n';
    std::cout << "Instruction Register: " << instructionRegister << '\n';
    std::cout << "Registers: ";
    std::string cell{};
    for (int i = 0; i < 16; ++i) {
        cell = alu.decToHex(registers.getCell(i));
        std::cout << "R" << i << " = " << (cell.length() == 1? "0" : "") << cell << ' ';
    }
    std::cout << '\n';

    std::cout << "Memory: ";
    for (int i = 0; i < 256; ++i) {
        cell = memory.getCell(i);
        if(i % 8 == 0)
        {
            std::cout << '\n';
        }
        std::cout << std::setw(4) << std::setfill(' ');
        std::cout << (cell.length() == 1? "0" + cell : "" + cell) << ' ';
    }
    std::cout << '\n';
}
// End of Machine class

// MainUI class

int MainUI::displayMenu() {
    std::cout << "a. Enter program file\n";
    std::cout << "b. Display machine state\n";
    if(!fileName.empty())
    {
        std::cout << "c. Execute an instruction\n";
    }
    std::cout << "d. Exit machine\n";
    char choice = inputChoice();
    switch (choice) {
        case 'a':
            inputFileName();
            break;
        case 'b':
            machine.outputState();
            break;
        case 'c':
            machine.execute();
            break;
        case 'd':
            std::cout << "----------------------\n";
            std::cout << "| VOLE shutting down |\n";
            std::cout << "----------------------\n";
            return 0;
    }
    return 1;
}

void MainUI::inputFileName() {
    std::cout << "Enter program file name: ";
    std::cin >> fileName;
    if(!machine.loadProgramFile(fileName))
    {
        std::cout << "File doesn't exist\n";
        return;
    }
    std::cout << "File loaded successfully\n";
}

char MainUI::inputChoice() {
    string choice1;
    std::cout << "Enter your choice: ";
    getline(std::cin, choice1);
    while (choice1.size() != 1 or choice1[0] < 'a' or choice1[0] > 'd')
    {
        std::cout << "Please choose an option from the above only\n";
        std::cout << "Enter your choice: ";
        getline(std::cin, choice1);
    }
    return choice1[0];
}