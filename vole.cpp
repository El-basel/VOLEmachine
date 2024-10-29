#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include "vole.h"

bool inputStreamFailing()
{
    if(std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        return true;
    }
    return false;
}

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
    memory[index] = value;
}
void Memory::reset() {
    std::fill(std::begin(memory), std::end(memory), "00");
    instructionMemory.clear();
}
// End of Memory class

// ALU class
int ALU::hexToDec(const string& hexString) {
    int dec;
    stringstream ss;
    ss << hex << hexString;
    ss >> dec;
    return dec;
}

string ALU::decToHex(const int& decNumber) {
    stringstream ss;
    ss << hex << uppercase << decNumber;
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


void ALU::add(int x1, int x2, int resultx, Register& reg) {
    double val1 = reg.getCell(x1);
    double val2 = reg.getCell(x2);
    double sumx = val1 + val2;
    reg.setCell(resultx, sumx);
}
// End of ALU class
// CU class
void CU::load(int reg1, int cell, Register& reg, Memory& mem) {
    int num = alu.hexToDec(mem.getCell(cell));
    reg.setCell(reg1, num);
}
void CU::load(int reg1, int num, Register& reg) {
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

int Machine::decode(std::string instruction) {
    return alu.hexToDec(instruction);
}

void Machine::execute() {
    fetch();
    int operation   = decode(string{instructionRegister[0]});
    int register1   = decode(string{instructionRegister[1]});
    int register2   = decode(string{instructionRegister[2]});
    int register3   = decode(string{instructionRegister[3]});
    int memoryCell  = decode(instructionRegister.substr(2));
    int pattern     = decode(instructionRegister.substr(2));
    switch (operation) {
        case 1:
            cu.load(register1, memoryCell, registers, memory);
            break;
        case 2:
            cu.load(register1, pattern, registers);
            break;
        case 3:
            cu.store(register1, pattern, registers, memory);
            break;
        case 4:
            cu.move(register2, register3, registers);
            break;
        case 5:
            alu.add(register2, register3, register1, registers);
            break;
        case 6:
             alu.add(register2, register3, register1, registers);
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
//        std::cout << cell ;
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
            std::cout << "VOLE shutting down\n";
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
    char choice;
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    while (inputStreamFailing() or choice < 'a' or choice > 'd')
    {
        std::cout << "Please choose an option for the above only\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
    }
    return choice;
}