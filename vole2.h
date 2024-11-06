#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <fstream>
#include <vector>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
using namespace std;

#ifndef VOLE_H
#define VOLE_H
enum MachineFlags : int
{
    memory = 1,
    registers,
    IrAndPc,
    insertInstruction,
    execute,
    halt,
};

class Register
{
private:
    double memory[16]{};
    int size = 16;
public:
    Register();
    double getCell(int index);
    void setCell(int index, double value);
    void reset();
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
    void reset();
};


class ALU
{
public:
    int hexToDec(const string& hexString, bool);
    double hexToFloat(const string&);
    string decToHex(const double& decNumber);
    bool isValid(const string& hexString);
    std::string floatToHex(double number);
    void addDecimal(int x1, int x2, int resultx, Register& reg);
    void addFloat(int x1, int x2, int resultx, Register& reg);
};
class CU
{
private:
    ALU alu;
public:
    void load(int, int, Register&, Memory&);
    void load(int, double, Register&);
    bool store(int, int, Register&, Memory&,int&,int&);
    void move(int, int, Register&);
    void jump(int, int, Register&, int&);

};

class Machine
{
private:
    std::fstream programFile;
    int programCounter{16};
    int programStart{16};
    int programEnd{16};
    std::string instructionRegister{"00"};
    Register registers;
    ALU alu;
    CU cu;
    Memory memory;
    bool insert = true;
public:
    bool loadProgramFile(std::string&, std::string&);
    void fetch();
    void halt();
    int  decode(std::string, bool);
    int  execute();
    bool insertInstruction(std::string&, std::string&);
    void outputState(MachineFlags);
    void memoryOutput();
    void registerOutput();
};

class MainUI
{
private:
    std::string fileName{};
    Machine machine;
    bool programRunning = false;
    bool insert = false;
public:
    std::string msg{};
    bool inputError = false;
    int  displayMenu(MachineFlags);
    void inputFileName(bool &);
    char inputChoice();
    std::string getFileName();
    bool checkProgramRunning();
    void popup();
    void insertInstruction(bool);
};

#endif
