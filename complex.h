//11011183 11/05/2025
//header file for the complex class, containing funnctions to get the magnitude and the argument 

#include <iostream>
#include <cmath>


#ifndef COMPLEX_H
#define COMPLEX_H

class Complex //simple class to hold complex numbers 
{
private:
  double real; //two double memmbers, have to separate to encorporate real/imaginary parts 
  double imag;
public:
  Complex() {}; //rule of 5
  Complex(double r, double i) : real(r), imag(i) {}
  Complex(const Complex &c) : real(c.real), imag(c.imag) {}
  Complex(Complex &&c) : real(std::move(c.real)), imag(std::move(c.imag)) {}
  Complex &operator=(const Complex &c)
  {
    if(this != &c)
    {
      real = c.real;
      imag = c.imag;
    }
    return *this;
  }
  Complex &operator=(Complex &&c)
  {
    if(this != &c)
    {
      std::swap(real, c.real);
      std::swap(imag, c.imag);
    }
    return *this;
  }
  ~Complex() {}
  void set_real(double r) {real = r;} //settters to allow for access to the private members 
  void set_imag(double i) {imag = i;}

  double get_real() const {return real;}
  double get_imag() const {return imag;}
  double get_magnitude() const {return std::sqrt(real*real + imag*imag);} //useful for magintude and phase of impedance
  double get_argument() const {return std::atan2(imag, real);}

  friend Complex operator+(const Complex &a, const Complex &b); //friend functions to allow for easier access to the private members
  friend Complex operator-(const Complex &p, const Complex &q);
  friend Complex operator*(const Complex &p, const Complex &q);
  friend Complex operator/(const Complex &p, const Complex &q); 
};




#endif