#pragma once
#include "Scene.h"

//Can display a numerical value on the screen
class NumberDisplay : public Scene
{
public:

	NumberDisplay();
	NumberDisplay(unsigned int Number, unsigned int Max = 3);
	virtual ~NumberDisplay() override;

	virtual void Draw(SDL_Surface* DrawSurface) override;
	virtual void Initialize() override;
	virtual void ClearResources() override;

	virtual void UpdateNumber(unsigned int NewNumber);
	
	void SetFontSize(Vector2 NewSize)
	{
		FontSize = NewSize;
	}
	Vector2 GetFontSize() const {return FontSize;}

protected:
	virtual bool LoadResources() override;

	unsigned int Number = 0;

	//Max number of digits to contain in the number. Truncates the rest 
	unsigned int MaxDigits;

	std::vector<unsigned int> Digits;
	Vector2 FontSize = {16,32}; //Size of one number in the default font sheet

	bool bProcessingUpdate = false;

};
