#include "Caclulator.h"

int main() {
    Calculator calculator;
    string expression;
    cout << "Отрицательные числа должны быть заключены в скобки (кроме начала выражения)\n";
    cout << "Введите выражение: ";
    getline(cin, expression);
    calculator.printResult(expression);
    calculator.printStructure(expression);
    calculator.printRPN(expression);
}