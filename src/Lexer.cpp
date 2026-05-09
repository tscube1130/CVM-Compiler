#include "Lexer.h"
#include <cctype> // for isdigit, isalpha, isalnum

Lexer::Lexer(const std::string& sourceCode) : source(sourceCode) {
    // Register all the reserved keywords for CVM++
    keywords["let"] = TokenType::LET;
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["while"] = TokenType::WHILE;
    keywords["print"] = TokenType::PRINT;
    keywords["input"] = TokenType::INPUT;
    keywords["true"] = TokenType::BOOLEAN;
    keywords["false"] = TokenType::BOOLEAN;
    keywords["def"] = TokenType::DEF;
    keywords["return"] = TokenType::RETURN;
}

bool Lexer::isAtEnd() const { 
    return current >= source.length(); 
}

char Lexer::advance() { 
    return source[current++]; 
}

char Lexer::peek() const { 
    if (isAtEnd()) return '\0'; 
    return source[current]; 
}

void Lexer::addToken(TokenType type, std::vector<Token>& tokens) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

// Consumes continuous digits
void Lexer::number(std::vector<Token>& tokens) {
    while (isdigit(peek())) {
        advance();
    }
    addToken(TokenType::NUMBER, tokens);
}

// Consumes continuous letters/numbers and checks if it's a keyword
void Lexer::identifier(std::vector<Token>& tokens) {
    while (isalnum(peek())) {
        advance();
    }
    
    // Grab the string we just consumed
    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER; // Assume it's a variable name first
    
    // If the word exists in our keyword map, update the type
    if (keywords.find(text) != keywords.end()) {
        type = keywords[text];
    }
    
    addToken(type, tokens);
}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        start = current; // Reset the start pointer for the new token
        char c = advance();

        switch (c) {
            case '+': addToken(TokenType::PLUS, tokens); break;
            case '-': addToken(TokenType::MINUS, tokens); break;
            case '*': addToken(TokenType::STAR, tokens); break;
            case '/': addToken(TokenType::SLASH, tokens); break;
            case '<': addToken(TokenType::LESS, tokens); break;
            case '(': addToken(TokenType::LEFT_PAREN, tokens); break;
            case ')': addToken(TokenType::RIGHT_PAREN, tokens); break;
            case '{': addToken(TokenType::LEFT_BRACE, tokens); break;
            case '}': addToken(TokenType::RIGHT_BRACE, tokens); break;
            case ',': addToken(TokenType::COMMA, tokens); break;
            
            // Handle single '=' vs double '=='
            case '=': 
                if (peek() == '=') {
                    advance(); // Consume the second '='
                    addToken(TokenType::EQUAL_EQUAL, tokens);
                } else {
                    addToken(TokenType::EQUAL, tokens);
                }
                break;

            // Ignore whitespace
            case ' ': case '\r': case '\t': 
                break;
            case '\n': 
                line++; 
                break;

            default:
                if (isdigit(c)) {
                    number(tokens);
                } else if (isalpha(c)) {
                    identifier(tokens);
                } else {
                    // If it's a random symbol we don't support (like @ or ^)
                    tokens.push_back({TokenType::ERROR, std::string(1, c), line});
                }
                break;
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}