#include <iostream>
#include "fraction.h"

using namespace std;

int main()
{
    Fraction a(0, 1);
    Fraction b(3, 6);
    Fraction c(-2, 5);
    Fraction d(7, 6);
    Fraction e(-6, 15);
    Fraction f;

    cout << "Variable 'a': " << a << endl;
    cout << "Variable 'b': " << b << endl;
    cout << "Variable 'c': " << c << endl;
    cout << "Variable 'd': " << d << endl;
    cout << "Variable 'e': " << e << endl;

    cout << "Reciprocal 'c': " << c.reciprocal() << endl;

    cout << "b > c ? " << (b > c) << endl;
    cout << "b <= d ? " << (b <= d) << endl;
    cout << "c == e ? " << (c == e) << endl;

    cout << "b * c = " << (b * c) << endl;
    cout << "d - e = " << (d - e) << endl;
    cout << "(-b * e) * 3 = " << (-b * e) * 3 << endl;
    cout << "(b + c) / (d + 1) - 2 = " << ((b + c) / (d + 1) - 2) << endl;

    cout << "Enter a fraction:" << endl;
    cin >> f;
    cout << "f + a = " << (f + a) << endl;

    return 0;
}

