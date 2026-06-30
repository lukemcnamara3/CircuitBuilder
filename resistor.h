//11011183 11/05/2025
//code for the derived resistor class, inherited from the component class 

#include "component.h"

#ifndef RESISTOR_H
#define RESISTOR_H

class Resistor : public Component
{
private:
  double resistance; //separate value for resistance as it must be set by the user/ easier to make ideal components 
  static int count; //static 'count' variable used to keep track of components/ paths and so there is no confusion in selecting them in a circuit
public:
  Resistor() {count++;}
  Resistor(std::string t, Complex imp, double v, double ir, double ic, double ii, double r) : Component(t, imp, v, ir, ic, ii), resistance(r) {count++;}
  Resistor(const Resistor &r) : Component(r), resistance(r.resistance) {}
  Resistor(Resistor &&r) : Component(std::move(r)), resistance(std::move(r.resistance)) {}

  Resistor &operator=(const Resistor &r)
  {
    if(this != &r)
    {
      Component::operator=(r);
      resistance = r.resistance;
    }
    return *this;
  }
  Resistor &operator=(Resistor &&r)
  {
    if(this != &r)
    {
      Component::operator=(std::move(r));
      std::swap(resistance, r.resistance);
    }
    return *this;
  }
  ~Resistor() {}

  void set_resistance() //use of get_number template function to validate input
  {
    double r = get_number<double>("Please enter resistance (Ohms): ");
    if(r == 0) //resistance cannot be 0
    {
      std::cout<<"Resistance cannot be 0, setting to defualt value (1 Ohm)"<<std::endl;
      resistance = 1; 
    }
    else
    resistance = r;
  }

  static void set_count(int c) {count = c;} //static function to set the count for the resistor
  int get_count() {return count;} //used for the circuit design later on to track components 
  
  double get_resistance() {return resistance;}

  void print_data()
  {
    std::cout.precision(3);
    Component::print_data();
    std::cout<<"Resistance: "<<resistance<<" Ohms"<<std::endl;
  }

  void set_impedance()
  {
    impedance.set_real(resistance);
    if(internal_capacitance == 0) //needed to avoid a division by zero error 
    {
      impedance.set_imag(2*M_PI*frequency*internal_inductance);
    }
    else 
    {
      impedance.set_imag(2*M_PI*frequency*internal_inductance - 1/(2*M_PI*frequency*internal_capacitance));
    } 

  }

  void ideal_resistor() //function similar for all components, allows user to choose between ideal and non-ideal components
  {
    char answer = two_options("Is this an ideal resistor? (y/n)", 'y', 'n');
    internal_resistance = 0;
    if(answer == 'y')
    {
      internal_capacitance = 0;
      internal_inductance = 0;
    }
    else 
    {
      set_internal_capacitance();
      set_internal_inductance(); 
    }
  }
  void initialise_resistor() //initialise function needed for putting components in different paths
  {
    set_name("Unamed Resistor");
    ideal_resistor();
    set_resistance();
    set_impedance();
    
  }


};

#endif