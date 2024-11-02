#include <iostream>
#include <stack>
#include <string>
#include <conio.h>
#include "CLI_QOL.h"

using namespace std;

enum class ParseError {
    DIVISION_BY_ZERO,
    SYNTAX_ERROR,
    UNCLOSED_BRACKETS,
    INVALID_CHARACTERS,
    EMPTY,
    NONE
};

class Operation {
public:
    virtual double Calculate() const = 0;
    virtual string GetStructure() const = 0;    // для тестирования   
    virtual ~Operation() {}
};

class Number : public Operation {
public:
    Number(double _value) : value(_value) {}
    double Calculate() const override;
    string GetStructure() const override;
private:
    double value;
};

class Add : public Operation {
public:
    Add(Operation* _left, Operation* _right) : left(_left), right(_right) {}
    double Calculate() const override;
    string GetStructure() const override;
private:
    unique_ptr<Operation> left;
    unique_ptr<Operation> right;
};

class Subtract : public Operation {
public:
    Subtract(Operation* _left, Operation* _right) : left(_left), right(_right) {}
    double Calculate() const override;
    string GetStructure() const override;
private:
    unique_ptr<Operation> left;
    unique_ptr<Operation> right;
};

class Multiply : public Operation {
public:
    Multiply(Operation* _left, Operation* _right) : left(_left), right(_right) {}
    double Calculate() const override;
    string GetStructure() const override;
private:
    unique_ptr<Operation> left;
    unique_ptr<Operation> right;
};

class Divide : public Operation {
public:
    Divide(Operation* _left, Operation* _right) : left(_left), right(_right) {}
    double Calculate() const override;
    string GetStructure() const override;
private:
    unique_ptr<Operation> left;
    unique_ptr<Operation> right;
};

class ExpressionParser {
public:
    ExpressionParser(string expr) : expression(expr), errorCode(ParseError::NONE) {
        removeSpaces();
    }
    string getRPN() const;
    unique_ptr<Operation> parse();
    ParseError getErrorCode() const;
private:
    string expression;
    string rpnOutput;
    stack<unique_ptr<Operation>> values;
    stack<char> operators;
    ParseError errorCode;
    int parenthesesBalance = 0;

    bool isOperator(char c);
    bool isValidNonInteger(size_t index);
    bool isValidNegative(size_t index);
    bool isNumberStart(size_t index);
    bool isParentheses(size_t& index);
    bool isValidChar(char index);
    bool isOperatorAtStartOfExpression(size_t index);
    bool isInvalidOperatorPrecedence(size_t& index);
    int getPrecedence(char op);
    unique_ptr<Operation> applyOperation(char op, unique_ptr<Operation> left, unique_ptr<Operation> right);
    void processOperators();
    void processCurrentOperator(char op);
    void applyTopOperator();
    void removeSpaces();
    void handleNumber(size_t& index);
    void handleOperator(size_t& index);
    void handleParentheses(size_t& index);
    void tokenize();
};

class Calculator {
public:
    void printResult(string expression);
    void printStructure(string expression);
    void printRPN(string expression);
private:
    void printError(ParseError error);
};