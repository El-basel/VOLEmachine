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
int Register::getCell(int index) {
    if (index < 0 || index >= size) {
        throw out_of_range("Index out of bounds");
    }
    return memory[index];
}

void Register::setCell(int index, int value) {
    if (index < 0 || index >= size) {
        throw out_of_range("Index out of bounds");
    }
    memory[index] = value;
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
    int val1 = reg.getCell(x1);
    int val2 = reg.getCell(x2);
    int sumx = val1 + val2;
    reg.setCell(resultx, sumx);
}
// End of ALU class

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

int Machine::decode(const char instruction) {
    std::string tmp {instruction};
    return alu.hexToDec(tmp);
}

void Machine::execute() {
    fetch();
    int operation   = decode(instructionRegister[0]);
    int register1   = decode(instructionRegister[1]);
    int register2   = decode(instructionRegister[2]);
    int register3   = decode(instructionRegister[3]);
    int memoryCell  = decode(instructionRegister[2] + instructionRegister[3]);
    int pattern     = decode(instructionRegister[2] + instructionRegister[3]);
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
            cu.halt();
    }
}

void Machine::outputState() {
    std::cout << "Program Counter: " << programCounter << '\n';
    std::cout << "Instruction Register: " << instructionRegister << '\n';
    std::cout << "Registers: ";
    for (int i = 0; i < 16; ++i) {
        std::cout << "R" << i << " = " << registers.getCell(i) << ' ';
    }
    std::cout << '\n';

    std::cout << "Memory: ";
    for (int i = 0; i < 256; ++i) {
        if(i % 8 == 0)
        {
            std::cout << '\n';
        }
        std::cout << std::setw(3) << std::setfill(' ');
        std::cout << memory.getCell(i) << ' ';
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