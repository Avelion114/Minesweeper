#pragma once
#include <cstdint>
#include "Scene.h"

enum ButtonState : uint8_t
{
	Normal,
	Hovered,
	Pressed
};

class Button : public Scene
{
public:
	Button();
	virtual ~Button() override;
	void Draw(SDL_Surface* DrawSurface) override;
	void ProcessInputEvents(SDL_Event& E) override;
	
	void BindOnButtonPressed(void(*Function)())
	{
		OnButtonPressed = Function;
	}

protected:
	bool LoadResources() override;
	void ClearResources() override;
	void SetButtonState(ButtonState NewState);
	void CheckButtonState();

	bool IsMouseOver() const;

protected:
	ButtonState State = Normal;
	
	void(*OnButtonPressed)() = nullptr;
};
