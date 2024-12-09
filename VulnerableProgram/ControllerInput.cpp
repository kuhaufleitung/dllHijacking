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
    m_previousState = m_currentState;

    if (XInputGetState(m_controllerIndex, &m_currentState) == ERROR_SUCCESS)
    {
        // A Button - Detect on press (transition from not pressed to pressed)
        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
        {
            std::cout << "A Button Pressed!" << std::endl;
        }

        // B Button
        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_B))
        {
            std::cout << "B Button Pressed!" << std::endl;
        }

        // X Button
        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_X) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_X))
        {
            std::cout << "X Button Pressed!" << std::endl;
        }

        // Y Button
        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_Y))
        {
            std::cout << "Y Button Pressed!" << std::endl;
        }

        // Left Thumbstick Coordinates
        SHORT leftThumbX = m_currentState.Gamepad.sThumbLX;
        SHORT leftThumbY = m_currentState.Gamepad.sThumbLY;

        // Apply deadzone to prevent drift
        const SHORT DEADZONE = 7849;

        // Normalize coordinates
        float normalizedX = (abs(leftThumbX) < DEADZONE) ? 0.0f :
            leftThumbX / (leftThumbX > 0 ? 32767.0f : 32768.0f);
        float normalizedY = (abs(leftThumbY) < DEADZONE) ? 0.0f :
            leftThumbY / (leftThumbY > 0 ? 32767.0f : 32768.0f);

        // Print joystick coordinates if outside deadzone
        if (abs(leftThumbX) >= DEADZONE || abs(leftThumbY) >= DEADZONE)
        {
            std::cout << "Left Thumbstick - X: " << normalizedX
                << ", Y: " << normalizedY << std::endl;
        }

        // Update previous state for next frame
        m_previousState = m_currentState;
    }
}

void ControllerInput::run()
{
    if (!connect())
    {
        std::cout << "No controller connected." << std::endl;
        return;
    }

    std::cout << "Controller connected. Press A button to see output." << std::endl;

    while (true)
    {
        checkButtonPress();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}