#pragma once

#include <GLFW/glfw3.h>


namespace VGF
{
	class Timer
	{
	public:
		Timer(double interval) : _interval(interval) {};
		~Timer() {}

		bool Tick(double deltaTime);

	private:
		double _interval = 0;
		double accumulated = 0.0f;
	};

	class Time
	{
	public:
		static void UpdateDeltatime();
		static const double GetDeltaTime() { return _deltaTime; }
	private:
		static inline double _deltaTime = 0.0f;
		static inline double _lastFrame = 0.0f;
	};
}