#pragma once
#include <string>

enum class TokenType {
    // Single-character tokens
    PLUS, MINUS, STAR, SLASH, EQUAL, LESS,
    
    // --- NEW PHASE 6 PUNCTUATION ---
    LEFT_PAREN, RIGHT_PAREN,   // ( )
    LEFT_BRACE, RIGHT_BRACE,   // { }
    COMMA,                     // ,

    // Two-character tokens
    EQUAL_EQUAL,

    // Literals
    IDENTIFIER, NUMBER, BOOLEAN,

    // Keywords
    LET, PRINT, INPUT, IF, WHILE, 
    
    // --- NEW PHASE 6 KEYWORDS ---
    DEF, RETURN,ELSE,ERROR,

    END_OF_FILE
};

// Removed the custom constructor so C++ brace initialization works again!
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};