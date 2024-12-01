#include "ControllerInput.hpp"

#include <iostream>
#include <thread>

ControllerInput::ControllerInput(const DWORD index) : m_controllerIndex(index)
{
    ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
    ZeroMemory(&m_previousState, sizeof(XINPUT_STATE));
}

bool ControllerInput::connect()
{
    ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));

    DWORD result = XInputGetState(m_controllerIndex, &m_currentState);

    return (result == ERROR_SUCCESS);
}

void ControllerInput::checkButtonPress()
{
    // Store previous state
    m_previousState = m_currentState;

    // Get current state
    if (XInputGetState(m_controllerIndex, &m_currentState) == ERROR_SUCCESS)
    {
        // Check for A button press (on Xbox 360 controller)
        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
        {
            std::cout << "A Button Pressed!" << std::endl;
        }
    }
}

void ControllerInput::run()
{
    // Check if controller is connected
    if (!connect())
    {
        std::cout << "No controller connected." << std::endl;
        return;
    }

    std::cout << "Controller connected. Press A button to see output." << std::endl;

    // Main input loop
    while (true)
    {
        checkButtonPress();

        // Small delay to prevent high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}