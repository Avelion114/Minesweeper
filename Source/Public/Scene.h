#pragma once

struct SDL_Surface;
union SDL_Event;

struct Vector2
{
	Vector2(int x = 0, int y = 0) : Width(x), Height(y) {}
	int Width;
	int Height;

	bool operator==(const Vector2& other) const
	{
		return (Width == other.Width) && (Height == other.Height);
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

protected:
	virtual bool LoadResources() = 0;
	virtual void ClearResources() = 0;
	

};
