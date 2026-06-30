//11011183 11/05/2025
//code containing the abstract component class, its virtual functions and the template function for validating inputs

#include <string>


#ifndef COMPONENT_H
#define COMPONENT_H
#include "complex.h"

char two_options(std::string question, char option1, char option2); //forward declaration of function

//get_number template function validates numerical inputs and only returns positive values 
//required template as it is used on integers and doubles 
template <class c_type> c_type get_number(std::string question) 
{
  c_type input;
  while(true)
  {
    try //used here as there were two conditions which it needed to suffice and needed to distinguish between errors 
    {
      std::cout<<question<<std::endl;
      std::cin>>input;
      if(std::cin.fail() || std::cin.peek() != '\n')
      {
      std::cin.clear(); //clears and flags invalid input 
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      throw std::invalid_argument("Error: invalid input");
      }
      else if(input < 0)
      {
        throw std::invalid_argument("Error: value cannot be negative"); //other condition being the value needs to be positive
      }
      return input;
      break;
    }
    catch(const std::invalid_argument &e)
    {
      std::cout<<e.what()<<std::endl;
    }
  }
}

class Component //base class for all components 
{
protected:
  std::string name;
  static double frequency; //frequency must be static as it the same for all components 

  Complex impedance;
  double voltage;

  double internal_resistance; //to allow for non-ideal components and parasitic effects 
  double internal_capacitance;
  double internal_inductance;

public:
  Component() {} //contructors and assignment operators defined here, rule of 5 preserved throughout the project
  Component(std::string n, Complex imp, double v, double ir, double ic, double ii) : 
    name(n), impedance(imp), voltage(v),
    internal_resistance(ir), internal_capacitance(ic), internal_inductance(ii) {}
  Component(const Component &c) : name(c.name), impedance(c.impedance), voltage(c.voltage),
    internal_resistance(c.internal_resistance), internal_capacitance(c.internal_capacitance), internal_inductance(c.internal_inductance) {}
  Component(Component &&c) : name(std::move(c.name)), impedance(std::move(c.impedance)), voltage(std::move(c.voltage)),
    internal_resistance(std::move(c.internal_resistance)), internal_capacitance(std::move(c.internal_capacitance)), internal_inductance(std::move(c.internal_inductance)) {}
  Component &operator=(const Component &c)
  {
    if(this != &c)
    {
      name = c.name;
      impedance = c.impedance;
      voltage = c.voltage;
      internal_resistance = c.internal_resistance;
      internal_capacitance = c.internal_capacitance;
      internal_inductance = c.internal_inductance;
    }
    return *this;
  }
  Component &operator=(Component &&c)
  {
    if(this != &c)
    {
      std::swap(name, c.name);
      std::swap(impedance, c.impedance);
      std::swap(voltage, c.voltage);
      std::swap(internal_resistance, c.internal_resistance);
      std::swap(internal_capacitance, c.internal_capacitance);
      std::swap(internal_inductance, c.internal_inductance);
    }
    return *this;
  }
  virtual ~Component() {}

  virtual void set_name(std::string n) {name = n;}
  static void set_frequency(double f) {frequency = f;} //no need to validate this one as this will be set in the circuit class
  virtual void set_voltage(double v) {voltage = v;} //setter function for voltage

  
  virtual void set_impedance(){} //purely virtual function to be defined in derived classes 

  virtual void set_internal_resistance() //setter functions allow user input, as ideal-components will be optional
  {
    double ir = get_number<double>("Please enter internal resistance (Ohms): ");
    internal_resistance = ir;
  }
  virtual void set_internal_capacitance() 
  {
    double ic = get_number<double>("Please enter internal capacitance (F): ");
    internal_capacitance = ic;
  }
  virtual void set_internal_inductance() 
  {
    double ii = get_number<double>("Please enter internal inductance (H): ");
    internal_inductance = ii;
  }

  virtual std::string get_name() const {return name;}
  virtual int get_count() {return 0;} //not used in this class but needed for derived classes 
  static double get_frequency() {return frequency;}

  virtual Complex get_impedance() const {return impedance;}
  virtual double get_voltage() const {return voltage;}

  virtual double get_internal_resistance() const {return internal_resistance;}
  virtual double get_internal_capacitance() const {return internal_capacitance;}
  virtual double get_internal_inductance() const {return internal_inductance;} 

  virtual double get_magnitude_imp() const {return impedance.get_magnitude();}
  virtual double get_phase() const {return impedance.get_argument();} //using functions from complex class to get magnitude and phase of impedance

  virtual void print_data() //purely virtual functions- component is an abstract class 
  {
    std::cout.precision(3);
    std::cout<<"Name: "<<name<<std::endl;
    std::cout<<"Frequency: "<<frequency<<" Hz"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Phase: "<<get_phase()<<" Radians"<<std::endl;
    std::cout<<"Impedance: "<<impedance.get_real()<<" + "<<impedance.get_imag()<<"i"<<std::endl;
    std::cout<<"Magnitude Impedance: "<<impedance.get_magnitude()<<" Ohms"<<std::endl;
    std::cout<<"Voltage: "<<voltage<<" V"<<std::endl;
    std::cout<<"Current: "<<voltage/impedance.get_magnitude()<<" A"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Internal Resistance: "<<internal_resistance<<" Ohms"<<std::endl;
    std::cout<<"Internal Capacitance: "<<internal_capacitance<<" F"<<std::endl;
    std::cout<<"Internal Inductance: "<<internal_inductance<<" H"<<std::endl;
    std::cout<<std::endl;
  }

};







#endif