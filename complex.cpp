//11011183 11/05/2025
//implementation of the complex class and its operators 

#include "complex.h"

Complex operator+(const Complex &a, const Complex &b) //useful for adding impedances in series
{
  return Complex(a.get_real() + b.get_real(), a.get_imag() + b.get_imag());
}
Complex operator-(const Complex &p, const Complex &q) //just in case 
{
  return Complex(p.get_real() - q.get_real(), p.get_imag() - q.get_imag());
}
Complex operator*(const Complex &p, const Complex &q)
{
  return Complex(p.get_real()*q.get_real() - p.get_imag()*q.get_imag(), p.get_real()*q.get_imag() + p.get_imag()*q.get_real());
}
Complex operator/(const Complex &p, const Complex &q) //useful for calculating impedance in parallel
{
  double denom = q.get_real()*q.get_real() + q.get_imag()*q.get_imag();
  return Complex((p.get_real()*q.get_real() + p.get_imag()*q.get_imag())/denom, (p.get_imag()*q.get_real() - p.get_real()*q.get_imag())/denom);
}