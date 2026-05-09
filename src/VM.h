#pragma once
#include "Chunk.h"
#include <vector>
#include <unordered_map>
#include <string>

// The Bookmark!
struct CallFrame {
    Function* function;
    int ip; // The instruction pointer specifically for this function
};

class VM {
private:
    std::vector<double> stack;
    
    // The new brain: A stack of frames instead of a single chunk/ip
    std::vector<CallFrame> frames; 
    
    // Dictionary of all compiled functions
    std::unordered_map<std::string, Function> functions;
    
    // Memory bank for variables
    std::unordered_map<std::string, double> globals;

    void push(double value);
    double pop();

public:
    VM();
    
    // Notice how this now takes the whole dictionary from the Compiler!
    void interpret(std::unordered_map<std::string, Function>& compiledFunctions);
    
    void run();
};