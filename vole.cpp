#include "vole.h"

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

string ALU::hexToDec(const string& hexString) {
    int dec;
    stringstream ss;
    ss << hex << hexString;
    ss >> dec;
    return to_string(dec);
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