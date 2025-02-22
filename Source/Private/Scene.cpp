#include "Scene.h"
#include <SDL2/SDL.h>

void Scene::ClearResources()
{
	for(auto Surface : SceneSurfaces)
	{
		SDL_FreeSurface(Surface);
	}
	SceneSurfaces.clear();
}
