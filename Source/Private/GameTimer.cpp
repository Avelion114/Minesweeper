#include "GameTimer.h"
#include <ctime>
#include <iostream>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

GameTimer::GameTimer()
{
	MaxDigits = 4;
}

void GameTimer::UpdateNumber(unsigned int NewNumber)
{
	bProcessingUpdate = true;

	Digits.clear();
	unsigned int M = NewNumber / 60;
	unsigned int S = NewNumber % 60;
	

	//Add Seconds
	if(S < 10) //Add leading 0 if needed
	{
		Digits.push_back(S);
		Digits.push_back(0);
		
	}
	else
	{
		while(S > 9)
		{
			Digits.push_back(S % 10);
			S /= 10;
		}
		Digits.push_back(S);
	}
	
	//Add Minutes
	if(M < 10) //Add leading 0 if needed
	{
		Digits.push_back(M);
		Digits.push_back(0);
		
	}
	else
	{
		while(M > 9)
		{
			Digits.push_back(M % 10);
			M /= 10;
		}
		Digits.push_back(M);
	}

	
		
	std::reverse(Digits.begin(), Digits.end());

	bProcessingUpdate = false;
}

void GameTimer::Draw(SDL_Surface* DrawSurface)
{
	int OFFSET = (FontSize.Width() * 7) + 1; // Skip first 8 digits in font sheet. Need to add one pixel for some reason
	int ADD_SECONDS = 0; //Offset for post colon when drawing seconds
	
	for(int i = 0; i < Digits.size(); i++)
	{
		if(bProcessingUpdate){return;}//Exit loop if we've updated the number in the middle of drawing

	
		int Value = Digits[i];
		int x;
		int y;

		if(i == 2)//Draw Colon
		{
			y = 0;
			x = OFFSET + FontSize.Width();
			SDL_Rect SrcRect{x, y, FontSize.Width(), FontSize.Height()};
			SDL_Rect DestRect{Position.x + FontSize.Width() * i, Position.y, FontSize.Width(), FontSize.Height()};
			ADD_SECONDS = FontSize.Width();
			SDL_BlitSurface(SceneSurfaces[0],&SrcRect,DrawSurface, &DestRect);
		}

		if(Value < 9 && Value > 0)
		{
			y = FontSize.Height();
			x = OFFSET + Value * FontSize.Width();
		}
		else
		{
			y = FontSize.Height() * 2;
			x = Value == 0 ? OFFSET + FontSize.Width() * 2 : OFFSET + FontSize.Width(); //Handle 0 and 9 respectively
		}
		SDL_Rect SrcRect{x, y, FontSize.Width(), FontSize.Height()};
		SDL_Rect DestRect{Position.x + FontSize.Width() * i + ADD_SECONDS, Position.y, FontSize.Width(), FontSize.Height()};
		
		SDL_BlitSurface(SceneSurfaces[0],&SrcRect,DrawSurface, &DestRect);
	}
}

void GameTimer::ClearResources()
{
	if(bIsRunning.load())
	{
		StopTimer();
	}
	NumberDisplay::ClearResources();
}

void GameTimer::RunTimer()
{
	CurrentTime.store(0);
	while(bIsRunning.load())
	{
		std::unique_lock<std::mutex> Lock(Mutex);
		if(CV.wait_for(Lock, std::chrono::seconds(1), [this]{return !bIsRunning.load();}))
		{
			break;
		}
		CurrentTime.fetch_add(1); //Increment every 1 second
		UpdateNumber(CurrentTime.load());
	}
}

void GameTimer::StartTimer()
{
	if(!bIsRunning.load())
	{
		bIsRunning.store(true);
		TimerThread = std::thread(&GameTimer::RunTimer, this);
	}
}

int GameTimer::StopTimer()
{
	//std::unique_lock<std::mutex> Lock(Mutex);
	bIsRunning.store(false);
	CV.notify_one();
	TimerThread.join();
	return CurrentTime.load();
}
