#pragma once

#include <windows.h>
#include <xinput.h>

#pragma comment(lib, "xinput.lib")

class ControllerInput
{
	public:
	    explicit ControllerInput(const DWORD index = 0);

	    bool connect();

	    void checkButtonPress();

	    void run();

	private:
	    XINPUT_STATE m_currentState;
	    XINPUT_STATE m_previousState;
	    DWORD m_controllerIndex;

};