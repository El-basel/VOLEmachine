#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <cstdint>
#include "vole2.h"

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
    // divide the instruction into two bytes to insert in the memory one byte only for every location
    for (int i = 0; i < value.length(); i += 2) {
        memory[index] = value.substr(i,2);
        ++index;
    }
}

// reset the machine memory after halting from the program
void Memory::reset() {
    std::fill(std::begin(memory), std::end(memory), "00");
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
// Convert from hex to float using the (sign, exponent, mantissa) notation
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

std::string ALU::floatToHex(double number){
    std::bitset<8> bits;
    if(number < 0)
    {
        bits[7] = true;
    }
    else
    {
        bits[7] = false;
    }
    int integerPart = std::abs(static_cast<int>(number));
    std::string integerBinary = "";
    while (integerPart > 0)
    {
        integerBinary = std::to_string(integerPart % 2) + integerBinary;
        integerPart /= 2;
    }
    int exponent = 3;
    int i = 0;
    for (; i < integerBinary.length() ; ++i) {
        if(integerBinary[i] == '1')
        {
            exponent += integerBinary.length() - (i + 1);
            break;
        }
    }
    ++i;
    bits[6] = exponent & 4;
    bits[5] = exponent & 2;
    bits[4] = exponent & 1;
    double fractionalPart = std::abs(number) - std::abs(static_cast<int>(number));
    std::string fractionalBinary = "";
    for (int j = 0; j < 4; ++j) {
        fractionalPart *= 2;
        if(fractionalPart >= 1.0)
        {
            fractionalBinary += "1";
            fractionalPart -= 1.0;
        }
        else
        {
            fractionalBinary += "0";
        }
    }
    std::string binary = integerBinary.substr(i) + fractionalBinary;
    for (int j = 0; j < 4; ++j) {
        if(binary[j] == '1')
        {
            bits[4 - j - 1] = true;
        }
        else
        {
            bits[4 - j - 1] = false;
        }
    }
    int finalNumber = bits.to_ullong();
    return decToHex(finalNumber);
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
bool CU::store(int reg1, int loc, Register& reg, Memory& mem,int& programEnd,int& programStart){
    // if the location is zero we output the content of the given register
    if (loc == 0) {
        std::cout << alu.decToHex(reg.getCell(reg1)) << '\n';
    }
        // check if the user wants to save content in a placw where memory is saved to avoid crashes
        // return zero to let the machine class know that it needs to halt execution
    else if (loc > programStart and loc < programEnd) {
        cout << "Error: Attempt to access restricted memory. Operation not permitted." << endl;
        return 0;
    }
    else{
        int num = reg.getCell(reg1);
        mem.setCell(loc, alu.decToHex(num));
    }
    return 1;
}
void CU::move(int reg1, int reg2, Register& reg) {
    reg.setCell(reg2, reg.getCell(reg1));
}
void CU::jump(int reg1, int cell, Register& reg, int& counter) {
    if (reg.getCell(reg1) == reg.getCell(0)) {
        counter = cell;
    }
}
// Machine class

// Changed
bool Machine::loadProgramFile(std::string& file, std::string& memoryLocation) {
    programFile.open(file);
    std::string instruction;
    if(!programFile)
    {
        return false;
    }


    // store the program end in case the user entered another program after the provided one or entered individual instructions
    if(memoryLocation != "default")
    {
        programCounter = alu.hexToDec(memoryLocation, true);
        programEnd = programStart = programCounter;
    }
    while(!programFile.eof())
    {
        programFile >> instruction;
        memory.setCell(programEnd, instruction) ;
        programEnd += 2;
    }
    return true;
}

void Machine::halt() {
    registers.reset();
    memory.reset();
    programCounter = programEnd = programStart = 16;
    programFile.close();
    instructionRegister = "00";
}

// Get the instructions from memory
void Machine::fetch() {
    instructionRegister = memory.getCell(programCounter) + memory.getCell(programCounter + 1);
    programCounter += 2;
}

// There are two possible situations in the decoding process
// If we are decoding a numerical value for a mathematical operation, so it might be a positive or negative value
// and if we are decoding a numerical value to get the register or memory location
// In the second case we want the values to be positive always, that's why we provide a second parameter "positiveOnly"
int Machine::decode(std::string instruction, bool positiveOnly = false) {
    return alu.hexToDec(instruction,positiveOnly);
}

int Machine::execute() {
    // if the program memory is empty that's mean the user didn't load a program, so we alert the user about that
    if(memory.getCell(programCounter) == "00")
    {
        halt();
        std::cerr << "Please load a program first\n";
        return 1;
    }
    fetch();
    // In the decoding instruction process we decode every possible combination and then from the type of the operation we provide the appropriate arguments
    // That's mean even if the instruction is providing a memory location and not the two other registers
    // we decode the instruction in the two ways (as there are three registers, or as there are one register and a memory location)
    // This makes it easier as we won't make a big if-else condition to first check the type of the operation and see if it provides three registers or memory location
    // and then decode the rest of the instruction to the corresponding operation
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
            if (!cu.store(register1, memoryCell, registers, memory, programEnd, programCounter)) {
                halt();
            }
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
            halt();
            std::cout << "---------------\n";
            std::cout << "| Program End |\n";
            std::cout << "---------------\n";
            return 1;
        default:
            std::cerr << "Invalid instruction\n";
            halt();
            return 1;
    }
    return 0;
}

void Machine::memoryOutput()
{
    std::string cell{};
    if(ImGui::BeginTable("Memory",17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        int index = 0;
        ImGui::TableSetupColumn(" ");
        for (int i = 0; i < 16; ++i) {
            ImGui::TableSetupColumn(alu.decToHex(i).c_str());
        }
        ImGui::TableHeadersRow();
        for (int row = 0; row < 16; ++row) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(alu.decToHex(row).c_str());
            for (int column = 1; column < 17; ++column) {
                ImGui::TableSetColumnIndex(column);
                cell = memory.getCell(index);
                ++index;
                if(cell.length() == 1)
                {
                    cell = "0" + cell;
                }
                ImGui::Text(cell.c_str());
            }
        }
        ImGui::EndTable();
    }
}

void Machine::registerOutput()
{
    std::string cell{};
    if(ImGui::BeginTable("Registers",2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        int index = 0;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("IR");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text(instructionRegister.c_str());
        for (int row = 0; row < 16; ++row) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            cell = "R" + alu.decToHex(row);
            ImGui::Text(cell.c_str());
            for (int column = 1; column < 2; ++column) {
                ImGui::TableSetColumnIndex(column);
                int value = registers.getCell(index);
                if(value - int(value) != 0)
                {
                    cell = alu.floatToHex(value);
                }
                else
                {
                    cell = alu.decToHex(value);
                }
                if(cell.length() == 1)
                {
                    cell = "0" + cell;
                }
                ++index;
                ImGui::Text(cell.c_str());
            }
        }
        ImGui::EndTable();
    }
}

// add in the original one

void Machine::outputState(MachineFlags typeFlag) {
    if(typeFlag == MachineFlags::memory)
    {
        memoryOutput();
    }
    else if (typeFlag == MachineFlags::registers)
    {
        registerOutput();
    }
    else if(typeFlag == MachineFlags::IrAndPc)
    {
        ImGui::Text("IR: ");
        ImGui::SameLine();
        ImGui::Text(instructionRegister.c_str());
        ImGui::Text("PC: ");
        ImGui::SameLine();
        ImGui::Text(alu.decToHex(programCounter).c_str());
    }
}

// Changed
bool Machine::insertInstruction(std::string& instruction, std::string& memoryLocation) {
    // as the instruction is 2 bytes and the memory can only hold one byte
    int location;
    if(memoryLocation == "default")
    {
        location = programEnd;
    }
    else
    {
        location = alu.hexToDec(memoryLocation);
    }
    if(alu.isValid(instruction) and programStart == programEnd)
    {
        memory.setCell(location, instruction);
        programStart = programCounter = location;
        programEnd += 2;
        instruction = "";
        memoryLocation = "";
        return true;
    }
    if(alu.isValid(instruction) and (location < programStart or location >= programEnd))
    {
        memory.setCell(location, instruction);
        programEnd += 2;
        instruction = "";
        memoryLocation = "";
        return true;
    }
    else
    {
        instruction = "";
        memoryLocation = "";
        return false;
    }
}
// End of Machine class

// MainUI class
// Changed
int MainUI::displayMenu(MachineFlags choice) {
    switch (choice) {
        case registers:
            machine.outputState(registers);
            break;
        case memory:
            machine.outputState(memory);
            break;
        case IrAndPc:
            machine.outputState(IrAndPc);
            break;
        case execute:
            if(machine.execute())
                programRunning = false;
            break;
        case halt:
            machine.halt();
            programRunning = false;
            break;
    }
    return 1;
}

// Ask the user for the program file name
void MainUI::inputFileName(bool &enterFileName) {
    if(enterFileName)
    {
        static std::string memoryLocation{"0A"};
        ImGui::OpenPopup("Enter Program File Name");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Enter Program File Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("File Name:");
            ImGui::InputText(":Name ",&fileName);
            ImGui::Separator();
            if(!programRunning)
            {
                ImGui::Text("Memory Location:");
                ImGui::InputText(":Location ",&memoryLocation);
                ImGui::Separator();
            }
            else
            {
                memoryLocation = "default";
            }

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                if(machine.loadProgramFile(fileName, memoryLocation))
                {
                    std::cout << fileName << std::endl;
                    std::cout << "Program loaded successfully" << std::endl;
                    programRunning = true;
                    memoryLocation = "0A";
                }
                else
                {
                    inputError = true;
                    std::cerr << "Program didn't load" << std::endl;
                    fileName = "";
                    programRunning = false;
                }
                enterFileName = false;
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel", ImVec2(120,0)))
            {
                fileName = "";
                ImGui::CloseCurrentPopup();
                enterFileName = false;
            }
            ImGui::EndPopup();
        }
    }

    if(inputError)
    {
        msg = "Please load a program, and in the correct memory location";
    }
}

// Get the user choice from the Main menu options
char MainUI::inputChoice() {
    string choice1;
    std::cout << "Enter your choice: ";
    getline(std::cin >> std::ws, choice1);
    while (choice1.size() != 1 or choice1[0] < 'a' or choice1[0] > 'e')
    {
        std::cout << "Please choose an option from the above only\n";
        std::cout << "Enter your choice: ";
        getline(std::cin >> std::ws, choice1);
    }
    return choice1[0];
}


std::string MainUI::getFileName() {
    return fileName;
}

bool MainUI::checkProgramRunning() {
    return programRunning;
}

// added
void MainUI::popup()
{
    // Always center this window when appearing

}

// added
void MainUI::insertInstruction(bool modify) {
    if(modify)
    {
        insert = true;
    }
    if(insert)
    {
        static std::string instruction{};
        static std::string memoryLocation{};
        ImGui::OpenPopup("Insert Instruction");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Insert Instruction", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Instruction (in hexadecimal):");
            ImGui::SameLine();
            ImGui::InputText(": Instruction",&instruction);
            if(!programRunning)
            {
                ImGui::Text("Memory Location:");
                ImGui::SameLine();
                ImGui::InputText(": Location",&memoryLocation);
            }
            else
            {
                memoryLocation = "default";
            }
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                if(!instruction.empty() and machine.insertInstruction(instruction, memoryLocation))
                {
                    programRunning = true;
                }
                else
                {
                    programRunning = false;
                    inputError = true;
                }
                insert = false;
            }

            ImGui::SameLine();
            if(ImGui::Button("Cancel", ImVec2(120,0)))
            {
                insert = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    if(inputError)
    {
        msg = "Instruction";
    }
}
// End of MainUI class