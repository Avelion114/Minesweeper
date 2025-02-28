#pragma once
#include <SDL2/SDL_surface.h>

#include "Scene.h"


//Class to draw a simple image to the screen
class Image : public Scene
{
public:
	Image() = default;
	Image(const char* ImagePath);
	virtual ~Image() override;
	virtual void Initialize() override;
	void Draw(SDL_Surface* DrawSurface) override;
	void ClearResources() override;

	Vector2 GetSceneDimensions() override
	{
		if(!SceneSurfaces.empty())
		{
			auto rect = SceneSurfaces[0]->clip_rect;
			return {rect.w, rect.h};
		}
		return Vector2::Zero();
	}

protected:
	bool LoadResources() override;
	
};
