#include "VGFTime.h"

namespace VGF
{
	bool Timer::Tick(double deltaTime)
	{
		accumulated += deltaTime;
		if (accumulated >= interval)
		{
			accumulated -= interval;
			return true;
		}

		return false;
	}

	void Time::UpdateDeltatime()
	{
		double current_frame = glfwGetTime();
		_deltaTime = current_frame - _lastFrame;
		_lastFrame = current_frame;
	}
}
