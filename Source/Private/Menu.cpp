#include "Menu.h"

#include <SDL2/SDL_surface.h>

#include "Button.h"

Menu::~Menu()
{
	for(auto Button : MenuButtons)
	{
		delete Button;
		Button = nullptr;
	}
	MenuButtons.clear();
}

void Menu::Draw(SDL_Surface* DrawSurface)
{
	
	for(auto Scene : MenuButtons)//Draw all buttons
	{
		Scene->Draw(DrawSurface);
	}
}

void Menu::ProcessInputEvents(SDL_Event& E)
{
		for(auto Scene : MenuButtons)//Process input for buttons
		{
			if(bShouldDestroy) break; //Terminate loop if we've been marked for destruction
			if(Scene)
			{
				Scene->ProcessInputEvents(E);
			}
		}
}

bool Menu::AddButton(Vector2 ButtonPosition, const char* TitlePath, void(*Callback)())
{
	Button* NewButton = new Button;
	NewButton->Position = ButtonPosition;
	if(Callback) NewButton->BindOnButtonPressed(Callback);
	if(TitlePath) NewButton->SetButtonTitle(TitlePath);
	MenuButtons.push_back(NewButton);
	return true;
}
