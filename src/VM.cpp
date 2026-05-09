#include "VM.h"
#include <iostream>

VM::VM() {}

void VM::push(double value) { stack.push_back(value); }

double VM::pop() {
    double value = stack.back();
    stack.pop_back();
    return value;
}

void VM::interpret(std::unordered_map<std::string, Function>& compiledFunctions) {
    functions = compiledFunctions;
    
    // Setup the entry point (the main script)
    CallFrame mainFrame;
    mainFrame.function = &functions["main"];
    mainFrame.ip = 0;
    
    frames.push_back(mainFrame);
    run();
}

void VM::run() {
    while (true) {
        // ALWAYS fetch the current frame (the function currently executing)
        CallFrame* frame = &frames.back(); 
        
        // Read the instruction and advance THIS frame's pointer
        uint8_t instruction = frame->function->chunk.code[frame->ip++];
        
        switch (static_cast<Opcode>(instruction)) {
            case Opcode::PUSH_CONST: {
                uint8_t constantIndex = frame->function->chunk.code[frame->ip++];
                push(frame->function->chunk.constants[constantIndex]);
                break;
            }
            case Opcode::ADD: { double b=pop(); double a=pop(); push(a+b); break; }
            case Opcode::SUBTRACT: { double b=pop(); double a=pop(); push(a-b); break; }
            case Opcode::MULTIPLY: { double b=pop(); double a=pop(); push(a*b); break; }
            case Opcode::DIVIDE: { 
                double b=pop(); 
                double a=pop(); 
                if (b == 0.0) {
                    std::cerr << "Runtime Error: Division by zero.\n";
                    exit(1);
                }
                push(a/b); 
                break; 
            }
            case Opcode::LESS: { double b=pop(); double a=pop(); push(a<b?1.0:0.0); break; }
            case Opcode::EQUAL_COMPARE: { double b=pop(); double a=pop(); push(a==b?1.0:0.0); break; }
            
            case Opcode::DEFINE_VAR: {
                uint8_t nameIndex = frame->function->chunk.code[frame->ip++];
                std::string varName = frame->function->chunk.identifiers[nameIndex];
                globals[varName] = pop();
                break;
            }
            case Opcode::GET_VAR: {
                uint8_t nameIndex = frame->function->chunk.code[frame->ip++];
                std::string varName = frame->function->chunk.identifiers[nameIndex];
                push(globals[varName]);
                break;
            }
            case Opcode::PRINT: {
                std::cout << pop() << "\n";
                break;
            }
            case Opcode::INPUT_OP: {
                double val; std::cout << ">> "; std::cin >> val; push(val); break;
            }
            case Opcode::JUMP_IF_FALSE: {
                uint8_t offset = frame->function->chunk.code[frame->ip++];
                if (pop() == 0.0) frame->ip += offset;
                break;
            }
            case Opcode::LOOP: {
                uint8_t offset = frame->function->chunk.code[frame->ip++];
                frame->ip -= offset;
                break;
            }
            case Opcode::HALT: {
                return; // Stop the VM completely
            }
            
            // --- NEW PHASE 6: Teleporting! ---
            case Opcode::CALL: {
                // 1. Figure out which function we are calling
                uint8_t nameIndex = frame->function->chunk.code[frame->ip++];
                std::string funcName = frame->function->chunk.identifiers[nameIndex];
                Function* targetFunc = &functions[funcName];
                
                // 2. Map the popped stack arguments to the function's parameter names
                for (int i = targetFunc->arity - 1; i >= 0; i--) {
                    std::string paramName = targetFunc->chunk.identifiers[i];
                    globals[paramName] = pop();
                }
                
                // 3. Create the new Call Frame bookmark
                CallFrame newFrame;
                newFrame.function = targetFunc;
                newFrame.ip = 0;
                
                // 4. Push it to the stack! The next loop iteration will run this function!
                frames.push_back(newFrame);
                break;
            }
            case Opcode::RETURN: {
                // 1. Grab the final answer off the stack
                double result = pop(); 
                
                // 2. Destroy the current function's Call Frame (Teleport back!)
                frames.pop_back();     
                
                // 3. Put the answer back on the stack for the calling code to use
                push(result);          
                break;
            }
        }
    }
}