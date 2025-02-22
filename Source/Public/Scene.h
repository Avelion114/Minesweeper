#pragma once
#include <vector>
#include <map>

struct SDL_Surface;
union SDL_Event;

struct Vector2
{
	Vector2(int x = 0, int y = 0) : x(x), y(y){}
	int x, y;
	int Width() const {return x;}
	int Height() const {return y;}

	bool operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}
};

//Base class for objects that are drawn to the screen
class Scene
{
public:

	Scene() = default;
	virtual ~Scene() = default;

	virtual void Draw(SDL_Surface* DrawSurface, int SCREEN_WIDTH, int SCREEN_HEIGHT) {}
	virtual void Draw(SDL_Surface* DrawSurface) = 0;
	virtual void ProcessInputEvents(SDL_Event& E) = 0;

	Vector2 Position; //Position within window
	Vector2 Size;

protected:
	virtual bool LoadResources() = 0;
	virtual void ClearResources();

	std::vector<SDL_Surface*> SceneSurfaces;
	std::map<int, const char*> Resources;
	
	

};
