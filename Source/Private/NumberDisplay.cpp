#include "NumberDisplay.h"

#include <iostream>
#include <SDL2/SDL_image.h>

//Default Constructor
NumberDisplay::NumberDisplay()
{
	MaxDigits = 3;
	Resources[0] = "Resources/digital_font_sheet.png";
}

NumberDisplay::NumberDisplay(unsigned int InitNumber, unsigned int Max)
{
	MaxDigits = Max;
	Number = InitNumber;
	Resources[0] = "Resources/digital_font_sheet.png";
}

NumberDisplay::~NumberDisplay()
{
	Resources.clear();
}

void NumberDisplay::Draw(SDL_Surface* DrawSurface)
{
	int OFFSET = (FontSize.Width() * 7) + 1; // Skip first 8 digits in font sheet. Add one pixel
	
	for(int i = 0; i < Digits.size(); i++)
	{
		if(bProcessingUpdate){return;}//Exit loop if we've updated the number in the middle of drawing
		int Value = Digits[i];
		int x;
		int y;

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
		SDL_Rect DestRect{Position.x + FontSize.Width() * i, Position.y, FontSize.Width(), FontSize.Height()};
		
		SDL_BlitSurface(SceneSurfaces[0],&SrcRect,DrawSurface, &DestRect);
	}
}

void NumberDisplay::Initialize()
{
	LoadResources();
	UpdateNumber(Number);
}

void NumberDisplay::ClearResources()
{
	Scene::ClearResources();
}

void NumberDisplay::UpdateNumber(unsigned int NewNumber)
{
	bProcessingUpdate = true;

	//Truncate number if larger than our max size
	if(NewNumber > std::pow(10, MaxDigits - 1))
	{
		unsigned int Diff = static_cast<int>(std::floor(std::log(NewNumber) / std::log(10))) - (MaxDigits - 1);
		NewNumber /= static_cast<int>(std::pow(10, Diff));
	}
	Number = NewNumber;

	Digits.clear();

	while(NewNumber > 9)
	{
		Digits.push_back(NewNumber % 10);
		NewNumber /= 10;
	}
	Digits.push_back(NewNumber);
	Digits.resize(MaxDigits, 0); //Add leading zeros if needed
	std::reverse(Digits.begin(), Digits.end());
	

	bProcessingUpdate = false;
}

bool NumberDisplay::LoadResources()
{
	SDL_Surface* FontSheet = IMG_Load(Resources[0]);
	if(FontSheet)
	{
		SceneSurfaces.push_back(FontSheet);
		return true;
	}
	std::cout << "Error loading fonts\n";
	return false;
}
