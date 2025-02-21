#pragma once
#include "Scene.h"

class Menu : public Scene
{
public:

	Menu(){}
	virtual ~Menu() override{}
	void Draw(SDL_Surface* DrawSurface) override{}
	void ProcessInputEvents(SDL_Event& E) override{}

protected:
	bool LoadResources() override{return true;}
	void ClearResources() override{}
};
