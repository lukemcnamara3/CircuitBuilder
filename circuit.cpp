//11011183 11/05/2025
//implementation of the circuit class, its derived classes and its functions 
//contains the key functions for creating a circuit and adding components to it
//also contains the code for the user interface

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "circuit.h"


double Circuit::source_frequency = 0; //initialisation of the static variables 
double Circuit::source_voltage = 0;
int SeriesPath::count = 0;
int ParallelPath::count = 0;
std::string SeriesPath::type = "SeriesPath";
std::string ParallelPath::type = "ParallelPath";


void circuit_statics()
{
  double f = get_number<double>("Please enter the source frequency (Hz): "); 
  Circuit::set_source_frequency(f);
  Component::set_frequency(f); //static source frequency set for the circuit and all components 

  double v = get_number<double>("Please enter the source voltage (V): ");
  if(v == 0)
  {
    std::cout<<"Source voltage cannot be 0, setting to default value (1 V)"<<std::endl;
    v = 1; 
  }
  Circuit::set_source_voltage(v); //static source voltage set for the circuit
  std::cout<<std::endl;
}


void first_component(std::shared_ptr<Circuit> circuit) //adds the first component to the circuit 
{
  std::cout<<"It is now time to add the first components to the circuit"<<std::endl;
  try
  {
    if(circuit->get_library().empty()) //throws error if there are no components in the library
    {
      throw std::runtime_error("Error: no components in the library");
    }
    std::pair<std::string, std::shared_ptr<Component>> first_component = circuit->select_from_library();
    std::shared_ptr<SeriesPath> s1 = std::make_shared<SeriesPath>(); //any path implemented as a shared pointer to manage memory
    s1->add_to_library(first_component);
    s1->total_impedance(); 
    add_to_configuration(circuit, s1);
    circuit->total_impedance(); //circuit impedance is set at each intermediate step of placing components
    circuit->voltage(); 
    
    std::cout<<"First component added to the circuit"<<std::endl;
    std::cout<<std::endl;

  }
  catch(const std::exception& e)
  {
    std::cerr<<e.what()<<'\n';
  }
  
}

void Circuit::voltage() 
{
  double circuit_impedance = impedance.get_magnitude();
  if(circuit_impedance == 0) 
  {
    return; //prevent division by zero 
  }
  for(auto &p : configuration)
  {
    auto &path = p.second;
    double path_impedance = path->get_impedance().get_magnitude();
    double path_voltage = (path_impedance / circuit_impedance) * source_voltage; //voltage divider rule
    path->set_path_voltage(path_voltage);  
    path->voltage(); //recursively calculate voltage for sub-paths
  }
}

void SeriesPath::voltage()
{
  double path_impedance = impedance.get_magnitude();
  if(path_impedance == 0) 
  {
    return; 
  }
  for(auto &c : component_library)
  {
    auto &component = c.second;
    double component_impedance = component->get_impedance().get_magnitude();
    double component_voltage = (component_impedance / path_impedance) * path_voltage; //voltage divider rule
    component->set_voltage(component_voltage); 
  }
  for(auto &p : configuration)
  {
    auto &subcircuit = p.second;
    double subcircuit_impedance = subcircuit->get_impedance().get_magnitude();
    double subcircuit_voltage = (subcircuit_impedance / path_impedance) * path_voltage; 
    subcircuit->set_path_voltage(subcircuit_voltage); 
    subcircuit->voltage(); //recursively calculate voltage for sub-paths
  }
}

void ParallelPath::voltage()
{
  for(auto &c : component_library)
  {
     c.second->set_voltage(path_voltage); //all components in parallel share the same voltage
  }

  for(auto &subcircuit : configuration)
  {
    auto &p = subcircuit.second;
    p->set_path_voltage(path_voltage); //all sub-paths in parallel share the same voltage
    p->voltage(); //recursively calculate voltage for sub-paths
  }
}

//adds a component to the whole circuit in series or parallel
void add_to_whole_circuit_in_series(std::shared_ptr<Circuit> circuit, std::pair<std::string, std::shared_ptr<Component>> &selected_component)
{
  
  bool component_added = false; //bool used to track whether the component has been added 
  for(const auto &c : circuit->get_configuration())
  {
    if(c.second->get_type() == "SeriesPath" && c.second->get_configuration().empty())
    {
      c.second->add_to_library(selected_component); //if there is a series path with no configuration, add the component to its library
      c.second->total_impedance();
      std::cout<<"Component added to the circuit in series"<<std::endl;
      component_added = true;
      break; //break out of the loop if the component is added 
    }
  }
  if(!component_added)
  {
    std::shared_ptr<SeriesPath> s = std::make_shared<SeriesPath> (); //create a new series path if there are subcircuits in all the others
    s->add_to_library(selected_component);
    s->total_impedance();
    add_to_configuration(circuit, s);
    std::cout<<"Component added to the circuit in series"<<std::endl;
  }
  circuit->total_impedance(); //impedance calculated at each intermediate step
  circuit->voltage(); 
  std::cout<<std::endl;
}


void add_across_component(std::shared_ptr<Circuit> circuit, std::pair<std::string, std::shared_ptr<Component>> &selected_component)
{
  std::cout<<"Now you need to decide which component you will be connecting the selected component across"<<std::endl;
  
  std::string target_component; //component to be connected across
  std::pair<std::string, std::shared_ptr<Circuit>> target; //path/subcircuit where the component is connected 
  std::unordered_set<std::shared_ptr<Circuit>> visited; 
  
  while(true)
  {
    try
    {
      std::cin>>target_component;
      target = circuit->search_component(target_component, visited);
      if(target.second == nullptr)
      {
        throw std::runtime_error("Error: component not found"); //deal with invalid inputs 
      }
      break;
    }
    catch(const std::exception& e)
    {
      std::cerr<<e.what()<<'\n';
      std::cout<<"Please enter a valid component name"<<std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //clear invalid inputs 
      continue;
    }
    
  }

  std::cout<<"You have selected: "<<target_component<<std::endl;
  char answer = two_options("Would you like to add this component in series or parallel? (s/p)", 's', 'p');
  if(answer == 's')
  {
    if(target.second->get_type() == "SeriesPath")
    {
      target.second->add_to_library(selected_component); //add to the library of the path if the component is already in a series path 
      target.second->total_impedance();
      std::cout<<"Selected component added to "<<target_component<<" in series"<<std::endl;
    }
    else
    {
      std::shared_ptr<SeriesPath> s = std::make_shared<SeriesPath> (); //if the component is in a parallel path, create a series path and move the components inside 
      s->add_to_library(selected_component);
      auto iterated_component = target.second->get_library().find(target_component);
      std::pair<std::string, std::shared_ptr<Component>> old_component = *iterated_component;
      target.second->get_library().erase(iterated_component); //remove component from the original library so it can be moved 
      s->add_to_library(old_component);
      s->total_impedance(); //impedance calculated at each intermediate step
      add_to_configuration(target.second, s);
      target.second->total_impedance();
    }
  }
  else
  {
    if(target.second->get_type() == "ParallelPath") //add to library of path if component is already in parallel
    {
      target.second->add_to_library(selected_component);
      target.second->total_impedance();
      std::cout<<"Selected component added to "<<target_component<<" in parallel"<<std::endl;
    }
    else
    {
      std::shared_ptr<ParallelPath> p = std::make_shared<ParallelPath> (); //same as before but for parallel paths 
      p->add_to_library(selected_component);
      auto iterated_component = target.second->get_library().find(target_component); //auto used because of the fact that the components are stored in maps
      std::pair<std::string, std::shared_ptr<Component>> old_component = *iterated_component;
      target.second->get_library().erase(iterated_component);
      p->add_to_library(old_component);
      p->total_impedance();
      add_to_configuration(target.second, p);
      target.second->total_impedance();
    }
  }
  circuit->total_impedance();
  circuit->voltage();
  std::cout<<std::endl;
}

void print_component_info(std::shared_ptr<Circuit> circuit) //finds a component and prints the data for it 
{
  std::cout<<"Please enter the name of the component you would like to see the data for "<<std::endl;
  std::string component;
  std::cin>>component;
  std::unordered_set<std::shared_ptr<Circuit>> visited; //set used to track the paths that have been visited

  std::pair<std::string, std::shared_ptr<Circuit>> target_path = circuit->search_component(component, visited);
  if(target_path.second == nullptr)
  {
    std::cout<<"Error: component not found"<<std::endl;
    return; //checks if component is connected 
  }
  std::cout<<"You have selected: "<<component<<std::endl;
  auto iterator_component = target_path.second->get_library().find(component);
  if(iterator_component == target_path.second->get_library().end())
  {
    std::cout<<"Error: component not found"<<std::endl;
    return; 
    //run circuit will just give the option again to print info so no need for error handling
  }
  iterator_component->second->print_data();
  std::cout<<std::endl;
}

void run_circuit(std::shared_ptr<Circuit> circuit) //main function which runs the circuit and provides the user interface
{
  std::cout<<std::endl;
  std::cout<<"Welcome to the circuit simulator"<<std::endl;
  std::cout<<std::endl;
  circuit_statics();
  std::cout<<"Please start by creating the component library"<<std::endl;
  circuit->set_library();
  std::cout<<"Library created"<<std::endl;
  first_component(circuit); //frequency and library set, then first component is added to begin 
  
  
  while(true) //loop keeps the program running until the user exits 
  {
    try
    {
      if(circuit->get_library().empty()) //options if there are no more components in the library 
      {
        std::cout<<"Error: no more components in the library"<<std::endl;
        std::cout<<"Here are your options: "<<std::endl;
        std::cout<<"1. Print the data for the circuit"<<std::endl;
        std::cout<<"2. Print the data for a specific component"<<std::endl;
        std::cout<<"3. Add more components to the circuit"<<std::endl;
        std::cout<<"4. Exit"<<std::endl; //exits the function if this option is selected 
        std::cout<<std::endl;

        int answer = get_number<int>("Please enter your choice: "); //robustness of get_number used to also generate the menu 
        if(answer == 1)
        {
          circuit->print_data();
          continue; //continue the loop at each intermediate step unless an exit is called 
        }
        else if(answer == 2)
        {
          print_component_info(circuit);
          continue;
        }
        else if(answer == 3)
        {
          circuit->set_library();
          continue;
        }
        else if(answer == 4)
        {
          std::cout<<"Exiting the program"<<std::endl;
          std::cout<<std::endl;
          break;
        }
        else
        {
          std::cout<<"Error: invalid choice"<<std::endl;
          std::cout<<std::endl;
          continue; //handles invalid numerical inputs 
        }
        
      }
      else
      {
        std::cout<<"Here are your options: "<<std::endl;
        std::cout<<"1. Print the data for the circuit"<<std::endl;
        std::cout<<"2. Print the data for a specific component"<<std::endl;
        std::cout<<"3. Print the data from the component library"<<std::endl;
        std::cout<<"4. Add a component to the whole circuit in series"<<std::endl;
        std::cout<<"5. Add a component across another component"<<std::endl; //options 4 and 5 only appear since there are components in the circuit library
        std::cout<<"6. Exit"<<std::endl;
        std::cout<<std::endl;

        int answer = get_number<int>("Please enter your choice: ");
        if(answer == 1)
        {
          circuit->print_data();
          continue;
        }
        else if(answer == 2)
        {
          print_component_info(circuit);
          continue;
        }
        else if(answer == 3)
        {
          circuit->print_library();
          continue;
        }
        else if(answer == 4)
        {
          std::pair<std::string, std::shared_ptr<Component>> component = circuit->select_from_library();
          add_to_whole_circuit_in_series(circuit, component);
          continue;
        }
        else if(answer == 5)
        {
          std::pair<std::string, std::shared_ptr<Component>> component = circuit->select_from_library();
          add_across_component(circuit, component);
          continue;
        }
        else if(answer ==6)
        {
          std::cout<<"Exiting the program"<<std::endl;
          std::cout<<std::endl;
          break; //only way to exit the program is to select this options
        }
        else
        {
          std::cout<<"Error: invalid choice"<<std::endl;
          continue;
        }
        
      }
    }
    catch(const std::exception& e) //catch any thrown errors from the functions
    {
      std::cerr<<e.what()<<'\n';
    }
    
  }
}



  