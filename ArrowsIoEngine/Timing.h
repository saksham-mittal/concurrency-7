#pragma once

#include <SDL\SDL.h>

namespace ArrowsIoEngine {

	class FpsLimiter {
	public:
		FpsLimiter();
		void init(float maxtFPS);

		void setMaxFPS(float maxFPS);

		void begin();

		float end();
	private:
		void calculateFPS();
		float _fps;
		float _maxFPS;
		float _frameTime;
		unsigned int _startTicks;
	};
}