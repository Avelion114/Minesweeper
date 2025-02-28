#include <Image.h>
#include <SDL2/SDL_image.h>

Image::Image(const char* ImagePath)
{
	Resources.insert({0, ImagePath});
}

Image::~Image()
{
}

void Image::Initialize()
{
	LoadResources();
}

void Image::Draw(SDL_Surface* DrawSurface)
{
	SDL_Surface* Surface = SceneSurfaces[0];
	SDL_Rect DrawRect = Surface->clip_rect;
	DrawRect.x = Position.x;
	DrawRect.y = Position.y;
	SDL_BlitSurface(Surface, nullptr, DrawSurface, &DrawRect);
}


bool Image::LoadResources()
{
	if(!Resources.empty())
	{
		SDL_Surface* surface = IMG_Load(Resources[0]);
		SceneSurfaces.push_back(surface);
	}
	return true;
}

void Image::ClearResources()
{
	Scene::ClearResources();
	Resources.clear();
}
