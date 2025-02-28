#include <Button.h>
#include <iostream>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

Button::Button()
{
	SetButtonState(ButtonState::Normal);
	bReceiveInput = true; //Buttons should receive input by default
}

Button::~Button()
{
	//ClearResources();
}

void Button::Draw(SDL_Surface* DrawSurface)
{
	
	SDL_Surface* Surface = SceneSurfaces[State];
	SDL_Rect DrawRect = Surface->clip_rect;
	DrawRect.x = Position.x;
	DrawRect.y = Position.y;
	SDL_BlitSurface(Surface, nullptr, DrawSurface, &DrawRect);
	if(ButtonTitle)//Draw button title if available
	{
		SDL_BlitSurface(ButtonTitle, nullptr, DrawSurface, &DrawRect);
	}
	
	CheckButtonState();
	
}

void Button::ProcessInputEvents(SDL_Event& E)
{
	switch(State)
	{
		case ButtonState::Normal: return;
		case ButtonState::Hovered:
			{
				if(E.type == SDL_MOUSEBUTTONDOWN)
				{
					if(E.button.button == SDL_BUTTON_LEFT) SetButtonState(ButtonState::Pressed);
				}
				break;
			}
		case ButtonState::Pressed:
			{
				if(E.type == SDL_MOUSEBUTTONUP)
				{
					if(E.button.button == SDL_BUTTON_LEFT) //Button was released
					{
						SetButtonState(ButtonState::Hovered);
						if(OnButtonPressed) OnButtonPressed();
					}
				}
				break;
			}
	}
	return;
}

void Button::Initialize()
{
	if(LoadResources())//Honestly it's probably better to do this elsewhere so buttons don't all allocate resources individually but whatever
	{
		//Set size to button rectangle
		SDL_Rect ButtonRect = SceneSurfaces[State]->clip_rect;
		Size = Vector2(ButtonRect.w, ButtonRect.h);
		std::cout << Size << "\n";
	}
}

bool Button::SetButtonTitle(const char* ResourcePath)
{
	ButtonTitle = IMG_Load(ResourcePath);
	if(ButtonTitle == nullptr) return false;
	return true;
}

bool Button::LoadResources()
{
	Resources = std::map<int, const char*>
	{
		{ButtonState::Normal, "Resources/Button/Button_Base.png"},
		{ButtonState::Hovered, "Resources/Button/Button_Hovered.png"},
		{ButtonState::Pressed, "Resources/Button/Button_Pressed.png"}
	};

	for(auto i : Resources)
	{
		SDL_Surface* Surface = IMG_Load(i.second);
		SceneSurfaces.push_back(Surface);
	}
	
	return true;
}

void Button::ClearResources()
{
	Scene::ClearResources();
	SDL_FreeSurface(ButtonTitle);
	ButtonTitle = nullptr;
	OnButtonPressed = nullptr;
}

void Button::SetButtonState(ButtonState NewState)
{
	State = NewState;
}

void Button::CheckButtonState()
{
	if(IsMouseOver() && State == ButtonState::Normal) SetButtonState(ButtonState::Hovered);
	else if(!IsMouseOver() && State == ButtonState::Hovered) SetButtonState(ButtonState::Normal);
}

bool Button::IsMouseOver() const
{
	int x;
	int y;
	SDL_GetMouseState(&x, &y);
	if(x > Position.x && y > Position.y)
	{
		return(x - Position.x < Size.x && y - Position.y < Size.y);
	}
	return false;
}
