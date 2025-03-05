#pragma once
#include <mutex>

#include "NumberDisplay.h"
#include <thread>

class GameTimer : public NumberDisplay
{
public:
	GameTimer();

	void Draw(SDL_Surface* DrawSurface) override;
	void UpdateNumber(unsigned int NewNumber) override;
	void ClearResources() override;

	void StartTimer();
	int StopTimer();
	void RunTimer();

private:
	std::thread TimerThread;
	std::atomic<bool> bIsRunning;
	std::atomic<int> CurrentTime;
	std::mutex Mutex;
	std::condition_variable CV;
};
