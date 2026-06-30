//11011183 11/05/2025
//main function for the code, makes use of the run circuit function and asks if the user wishes to repeat

#include "circuit.h"

int main()
{
  while(true)
  {
    std::shared_ptr<Circuit> circuit = std::make_shared<Circuit>(); //create a new circuit object
    run_circuit(circuit); //run the circuit function which contains the main loop of the program
    
    char answer = two_options("Would you like to run the program again? (y/n)", 'y', 'n'); //ask the user if they want to run the program again
    std::cout<<std::endl;
    if(answer == 'n') {break;} //exit the loop if the user does not want to run the program again
    else 
      {
        std::cout<<"Running the program again..."<<std::endl;
        std::cout<<std::endl;
        SeriesPath::set_count(0);
        ParallelPath::set_count(0); //reset the static counters for the paths
        Resistor::set_count(0);
        Capacitor::set_count(0);
        Inductor::set_count(0); //reset the static counters for the components
        continue;
      }

    
  }
  return 0;
}

//g++ -g -std=c++17 main.cpp circuit.cpp complex.cpp component.cpp inductor.cpp capacitor.cpp resistor.cpp -o circuit_program