#include "fraction.h"
#include <exception>
#include <cmath>

int getLCM(int lhs, int rhs){
    int i;
    if(lhs > rhs){
        for(i = lhs; i > 0; i += lhs){
            if(i % rhs == 0){
                return i;
            }
        }
    }
    else if(lhs < rhs){
        for(i = rhs; i > 0; i+= rhs){
            if(i % lhs == 0){
                return i;
            }
        }
    }
    else{
        return lhs;
    }
    throw std::overflow_error("Out of range");
}

int getGCD(int lhs, int rhs){
    int i;
    if(lhs > rhs){
        for(i = rhs; i > 0; i--){
            if(rhs % i == 0 && lhs % i == 0){
                return i;
            }
        }
    }
    else if(lhs < rhs){
        for(i = lhs; i > 0; i--){
            if(lhs % i == 0 && rhs % i == 0){
                return i;
            }
        }
    }
    else{
        return lhs;
    }
}

void Fraction::reduction(){
    int temp = getGCD(denominator, abs(numerator));
    numerator /= temp;
    denominator /= temp;
}

bool operator==(Fraction lhs, Fraction rhs){
    lhs.reduction();
    rhs.reduction();
    if(lhs.denominator == rhs.denominator && lhs.numerator == rhs.numerator){
        return true;
    }
    else{
        return false;
    }
}

bool operator>(Fraction lhs, Fraction rhs){
    if(lhs.numerator < 0 && rhs.numerator > 0){
        return false;
    }
    if(lhs.numerator > 0 && rhs.numerator < 0){
        return true;
    }
    Fraction temp = lhs / rhs;
    if(temp.denominator < temp.numerator){
        return true;
    }
    else{
        return false;
    }
}

bool operator<(Fraction lhs, Fraction rhs){
    return (rhs > lhs);
}

bool operator>=(Fraction lhs, Fraction rhs){
    return !(lhs < rhs);
}

bool operator<=(Fraction lhs, Fraction rhs){
    return !(lhs > rhs);
}

Fraction operator+(Fraction lhs, Fraction rhs){
    Fraction result;
    lhs.reduction();
    rhs.reduction();
    result.denominator = getLCM(lhs.denominator, rhs.denominator);
    result.numerator = result.denominator / lhs.denominator * lhs.numerator + 
        result.denominator / rhs.denominator * rhs.numerator;
    result.reduction();
    return result;
}

Fraction operator-(Fraction lhs, Fraction rhs){
    return lhs + (-rhs);
}

Fraction operator*(Fraction lhs, Fraction rhs){
    lhs.reduction();
    rhs.reduction();
    Fraction result;
    result.denominator = lhs.denominator * rhs.denominator;
    result.numerator = lhs.numerator * rhs.numerator;
    result.reduction();
    return result;
}

Fraction operator/(Fraction lhs, Fraction rhs){
    if(rhs.numerator == 0){
        throw std::invalid_argument("The divisor can not be zero");
    }
    return lhs * rhs.reciprocal();
} 

Fraction Fraction::reciprocal(){
    Fraction result;
    if(numerator > 0){
        result.numerator = denominator;
        result.denominator = numerator;
    }
    else{
        result.numerator = -denominator;
        result.denominator = -numerator;
    }
    return result;
}

Fraction Fraction::operator+(){
    Fraction result = *this;
    result.numerator = abs(result.numerator);
    return result;
}

Fraction Fraction::operator-(){
    Fraction result = *this;
    result.numerator = -result.numerator;
    return result;
}

std::ostream& operator<<(std::ostream &os, const Fraction &f){
    os << "Output the fraction: ";
    if(f.numerator == 0 || f.denominator == 1){
        os << f.numerator << std::endl;
    }
    else{
        os << f.numerator << '/' << f.denominator << std::endl;
    }
    return os;
}

std::istream& operator>>(std::istream &is, Fraction &f){
    std::cout << "Input the numerator: ";
    is >> f.numerator;
    std::cout << "Input the denominator: ";
    is >> f.denominator;
    if(f.denominator == 0){
        throw std::invalid_argument("The denominator can not be zero");
    }
    return is;
}
