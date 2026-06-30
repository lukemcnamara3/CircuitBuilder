//11011183 11/05/2025
//code for the derived inductor class, inherited from the component class

#include "component.h"

#ifndef INDUCTOR_H
#define INDUCTOR_H

class Inductor : public Component
{
private:
  double inductance;
  static int count;
public:
  Inductor() {count++;} //count is increased in every new instance of a particular component, to keep track of them and avoid confusion
  Inductor(std::string t, Complex imp, double v, double ir, double ic, double ii, double l) : Component(t, imp, v, ir, ic, ii), inductance(l) {count++;}
  Inductor(const Inductor &i) : Component(i), inductance(i.inductance) {}
  Inductor(Inductor &&i) : Component(std::move(i)), inductance(std::move(i.inductance)) {}

  Inductor &operator=(const Inductor &i)
  {
    if(this != &i)
    {
      Component::operator=(i);
      inductance = i.inductance;
    }
    return *this;
  }
  Inductor &operator=(Inductor &&i)
  {
    if(this != &i)
    {
      Component::operator=(std::move(i));
      std::swap(inductance, i.inductance);
    }
    return *this;
  }
  ~Inductor() {}

  void set_inductance() 
  {
    double l = get_number<double>("Please enter inductance (H): "); 
    if(l == 0) 
    {
      std::cout<<"Inductance cannot be 0, setting to defualt value (1 H)"<<std::endl;
      inductance = 1; 
    }
    inductance = l;
    
    
  }

  static void set_count(int c) {count = c;}
  int get_count() {return count;}

  void set_impedance()
  {
    impedance.set_real(internal_resistance);
    if(internal_capacitance == 0)
    {
      impedance.set_imag(2*M_PI*frequency*inductance);
    }
    else 
    {
      impedance.set_imag(2*M_PI*frequency*inductance - 1/(2*M_PI*frequency*internal_capacitance));
    }
    
  }
  double get_inductance() const {return inductance;}

  void print_data()
  {
    std::cout.precision(3);
    Component::print_data();
    std::cout<<"Inductance: "<<inductance<< "H"<<std::endl;
  }

  void ideal_inductor()
  {
    char answer = two_options("Is this an ideal inductor? (y/n)", 'y', 'n');
    internal_inductance = 0;
    if(answer == 'y')
    {
      internal_capacitance = 0;
      internal_resistance = 0;
    }
    else 
    {
      set_internal_capacitance();
      set_internal_resistance(); 
    }
  }

  void initialise_inductor() 
  {
    set_name("Unamed Inductor");
    ideal_inductor();
    set_inductance();
    set_impedance();
    
    
  }

};

#endif 