//11011183 11/05/2025
//implementation of the component class, its functions and the two_options function, which gives the user a choice between two options

#include "component.h"

//two_options is a function used for yes/no type questions, and is used later for choosing between series and parallel options
char two_options(std::string question, char option1, char option2) 
{
  while(true)
  {
    char input3;
    std::cout<<question<<std::endl;
    std::cin>>input3;

    if(input3 != option1 && input3 != option2)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    else if(std::cin.fail() || std::cin.peek() != '\n') //fails if input not valid or if there are any spaces, and then removes input
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    else
    {
      return input3; //returns character to be used in if conditions 
    }
  }
}


double Component::frequency = 0;