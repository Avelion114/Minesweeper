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
	virtual void Initialize() override;
	bool SetButtonTitle(const char* ResourcePath);
	
	void BindOnButtonPressed(void(*Function)())
	{
		OnButtonPressed = Function;
	}
	void ClearResources() override;
	Vector2 GetSceneDimensions() override
	{
		return {194, 53};//Size of the button image
	}

protected:
	bool LoadResources() override;
	
	void SetButtonState(ButtonState NewState);
	void CheckButtonState();

	bool IsMouseOver() const;

protected:
	ButtonState State = Normal;
	
	void(*OnButtonPressed)() = nullptr;
	SDL_Surface* ButtonTitle = nullptr;
};
