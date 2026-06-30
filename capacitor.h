//11011183 11/05/2025
//code for the derived capacitor class, inherited from the component class

#include "component.h"

#ifndef CAPACITOR_H
#define CAPACITOR_H

class Capacitor : public Component
{
private:
  double capacitance; 
  static int count; //count again used to track capacitors 
public:
  Capacitor() {count++;} //same rule of 5 continues here 
  Capacitor(std::string t, Complex imp, double v, double ir, double ic, double ii, double c) : Component(t, imp, v, ir, ic, ii), capacitance(c) {count++;}
  Capacitor(const Capacitor &c) : Component(c), capacitance(c.capacitance) {}
  Capacitor(Capacitor &&c) : Component(std::move(c)), capacitance(std::move(c.capacitance)) {}

  Capacitor &operator=(const Capacitor &c)
  {
    if(this != &c)
    {
      Component::operator=(c);
      capacitance = c.capacitance;
    }
    return *this;
  }
  Capacitor &operator=(Capacitor &&c)
  {
    if(this != &c)
    {
      Component::operator=(std::move(c));
      std::swap(capacitance, c.capacitance);
    }
    return *this;
  }
  ~Capacitor() {}

  void set_capacitance() 
  {
    double c = get_number<double>("Please enter capacitance (F): ");
    if(c == 0) //capacitance cannot be 0
    {
      std::cout<<"Capacitance cannot be 0, setting to defualt value (1 F)"<<std::endl;
      capacitance = 1; 
    }
    capacitance = c;
  }

  static void set_count(int c) {count = c;} 
  int get_count() {return count;}
  
  void set_impedance()
  {
    impedance.set_real(internal_resistance);
    if(capacitance == 0)
    {
      impedance.set_imag(2*M_PI*frequency*internal_inductance);
    }
    else 
    {
      impedance.set_imag(2*M_PI*frequency*internal_inductance - 1/(2*M_PI*frequency*capacitance));
    }
  }
  double get_capacitance() const {return capacitance;}

  void print_data()
  {
    std::cout.precision(3);
    Component::print_data();
    std::cout<<"Capacitance: "<<capacitance<<" F"<<std::endl;
  }

  void ideal_capacitor()
  {
    char answer = two_options("Is this an ideal capacitor? (y/n)", 'y', 'n');
    internal_capacitance = 0;
    if(answer == 'y')
    {
      internal_resistance = 0; //might change the parasitic effects as they only matter at high frequencies 
      internal_inductance = 0;
    }
    else 
    {
      set_internal_resistance();
      set_internal_inductance(); 
    }
  }

  void initialise_capacitor() 
  {
    set_name("Unamed Capacitor"); //setting name to a random string- this will be changed when the component is added to the library
    ideal_capacitor();
    set_capacitance();
    set_impedance();
    
  }
};

#endif