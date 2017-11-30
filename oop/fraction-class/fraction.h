#include <iostream>
#include <exception>

int getLCM(int lhs, int rhs);
int getGCD(int lhs, int rhs);

class Fraction{
public:
    Fraction(){};
    Fraction(int n)
        :numerator(n), denominator(1){};
    Fraction(int n, int d)
        :numerator(n), denominator(d){
            if(d == 0){
                throw std::invalid_argument("The denominator can not be zero");
            }
        };
    Fraction reciprocal();
    
    friend bool operator==(Fraction lhs, Fraction rhs);
    friend bool operator>(Fraction lhs, Fraction rhs);
    friend bool operator<(Fraction lhs, Fraction rhs);
    friend bool operator>=(Fraction lhs, Fraction rhs);
    friend bool operator<=(Fraction lhs, Fraction rhs);

    friend Fraction operator+(Fraction lhs, Fraction rhs);
    friend Fraction operator-(Fraction lhs, Fraction rhs);
    friend Fraction operator*(Fraction lhs, Fraction rhs);
    friend Fraction operator/(Fraction lhs, Fraction rhs);

    Fraction operator+();
    Fraction operator-();

    friend std::ostream& operator<<(std::ostream &os, const Fraction &f);
    friend std::istream& operator>>(std::istream &is, Fraction &f);
private:
    int numerator;
    int denominator;

    void reduction();
};
