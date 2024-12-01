#include <exception>
#include <iostream>

#include "ControllerInput.hpp"

int main()
{
    try
    {
        ControllerInput controller;
        controller.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}