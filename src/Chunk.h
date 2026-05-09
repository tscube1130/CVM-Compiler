#pragma once
#include <vector>
#include <cstdint>
#include <iostream>

// Our Virtual Machine's Instruction Set
enum class Opcode : uint8_t {
    PUSH_CONST,  // Load a number onto the stack
    ADD,         // Add top two numbers
    SUBTRACT,    // Subtract top two numbers
    MULTIPLY,    // Multiply top two numbers
    DIVIDE,      // Divide top two numbers
    DEFINE_VAR,  // Store a variable
    PRINT,       // Pop top of stack and print it
    GET_VAR,     // Fetch a variable from memory
    LESS,           // Handle '<'
    EQUAL_COMPARE,  // Handle '=='
    INPUT_OP,       // Handle 'input'
    
    // Jump instructions for Control Flow
    JUMP_IF_FALSE,  // Skip the 'if' body if the condition is false
    JUMP,           // Unconditional jump forward (used for 'else')
    LOOP,            // Jump backwards (used for 'while')
    CALL,    // Jump to a function
    RETURN,  // Exit a function and send a value back
    HALT         // Stop execution
};

struct Chunk {
    std::vector<uint8_t> code;       // The raw bytecode array
    std::vector<double> constants;   // Pool of numbers used in the script

    // Helper to add a byte
    void write(uint8_t byte) {
        code.push_back(byte);
    }

    // Helper to add a number and return its index
    uint8_t addConstant(double value) {
        constants.push_back(value);
        return static_cast<uint8_t>(constants.size() - 1);
    }
    std::vector<std::string> identifiers; 
    
    uint8_t addIdentifier(const std::string& name) {
        identifiers.push_back(name);
        return static_cast<uint8_t>(identifiers.size() - 1);
    }
    
    // For debugging: Print out the raw bytes
   // For debugging: Print out the raw bytes
    void disassemble() const {
        std::cout << "--- Compiled Bytecode ---\n";
        for (size_t i = 0; i < code.size(); i++) {
            std::cout << "[" << i << "] ";
            Opcode op = static_cast<Opcode>(code[i]);
            
            switch (op) {
                // Instructions with a 1-byte payload
                case Opcode::PUSH_CONST:
                    std::cout << "PUSH_CONST " << (int)code[++i] << " (Value: " << constants[code[i]] << ")\n";
                    break;
                case Opcode::DEFINE_VAR:
                    std::cout << "DEFINE_VAR " << (int)code[++i] << " (Name: " << identifiers[code[i]] << ")\n";
                    break;
                case Opcode::GET_VAR:
                    std::cout << "GET_VAR " << (int)code[++i] << " (Name: " << identifiers[code[i]] << ")\n";
                    break;
                case Opcode::JUMP_IF_FALSE:
                    std::cout << "JUMP_IF_FALSE (Offset: " << (int)code[++i] << ")\n";
                    break;
                case Opcode::JUMP:
                    std::cout << "JUMP (Offset: " << (int)code[++i] << ")\n";
                    break;
                case Opcode::LOOP:
                    std::cout << "LOOP (Offset: " << (int)code[++i] << ")\n";
                    break;

                // Single-byte instructions (No payload)
                case Opcode::ADD: std::cout << "ADD\n"; break;
                case Opcode::SUBTRACT: std::cout << "SUBTRACT\n"; break;
                case Opcode::MULTIPLY: std::cout << "MULTIPLY\n"; break;
                case Opcode::DIVIDE: std::cout << "DIVIDE\n"; break;
                case Opcode::LESS: std::cout << "LESS\n"; break;
                case Opcode::EQUAL_COMPARE: std::cout << "EQUAL_COMPARE\n"; break;
                case Opcode::INPUT_OP: std::cout << "INPUT_OP\n"; break;
                case Opcode::PRINT: std::cout << "PRINT\n"; break;
                case Opcode::HALT: std::cout << "HALT\n"; break;
                
                default: std::cout << "UNKNOWN (" << (int)op << ")\n"; break;
            }
        }
    }
};

struct Function {
    std::string name;
    int arity = 0; // The number of arguments (e.g., multiply(a, b) has an arity of 2)
    Chunk chunk;
};