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
	virtual void Initialize() override{} // not needed because we have no resources in this class
	bool AddButton(Vector2 ButtonPosition, const char* TitlePath = nullptr, void(*Callback)() = nullptr);
	void ClearResources() override{}

protected:
	bool LoadResources() override{return true;}
	

	std::vector<Button*> MenuButtons;
};
