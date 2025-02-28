#pragma once
#include <vector>
#include <map>
#include <ostream>

struct SDL_Surface;
union SDL_Event;
struct Vector2
{
	Vector2(int x = 0, int y = 0) : x(x), y(y){}
	int x, y;

	//Helper functions for readability in context to height and width
	int Width() const {return x;}
	int Height() const {return y;}

	static Vector2 Zero() {return {0,0};}

	bool operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	Vector2 operator-(const Vector2& other) const
	{
		return {x - other.x, y - other.y};
	}

	Vector2 operator+(const Vector2& other) const
	{
		return {other.x + x, other.y + y};
	}

	
};
inline std::ostream& operator<< (std::ostream& os, Vector2 Vec)
{
	return os << "(" << Vec.x << ", " << Vec.y << ")";
}

//Base class for objects that are drawn to the screen
class Scene
{
public:

	Scene() = default;
	virtual ~Scene() = default;

	virtual void Draw(SDL_Surface* DrawSurface, int SCREEN_WIDTH, int SCREEN_HEIGHT) {}
	virtual void Draw(SDL_Surface* DrawSurface) = 0;
	virtual void ProcessInputEvents(SDL_Event& E) {}

	Vector2 Position; //Position within window
	Vector2 Size;

	virtual Vector2 GetSceneDimensions(){return Vector2::Zero();}

	bool bShouldDestroy = false;

	virtual void Initialize() = 0; //Used for post constructor functions such as LoadResources.
	virtual void ClearResources();

	bool bReceiveInput = false;

protected:
	virtual bool LoadResources() = 0;
	

	std::vector<SDL_Surface*> SceneSurfaces;
	std::map<int, const char*> Resources;
	
	
	

};
