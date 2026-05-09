# ⚡ CVM++ : Custom Virtual Machine & Bytecode Compiler

A lightweight, stack-based Virtual Machine and Bytecode Compiler built entirely from scratch in modern C++17. 

CVM++ parses a custom, high-level scripting language into an Abstract Syntax Tree (AST), flattens it into a custom 1D bytecode Instruction Set Architecture (ISA), and executes it via a high-performance virtual CPU. **Zero external parser generators (like Flex or Bison) were used.**

## 🧠 Core Architecture Pipeline

The engine translates human-readable scripts into machine execution through a strict 4-phase pipeline:
1. **Lexical Analyzer (Scanner):** Converts raw string source code into a stream of categorized Tokens (Identifiers, Keywords, Operators, Punctuation).
2. **Recursive Descent Parser:** Consumes tokens to enforce grammar rules and builds a deeply nested 2D Abstract Syntax Tree (AST) using smart pointers.
3. **Bytecode Compiler:** Walks the AST and flattens it into a 1D array of raw Bytecode Opcodes, utilizing forward-jumping offsets for control flow and loops.
4. **Stack-Based Virtual Machine:** A simulated CPU featuring an Instruction Pointer (`ip`), an Execution Stack for math, and a Call Stack to support function execution and parameter passing.

## 🛠️ Getting Started 

### Prerequisites
You only need a standard C++ compiler (like `g++` or `clang++`) installed on your system.

### 1. Clone & Compile
Clone the repository and compile the source code directly from the `src/` directory. No `Makefile` or `CMake` required:

```bash
git clone [https://github.com/tscube1130/CVM-Compiler.git](https://github.com/tscube1130/CVM-Compiler.git)
cd CVM-Compiler
g++ src/main.cpp src/Lexer.cpp src/Parser.cpp src/Compiler.cpp src/VM.cpp -o cvm
```

### 2. Run a Script
Create a text file (e.g., `test.cvm`), write your custom CVM script, and execute it by passing the file to the engine:

```bash
./cvm examples/test.cvm
```
*(Note for Windows users: Your compiler will generate `cvm.exe`. Run it using `./cvm.exe examples/test.cvm`)*

## 🧪 Interactive Test Suite

Want to see what the engine can do? Copy and paste any of these test cases into a `.cvm` file and run it!

**Test Case 1: Variables & Interactive I/O**
Testing lexical tracking and the native VM halt/resume mechanism for user input.
```javascript
let base = 100
let multiplier = input

def calculate(a, b) {
    let result = a * b
    return result
}

let finalAnswer = calculate(base, multiplier)
print finalAnswer
```

**Test Case 2: Control Flow & Loops**
Testing the Compiler's bytecode offset patching (`OP_JUMP` and `OP_JUMP_IF_FALSE`).
```javascript
let count = 0
let limit = 5

while count < limit {
    if count < 3 {
        print 0 
    } 
    if 2 < count {
        print 1 
    }
    let count = count + 1
}
```

**Test Case 3: The Final Boss (Calculations)**
Testing the Virtual Machine's Call Stack and math operations by calculating the 10th number in the Fibonacci sequence.
```javascript
def fibonacci(n) {
    let a = 0
    let b = 1
    let count = 0
    
    while count < n {
        let temp = a + b
        let a = b
        let b = temp
        let count = count + 1
    }
    return a
}

let ans = fibonacci(10)
print ans // Expected output: 55
```


## 🗺️ Future Roadmap
- [ ] Add support for String types and concatenation.
- [ ] Implement Arrays/Lists.
- [ ] Introduce a Standard Library (e.g., `math.random()`).
- [ ] Build a mark-and-sweep Garbage Collector.

## 📄 License
This project is open-source and available under the MIT License.