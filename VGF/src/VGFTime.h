#pragma once

#include <GLFW/glfw3.h>


namespace VGF
{
	class Timer
	{
	public:
		Timer(double interval) : interval(interval) {};
		Timer() {};
		~Timer() {}

		bool Tick(double deltaTime);
		double interval = 1.0f;

	private:
		double accumulated = 0.0f;
	};

	class Time
	{
	public:
		static void UpdateDeltatime();
		static double GetDeltaTime() { return _deltaTime; }
	private:
		static inline double _deltaTime = 0.0f;
		static inline double _lastFrame = 0.0f;
	};
}