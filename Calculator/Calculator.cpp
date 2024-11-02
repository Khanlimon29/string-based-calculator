#include <iostream>
#include <stack>
#include <string>
#include "CLI_QOL.h"
#include "Caclulator.h"

using namespace std;

double Number::Calculate() const {
    return value;
}
string Number::GetStructure() const {
    if (value == static_cast<int>(value)) {
        return "Number(" + to_string(static_cast<int>(value)) + ")";  // вывод без десятичной части
    }
    return "Number(" + to_string(value) + ")";   // для тестирования
}


double Add::Calculate() const {
    return left->Calculate() + right->Calculate();
}
string Add::GetStructure() const {
    return "Add(" + left->GetStructure() + ", " + right->GetStructure() + ")";   // для тестирования
}


double Subtract::Calculate() const {
    return left->Calculate() - right->Calculate();
}
string Subtract::GetStructure() const {
    return "Subtract(" + left->GetStructure() + ", " + right->GetStructure() + ")";   // для тестирования
}


double Multiply::Calculate() const {
    return left->Calculate() * right->Calculate();
}
string Multiply::GetStructure() const {
    return "Multiply(" + left->GetStructure() + ", " + right->GetStructure() + ")";   // для тестирования
}


double Divide::Calculate() const {
    return left->Calculate() / right->Calculate();
}
string Divide::GetStructure() const {
    return "Divide(" + left->GetStructure() + ", " + right->GetStructure() + ")";   // для тестирования
}


unique_ptr<Operation> ExpressionParser::parse() {
    if (expression.empty()) {
        errorCode = ParseError::EMPTY;
        return nullptr;
    }
    tokenize();
    if (errorCode != ParseError::NONE) {
        return nullptr;
    }
    processOperators();
    return move(values.top());
}



ParseError ExpressionParser::getErrorCode() const {
    return errorCode;
}

int ExpressionParser::getPrecedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

unique_ptr<Operation> ExpressionParser::applyOperation(char op, unique_ptr<Operation> left, unique_ptr<Operation> right) {
    if (op == '/' && right->Calculate() == 0) {
        errorCode = ParseError::DIVISION_BY_ZERO;
        return nullptr;
    }
    switch (op) {
    case '+': return make_unique <Add>(left.release(), right.release());
    case '-': return make_unique <Subtract>(left.release(), right.release());
    case '*': return make_unique <Multiply>(left.release(), right.release());
    case '/': return make_unique <Divide>(left.release(), right.release());
    default: return nullptr;
    }
}

void ExpressionParser::processOperators() {
    while (!operators.empty()) {
        applyTopOperator();
    }
}

void ExpressionParser::processCurrentOperator(char op) {
    while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(op)) {
        applyTopOperator();
    }
    operators.push(op);
}

void ExpressionParser::applyTopOperator() {
    char op = operators.top();
    operators.pop();

    unique_ptr<Operation> right = move(values.top());
    values.pop();
    unique_ptr<Operation> left = move(values.top());
    values.pop();

    values.push(applyOperation(op, move(left), move(right)));
    rpnOutput += op;
    rpnOutput += ' ';
}

void ExpressionParser::removeSpaces() {
    expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());
}

bool ExpressionParser::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool ExpressionParser::isValidNonInteger(size_t index) {
    return expression[index] == '.' && index + 1 < expression.length() && isdigit(expression[index + 1]);
}

bool ExpressionParser::isValidNegative(size_t index) {
    return expression[index] == '-' && (index == 0 || expression[index - 1] == '(') && index + 1 < expression.length() && isdigit(expression[index + 1]);
}

bool ExpressionParser::isOperatorAtStartOfExpression(size_t index) {
    return values.empty() && expression[index] != '-';
}

bool ExpressionParser::isInvalidOperatorPrecedence(size_t& index) {
    return !values.empty() && !operators.empty() && isOperator(expression[index - 1]) && expression[index - 1] != '(';
}

bool ExpressionParser::isParentheses(size_t& index) {
    return expression[index] == '(' || expression[index] == ')';
}

bool ExpressionParser::isValidChar(char index) {
    return isdigit(index) || index == '+' || index == '-' || index == '*' || index == '/' || index == '(' || index == ')' || index == '.';
}

bool ExpressionParser::isNumberStart(size_t index) {
    if (index >= expression.length()) return false; // защита от вылета с index + 1
    if (isdigit(expression[index])) return true;
    if (isValidNonInteger(index)) return true;
    if (isValidNegative(index)) {
        parenthesesBalance++;
        return true;
    }
    return false;
}

void ExpressionParser::handleNumber(size_t& index) {
    size_t length;
    double number = stod(expression.substr(index), &length);
    values.push(make_unique<Number>(number));
    if ((expression[index + length] == ')' && isValidNegative(index)) || (index == 0 && isValidNegative(index))) {
        parenthesesBalance--;
    }
    index += length;
}

void ExpressionParser::handleOperator(size_t& index) {
    if (isOperatorAtStartOfExpression(index) || isInvalidOperatorPrecedence(index)) {
        errorCode = ParseError::SYNTAX_ERROR;
    }
    else {
        processCurrentOperator(expression[index]);
    }
    index++;
}

void ExpressionParser::handleParentheses(size_t& index) {
    if (expression[index] == '(') {
        operators.push(expression[index]);
        parenthesesBalance++;
    }
    else if (expression[index] == ')') {
        parenthesesBalance--;
        while (!operators.empty() && operators.top() != '(') {
            applyTopOperator();
        }
        if (!(operators.empty() || parenthesesBalance < 0)) {
            operators.pop();
        }
    }
    index++;
}

void ExpressionParser::tokenize() {
    for (size_t index = 0; index < expression.length();) {
        if (!isValidChar(expression[index])) {
            errorCode = ParseError::INVALID_CHARACTERS;
            return;
        }
        else if (isParentheses(index)) {
            handleParentheses(index);
        }
        else if (isNumberStart(index)) {
            handleNumber(index);
            double numValue = static_cast<Number*>(values.top().get())->Calculate();
            if (numValue == static_cast<int>(numValue)) {
                rpnOutput += to_string(static_cast<int>(numValue)) + ' ';
            }
            else {
                rpnOutput += to_string(numValue) + ' ';
            }
        }
        else if (isOperator(expression[index])) {
            handleOperator(index);
        }
    }
    if (parenthesesBalance != 0) {
        errorCode = ParseError::UNCLOSED_BRACKETS;
        return;
    }

    if (isOperator(expression.back())) {
        errorCode = ParseError::SYNTAX_ERROR;
        return;
    }
}

string ExpressionParser::getRPN() const { return rpnOutput; }


void Calculator::printResult(string expression){
    ExpressionParser parser(expression);
    unique_ptr<Operation> operation = parser.parse();
    if (parser.getErrorCode() != ParseError::NONE) {
        printError(parser.getErrorCode());
        return;
    }
    cout << "Результат: " << operation->Calculate() << endl;
}

void Calculator::printStructure(string expression) {
    ExpressionParser parser(expression);
    unique_ptr<Operation> operation = parser.parse();
    if (parser.getErrorCode() != ParseError::NONE) {
        printError(parser.getErrorCode());
        return;
    }
    cout << "Структура выражения: " << operation->GetStructure() << endl;
}

void Calculator::printRPN(string expression) {
    ExpressionParser parser(expression);
    parser.parse();
    if (parser.getErrorCode() != ParseError::NONE) {
        printError(parser.getErrorCode());
        return;
    }
    cout << "Обратная польская запись: " << parser.getRPN() << endl;
}

void Calculator::printError(ParseError error) {
    SetColor(31); // Цвет текста ошибки
    switch (error) {
    case ParseError::DIVISION_BY_ZERO: cout << "Ошибка: Деление на ноль!" << endl; break;
    case ParseError::SYNTAX_ERROR: cout << "Ошибка: Некорректное выражение!" << endl; break;
    case ParseError::UNCLOSED_BRACKETS: cout << "Ошибка: Незакрытые скобки в выражении!" << endl; break;
    case ParseError::INVALID_CHARACTERS: cout << "Ошибка: Некорректные символы в выражении!" << endl; break;
    case ParseError::EMPTY: cout << "Ошибка: Пустое выражение!" << endl; break;
    default: break;
    }
    SetColor(0); // Сброс цвета
}