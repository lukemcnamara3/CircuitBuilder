//11011183 11/05/2025
//code for the circuit class and paths, defines all the functions for searching for components and dding them to the library

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <map>
#include <memory>
#include <utility>
#include <unordered_set>

#include "component.h"
#include "inductor.h"
#include "capacitor.h"
#include "resistor.h"

class SeriesPath; //foward declaration of classes to avoid circular dependencies 
class ParallelPath;

class Circuit : public std::enable_shared_from_this<Circuit>
{
protected:
  static double source_frequency; //frequency again must be a static variable as it is the same for all components 
  std::map<std::string, std::shared_ptr<Component>> component_library; //library of components stored as a map to allow easier access and finding of components
  Complex impedance;
  std::map<std::string, std::shared_ptr<Circuit>> configuration; //configuration also stored as a map to allow for easy access to components
  static double source_voltage;
public:
  Circuit() {}; //rule of 5 also used for circuit class just in case 
  Circuit(std::map<std::string, std::shared_ptr<Component>> cl, Complex imp, std::map<std::string, std::shared_ptr<Circuit>> conf) 
    : component_library(cl), impedance(imp), configuration(conf) {};
  Circuit(const Circuit &c) 
    : component_library(c.component_library), impedance(c.impedance), configuration(c.configuration) {};
  Circuit(Circuit &&c) 
    : component_library(std::move(c.component_library)), impedance(std::move(c.impedance)), configuration(std::move(c.configuration)) {};
  Circuit &operator=(const Circuit &c)
  {
    if(this != &c)
    {
      component_library = c.component_library;
      impedance = c.impedance;
      configuration = c.configuration;
    }
    return *this;
  }
  Circuit &operator=(Circuit &&c)
  {
    if(this != &c)
    {
      std::swap(component_library, c.component_library);
      std::swap(impedance, c.impedance);
      std::swap(configuration, c.configuration);
    }
    return *this;
  }
  virtual ~Circuit() {}; //virtual destructor for polymorphism 

  static void set_source_frequency(double sf) {source_frequency = sf;}
  static void set_source_voltage(double sv) {source_voltage = sv;} //static function to set the source voltage for the circuit

  void set_library() //function designed to set the library of components which will be added in the circuit 
  {
    int resistors = get_number<int>("How many resistors do you want to add to the library?");
    std::cout<<std::endl;
    int capacitors = get_number<int>("How many capacitors do you want to add to the library?");
    std::cout<<std::endl;
    int inductors = get_number<int>("How many inductors do you want to add to the library?");
    std::cout<<std::endl;
    
    for(int x = 1; x < resistors + 1; x++)
    {
      Resistor r{};
      std::string new_name = "Resistor" + std::to_string(r.get_count()); //static counter keeps track of the different compnents
      std::cout<<"Choosing data for: "<<new_name<<std::endl;
      
      r.initialise_resistor(); //each iteration adds a new resistor to the library
      r.set_name(new_name);
      component_library[new_name] = std::make_shared<Resistor>(r);
      std::cout<<std::endl;
    }
    for(int y = 1; y < capacitors + 1; y++)
    {
      Capacitor c{}; //same process for capacitors and inductors 
      std::string new_name = "Capacitor" + std::to_string(c.get_count());
      std::cout<<"Choosing data for: "<<new_name<<std::endl;
      
      c.initialise_capacitor();
      c.set_name(new_name);
      component_library[new_name] = std::make_shared<Capacitor>(c);
      std::cout<<std::endl;
    }
    for(int z = 1; z < inductors + 1; z++)
    {
      Inductor i{};
      std::string new_name = "Inductor" + std::to_string(i.get_count());
      std::cout<<"Choosing data for: "<<new_name<<std::endl;
      
      i.initialise_inductor();
      i.set_name(new_name);
      component_library[new_name] = std::make_shared<Inductor>(i);
      std::cout<<std::endl;
    }
  }

  virtual std::map<std::string, std::shared_ptr<Component>>& get_library() {return component_library;}; 
  //referenced as there is a need to alter the library in the functions

  static double get_source_voltage() {return source_voltage;};
  static double get_source_frequency() {return source_frequency;};
  virtual void total_impedance() //adds up the impedance of the configuration (paths) in series
  {
    Complex total_impedance{0,0};
    for(auto &c : configuration) //only takes impedance from the configuration as the library is purely for storing components 
    {
      c.second->total_impedance(); 
      total_impedance = total_impedance + c.second->get_impedance();
    }
    impedance = total_impedance;
  };
  virtual Complex get_impedance() const {return impedance;}; 
  virtual std::map<std::string, std::shared_ptr<Circuit>>& get_configuration() {return configuration;};
  //again referenced as there is a need to alter the configuration

  virtual void set_path_voltage(double v) {}

  virtual void add_to_library(std::pair<std::string, std::shared_ptr<Component>> &selected_component)
  {
    std::string component_name = selected_component.first;
    std::shared_ptr<Component> component = selected_component.second;
    component_library[component_name] = component;
  } //virtual function to add components to the library as the paths will use this as well

  virtual void print_data() //simple function will prinr data for the circuit 
  {
    std::cout.precision(3);
    std::cout<<"Circuit data: "<<std::endl;
    std::cout<<"Impedance: "<<impedance.get_real()<<" + "<<impedance.get_imag()<<"i"<<std::endl;
    std::cout<<"Magnitude: "<<impedance.get_magnitude()<<" Ohms"<<std::endl;
    std::cout<<"Frequency: "<<source_frequency<<" Hz"<<std::endl;
    std::cout<<std::endl;

    std::cout<<"Configuration: "<<std::endl;
    for(const auto& c : configuration) //auto keyword used to accomodate the complexity of the map
    {
      std::cout<<c.first<<std::endl;
    }
    std::cout<<std::endl;
  }

  virtual void print_library() //prints the library of components for any circuit path
  {
    try
    {
      if(component_library.empty()) //throws an error if the library is empty
      {
        throw std::runtime_error("Error: library is empty");
      }
      std::cout << "Library contents:" << std::endl;
      std::cout<<std::endl;
      for(const auto& c : component_library) //iterates through the library printing component data
      {
        c.second->set_voltage(0);
        std::cout<<c.first<<std::endl;
        c.second->print_data();
        std::cout<<std::endl;
      }
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
    
  }

  virtual std::pair<std::string, std::shared_ptr<Component>> select_from_library() //selects and removes component from the library
  {
    std::string component_name;

    while (true) 
    { 
      std::cout << "Please select a component from the circuit library: " << std::endl;
      std::cin >> component_name;

      try 
      {
        auto ptr = component_library.find(component_name); //using auto keyword to select the component
        if (ptr == component_library.end()) 
        {
          throw std::string("Error: component not found");
        } 
        else 
        {
          std::pair<std::string, std::shared_ptr<Component>> selected_component = *ptr;
          std::cout << "You selected: " << selected_component.first << std::endl;
          component_library.erase(selected_component.first); //erasing component from library
          return selected_component; 
        }
      } 
      catch (const std::string &e) 
      {
      std::cout << e << std::endl;
      }
    }
  }


  std::pair<std::string, std::shared_ptr<Circuit>> search_component(const std::string& name, std::unordered_set<std::shared_ptr<Circuit>>& visited) 
  {
        
    if (visited.count(shared_from_this())) //prevent infinite recursion: skip already visited circuits
    {
      return {"", nullptr};
    }
    visited.insert(shared_from_this()); //mark this circuit as visited
  
    auto it = component_library.find(name); //check the current circuit's library for the component
    if (it != component_library.end()) 
    {
      return {"Current Circuit", shared_from_this()}; //component found in the current circuit
    }

    for (const auto& c : configuration) // Recursively search in sub-circuits
    {
      if (!c.second) 
      {
        continue; 
      }
      auto result = c.second->search_component(name, visited); // Recursively search in the sub-circuit
      if (result.second) 
      { 
        return result;
      }
    }
    return {"", nullptr}; //return a null pointer if the component is not found in the circuit or any subcircuits
  }

  virtual std::string get_type() {return "Circuit";};
  virtual int get_count() {return 1;}; //virtual functions so that they can be overridden in the derived classes 
  virtual void voltage();
};


class SeriesPath : public Circuit
{
private:
  static int count; //similar use to how components are counted- makes it easier to keep track of the paths/ subcircuits
  static std::string type;
  double path_voltage;
public:
  SeriesPath() {count++;}; //count is incremeted each time a new path is created 
  SeriesPath(double v, std::map<std::string, std::shared_ptr<Component>> cl, Complex imp, std::map<std::string, std::shared_ptr<Circuit>> c) : Circuit(cl, imp, c), path_voltage(v) {count++;};
  SeriesPath(const SeriesPath &b) : Circuit(b) , path_voltage(b.path_voltage) {};
  SeriesPath(SeriesPath &&b) : Circuit(std::move(b)), path_voltage(std::move(b.path_voltage)) {};
  SeriesPath &operator=(const SeriesPath &b)
  {
    if(this != &b)
    {
      Circuit::operator=(b);
      path_voltage = b.path_voltage;
    }
    return *this;
  }
  SeriesPath &operator=(SeriesPath &&b)
  {
    if(this != &b)
    {
      Circuit::operator=(std::move(b));
      std::swap(path_voltage, b.path_voltage);
    }
    return *this;
  }
  ~SeriesPath() {};

  void total_impedance() //overidding the total impedance function for the series path
  {
    Complex total_impedance{0,0};

    //anything in the library is added in series to the impedance for simplicity and so that less SeriesPaths need to be created to represent nested circuits 
    for(auto &c : component_library)     
    {
      total_impedance = total_impedance + c.second->get_impedance();
    }
    
    for(auto &p : configuration)
    {
      p.second->total_impedance(); //total impedance is also called recursively to accomodate for nested circuits 
      total_impedance = total_impedance + p.second->get_impedance();
    }
    
    impedance = total_impedance;
  }

  void set_path_voltage(double v) {path_voltage = v;} 
  double get_path_voltage() {return path_voltage;}

  static void set_count(int c) {count = c;} 
  int get_count() {return count;}
  std::string get_type() {return type;}

  void voltage();

  
};

class ParallelPath : public Circuit //makes it easier to add components in parallel
{
private:  
  static int count; //similar usage to the SeriesPath class and Component derived classes 
  static std::string type;
  double path_voltage;
public:
  ParallelPath() {count++;};
  ParallelPath(double v, std::map<std::string, std::shared_ptr<Component>> cl, Complex imp, std::map<std::string, std::shared_ptr<Circuit>> c) : Circuit(cl, imp, c), path_voltage(v)  {count++;};
  ParallelPath(const ParallelPath &p) : Circuit(p), path_voltage(p.path_voltage) {};
  ParallelPath(ParallelPath &&p) : Circuit(std::move(p)), path_voltage(std::move(p.path_voltage)) {};
  ParallelPath &operator=(const ParallelPath &p)
  {
    if(this != &p)
      {
        Circuit::operator=(p);
        path_voltage = p.path_voltage;
      }
    return *this;
  }
  ParallelPath &operator=(ParallelPath &&p)
  {
    if(this != &p)
    {
      Circuit::operator=(std::move(p));
      std::swap(path_voltage, p.path_voltage);
    }
    return *this;
  }
  ~ParallelPath() {};
   
  static void set_count(int c) {count = c;}; //static function to set the count for the parallel path

  int get_count() {return count;};
  std::string get_type() {return type;};

  void set_path_voltage(double v) {path_voltage = v;} 
  double get_path_voltage() {return path_voltage;} 
  void total_impedance()    
  {
    Complex one{1,0}; //easier to define this complex number as there is a divide function in the Complex class 
    Complex inverse_sum{0,0};
    Complex total_impedance{0,0}; //these must both be defined as 0 to begin otherwise there will be errors in the calculations

    for(auto &c : component_library) //components in the library added in parallel 
    {
      if(c.second->get_impedance().get_real() == 0 && c.second->get_impedance().get_imag() == 0) {continue;} //to prevent division by zero
      inverse_sum = inverse_sum + one/c.second->get_impedance(); 
    }
    for(auto &p: configuration) //the subcircuits/ configuration are also added in parallel
    {
      p.second->total_impedance();
      if(p.second->get_impedance().get_real() == 0 && p.second->get_impedance().get_imag() == 0) {continue;}
      inverse_sum = inverse_sum + one/p.second->get_impedance(); 
    }

    if(inverse_sum.get_real() == 0 && inverse_sum.get_imag() == 0) //final checker to prevent division by zero 
    {
      total_impedance.set_real(0); total_impedance.set_imag(0);
    }
    else
    {
      total_impedance = one/inverse_sum; //final impedance is one over the sum of the inverses 
    }
  
    impedance = total_impedance;
  }
  void voltage();

};;



void circuit_frequency();
void first_component(std::shared_ptr<Circuit> circuit);
void add_to_whole_circuit(std::shared_ptr<Circuit> circuit, std::pair<std::string, std::shared_ptr<Component>> &selected_component);
void add_across_component(std::shared_ptr<Circuit> circuit, std::pair<std::string, std::shared_ptr<Component>> &selected_component);
void print_component_info(std::shared_ptr<Circuit> circuit);
void run_circuit(std::shared_ptr<Circuit> circuit);

template <typename T1, typename T2> //adding a path to any sub-circuit or circuit, has to be template to add paths within other paths 
void add_to_configuration(std::shared_ptr<T1> circuit, std::shared_ptr<T2> path)
{
  std::string name = path->get_type() + std::to_string(path->get_count());
  circuit->get_configuration().insert({name, path});
  std::cout<<"Path: "<<name<<" added to Circuit"<<std::endl;
}


#endif