#pragma once
#include "Scene.h"

class Button;

class Menu : public Scene
{
public:

	Menu(){}
	virtual ~Menu() override;
	void Draw(SDL_Surface* DrawSurface) override;
	void ProcessInputEvents(SDL_Event& E) override;
	bool AddButton(Vector2 ButtonPosition, const char* TitlePath = nullptr, void(*Callback)() = nullptr);

protected:
	bool LoadResources() override{return true;}
	void ClearResources() override{}

	std::vector<Button*> MenuButtons;
};
