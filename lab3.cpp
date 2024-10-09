#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>

using namespace std;

// Перелічення для типів лексем
enum class TokenType {
    Number,
    FloatNumber,
    HexNumber,
    StringLiteral,
    CharLiteral,
    PreprocessorDirective,
    Comment,
    ReservedWord,
    Operator,
    Punctuation,
    Identifier,
    Unknown
};

// Структура лексеми
struct Token {
    string lexeme;
    TokenType type;
};

// Таблиці зарезервованих слів, операторів та розділових знаків
const vector<string> reservedWords = {"int", "float", "if", "else", "while", "return", "char", "string"};
const vector<string> operators = {"+", "-", "*", "/", "=", "==", "<", ">", "&&", "||", "!", "!="};
const vector<string> punctuation = {",", ";", "(", ")", "{", "}", "[", "]"};

// Функція для розпізнавання типу лексеми
TokenType recognizeTokenType(const string &lexeme) {
    if (regex_match(lexeme, regex("^[0-9]+$"))) {
        return TokenType::Number;
    }
    if (regex_match(lexeme, regex("^[0-9]*\\.[0-9]+$"))) {
        return TokenType::FloatNumber;
    }
    if (regex_match(lexeme, regex("^0[xX][0-9a-fA-F]+$"))) {
        return TokenType::HexNumber;
    }
    if (regex_match(lexeme, regex("^\".*\"$"))) {
        return TokenType::StringLiteral;
    }
    if (regex_match(lexeme, regex("^'.'$"))) {
        return TokenType::CharLiteral;
    }
    if (lexeme[0] == '#') {
        return TokenType::PreprocessorDirective;
    }
    if (lexeme.substr(0, 2) == "//" || lexeme.substr(0, 2) == "/*") {
        return TokenType::Comment;
    }
    if (find(reservedWords.begin(), reservedWords.end(), lexeme) != reservedWords.end()) {
        return TokenType::ReservedWord;
    }
    if (find(operators.begin(), operators.end(), lexeme) != operators.end()) {
        return TokenType::Operator;
    }
    if (find(punctuation.begin(), punctuation.end(), lexeme) != punctuation.end()) {
        return TokenType::Punctuation;
    }
    if (regex_match(lexeme, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
        return TokenType::Identifier;
    }
    return TokenType::Unknown;
}

// Функція для виводу типу лексеми
string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Number: return "Number";
        case TokenType::FloatNumber: return "FloatNumber";
        case TokenType::HexNumber: return "HexNumber";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::CharLiteral: return "CharLiteral";
        case TokenType::PreprocessorDirective: return "PreprocessorDirective";
        case TokenType::Comment: return "Comment";
        case TokenType::ReservedWord: return "ReservedWord";
        case TokenType::Operator: return "Operator";
        case TokenType::Punctuation: return "Punctuation";
        case TokenType::Identifier: return "Identifier";
        default: return "Unknown";
    }
}

// Основна функція лексичного аналізатора
void lexicalAnalysis(const string &code) {
    vector<Token> tokens;
    string lexeme;
    bool isInString = false;
    bool isInChar = false;
    bool isInComment = false;

    for (size_t i = 0; i < code.length(); ++i) {
        char ch = code[i];

        // Пропуск пробілів поза рядковими константами
        if (!isInString && !isInChar && !isInComment && isspace(ch)) {
            continue;
        }
        // Обробка рядкових констант
        if (ch == '\"') {
            if (isInString) {
                lexeme += ch;
                tokens.push_back({lexeme, TokenType::StringLiteral});
                lexeme.clear();
                isInString = false;
            } else {
                isInString = true;
                lexeme = ch;
            }
            continue;
        }

        // Обробка символьних констант
        if (ch == '\'') {
            if (isInChar) {
                lexeme += ch;
                tokens.push_back({lexeme, TokenType::CharLiteral});
                lexeme.clear();
                isInChar = false;
            } else {
                isInChar = true;
                lexeme = ch;
            }
            continue;
        }

        // Обробка коментарів
        if (code.substr(i, 2) == "//") {
            lexeme = code.substr(i, 2);
            isInComment = true;
            i += 1; // Пропуск другого символу коментаря
            while (i < code.length() && code[i] != '\n') {
                lexeme += code[++i];
            }
            tokens.push_back({lexeme, TokenType::Comment});
            lexeme.clear();
            isInComment = false;
            continue;
        }

        if (code.substr(i, 2) == "/*") {
            lexeme = code.substr(i, 2);
            isInComment = true;
            i += 1;
            while (i < code.length() && code.substr(i, 2) != "*/") {
                lexeme += code[++i];
            }
            lexeme += "*/"; // Додати завершення коментаря
            i += 1; // Пропуск "*/"
            tokens.push_back({lexeme, TokenType::Comment});
            lexeme.clear();
            isInComment = false;
            continue;
        }

        // Обробка операторів і розділових знаків
        if (find(operators.begin(), operators.end(), string(1, ch)) != operators.end()) {
            tokens.push_back({string(1, ch), TokenType::Operator});
            continue;
        }

        if (find(punctuation.begin(), punctuation.end(), string(1, ch)) != punctuation.end()) {
            tokens.push_back({string(1, ch), TokenType::Punctuation});
            continue;
        }

        // Формування лексеми, доки не зустрінемо оператор або розділовий знак
        lexeme += ch;

        // Перевірка наступного символу, чи є він оператором, розділовим знаком або пробілом
        if (i + 1 == code.length() || 
            find(operators.begin(), operators.end(), string(1, code[i + 1])) != operators.end() ||
            find(punctuation.begin(), punctuation.end(), string(1, code[i + 1])) != punctuation.end() ||
            isspace(code[i + 1])) {

            TokenType type = recognizeTokenType(lexeme);
            tokens.push_back({lexeme, type});
            lexeme.clear();
        }
    }

    // Вивід результатів
    for (const auto &token : tokens) {
        cout << "< " << token.lexeme << " , " << tokenTypeToString(token.type) << " >" << endl;
    }
}

// Точка входу
int main() {
    // Вхідний код для аналізу
    string code = R"(int main() {
    float number = 123.45;
    char c = 'a';
    string s = "Hello, world!";
    // Це коментар
    return 0;
})";

    // Виклик лексичного аналізу
    lexicalAnalysis(code);

    return 0;
}
