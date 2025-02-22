#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include "Scene.h"
#include "Tile.h"

enum : uint8_t
{
	INDEX_MINE = 9,
	INDEX_FLAG = 10
};

enum : uint8_t
{
	BORDER_T = 1,
	BORDER_TL,
	BORDER_L,
	BORDER_BL,
	BORDER_B,
	BORDER_BR,
	BORDER_R,
	BORDER_TR
};

//Forward Declarations
struct SDL_Rect;
enum class ETileType;
class Tile;

//Class for drawing the main map during a game and handling gameplay
class TileMap : public Scene
{
public:

	TileMap(Vector2 Dim, int TileSize, int MinesToSpawn);
	~TileMap() override;

	virtual void Draw(SDL_Surface* DrawSurface) override;
	virtual void ProcessInputEvents(SDL_Event& E) override;

	bool GetTileVisibility(Vector2 Tile);
	void ShowTile(Vector2 Tile);
	void MarkTile(Vector2 Tile);
	void RevealMines();
	void SetOnGameEnded(bool(*GameEnded)(bool))
	{
		OnGameEnded = GameEnded;
	}
	Vector2 GetDim()const {return Dimensions;}

	Tile& GetTile(Vector2 Tile)
	{
		return Tiles[Tile.Height()][Tile.Width()];
	}

	int GetTouchingMines(Vector2 Tile) const
	{
		return Tiles[Tile.Height()][Tile.Width()].MinesInProximity;
	}
	bool AllBombsDiffused()
	{
		return RemainingFlags == 0 && RemainingMines == 0;
	}

protected:

	virtual bool LoadResources() override;
	virtual void ClearResources() override;
	
	void GenerateTiles();
	void TryIncrementMines(Vector2 Tile);
	bool IsValidTile(Vector2 Tile) const
	{
		if(Tile.Height() >= 0 && Tile.Width() >= 0)
		{
			if(Tile.Height() < Dimensions.Height() && Tile.Width() < Dimensions.Width())
			{
				return true;
			}
		}
		return false;
	}


private:

	SDL_Surface* TileTypeSurface[11] = {nullptr};
	SDL_Surface* TileSurface = nullptr;
	SDL_Surface* BorderSurface = nullptr;

	//Callback for on game win or lose
	bool(*OnGameEnded)(bool);
	
	std::vector<std::vector<Tile>> Tiles;
	Vector2 Dimensions;
	int TILE_SIZE = 0;
	int NumMines = 0;
	std::vector<Vector2> Mines;

	int RemainingFlags = 0;
	int RemainingMines = 0;

	void DrawBorderTiles(int w, int h, int Width, int Height, SDL_Rect Destination, SDL_Surface* Surface);
};
