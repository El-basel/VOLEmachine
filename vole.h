#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
using namespace std;

#ifndef VOLE_H
#define VOLE_H

class Register
{
private:
    int memory[16]{};
    int size = 16;
public:
    int getCell(int index);
    void setCell(int index, int value);

};

class Memory
{
private:
    std::string memory[256]{};
    int size = 256;
public:
    int getCell(int);
    void setCell(int, int);
};

class CU
{
public:
    void load(int, int, Register&, Memory&);
    void load(int, int, Register&);
    void store(int, int, Register&, Memory&);
    void move(int, int, Register&);
    void jump(int, int, Register&, int&);
    void halt();

};

class ALU
{
public:
    std::string hexToDec(const string& hexString);
    std::string decToHex(const int& decNumber);
    bool isValid(const string& hexString);
    void add(int x1, int x2, int resultx, Register& reg);
};

class CPU
{
private:
    int programCounter{};
    std::string instructionRegister{};
    Register registers;
    ALU alu;
    CU cu;
public:
    void fetch(Memory&);
    void decode();
    void execute(Register&, Memory&);
};

class Machine
{
private:
    CPU processor;
    Memory memory;
public:
    void loadProgramFile();
    void outputState();
};

class MainUI
{
private:
    Machine machine;
    std::string fileName;
    bool enterFileOrInstruction;
public:
    bool getFileOrInstruction();
    void displayMenu();
    std::string inputFileName();
    char inputChoice();
};

#endif
