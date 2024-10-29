#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <fstream>
#include <vector>
using namespace std;

#ifndef VOLE_H
#define VOLE_H

class Register
{
private:
    double memory[16]{};
    int size = 16;
public:
    double getCell(int index);
    void setCell(int index, int value);
    void setCell(int index, double value);
};

class Memory
{
private:
    std::string memory[256]{};
    int size = 256;
public:
    Memory();
    std::string getCell(int);
    void setCell(int, std::string);
    std::vector<std::string> instructionMemory;
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
    int hexToDec(const string& hexString);
    string decToHex(const int& decNumber);
    bool isValid(const string& hexString);
    void add(int x1, int x2, int resultx, Register& reg);
};

class Machine
{
private:
    std::fstream programFile;
    int programCounter{};
    std::string instructionRegister{};
    Register registers;
    ALU alu;
    CU cu;
    Memory memory;

public:
    bool loadProgramFile(std::string&);
    void fetch();
    int decode(char);
    void execute();
    void outputState();
};

class MainUI
{
private:
    Machine machine;
    std::string fileName{};
public:
    int displayMenu();
    void inputFileName();
    char inputChoice();
};

#endif
