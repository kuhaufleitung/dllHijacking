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
        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
        {
            std::cout << "A Button Pressed!" << std::endl;
        }

        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_B))
        {
            std::cout << "B Button Pressed!" << std::endl;
        }

        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_X) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_X))
        {
            std::cout << "X Button Pressed!" << std::endl;
        }

        if ((m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) &&
            !(m_previousState.Gamepad.wButtons & XINPUT_GAMEPAD_Y))
        {
            std::cout << "Y Button Pressed!" << std::endl;
        }

        SHORT leftThumbX = m_currentState.Gamepad.sThumbLX;
        SHORT leftThumbY = m_currentState.Gamepad.sThumbLY;

        const SHORT DEADZONE = 7849;

        float normalizedX = (abs(leftThumbX) < DEADZONE) ? 0.0f :
            leftThumbX / (leftThumbX > 0 ? 32767.0f : 32768.0f);
        float normalizedY = (abs(leftThumbY) < DEADZONE) ? 0.0f :
            leftThumbY / (leftThumbY > 0 ? 32767.0f : 32768.0f);

        if (abs(leftThumbX) >= DEADZONE || abs(leftThumbY) >= DEADZONE)
        {
            std::cout << "Left Thumbstick - X: " << normalizedX
                << ", Y: " << normalizedY << std::endl;
        }

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

    std::cout << "Controller connected." << std::endl;

    while (true)
    {
        checkButtonPress();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}