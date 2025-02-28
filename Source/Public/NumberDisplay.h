#pragma once
#include "Scene.h"

class NumberDisplay : public Scene
{
public:

	NumberDisplay(int Number, unsigned int Max = 3);
	virtual ~NumberDisplay() override;

	void Draw(SDL_Surface* DrawSurface) override;
	void Initialize() override;
	void ClearResources() override;

	void UpdateNumber(int NewNumber);
	
	void SetFontSize(Vector2 NewSize)
	{
		FontSize = NewSize;
	}
	Vector2 GetFontSize() const {return FontSize;}

protected:
	bool LoadResources() override;

	int Number = 0;
	unsigned int MaxDigits;

	std::vector<int> Digits;
	Vector2 FontSize = {16,32}; //Size of default font sheet

private:

	bool bProcessingUpdate = false;
};
