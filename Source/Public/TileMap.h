#pragma once
#include <cstdint>
#include <vector>
#include <map>

#include "Tile.h"


enum class ETileType;
class Tile;

struct Vector2
{
	Vector2(uint32_t x = 0, uint32_t y = 0) : Width(x), Height(y) {}
	uint32_t Width;
	uint32_t Height;

	bool operator==(const Vector2& other) const
	{
		return (Width == other.Width) && (Height == other.Height);
	}
};

class TileMap
{
public:

	static std::map<int, const char*> TileResources;
	
	TileMap(Vector2 Dim, int MinesToSpawn);
	~TileMap();

	bool GetTileVisibility(Vector2 Tile);
	void ShowTile(Vector2 Tile);
	void MarkTile(Vector2 Tile, int& Flags, int& RMines);
	void RevealMines();

	Tile& GetTile(Vector2 Tile)
	{
		return Tiles[Tile.Height][Tile.Width];
	}

	int GetTouchingMines(Vector2 Tile) const
	{
		return Tiles[Tile.Height][Tile.Width].MinesInProximity;
	}

protected:
	
	void GenerateTiles();
	void TryIncrementMines(Vector2 Tile);
	bool IsValidTile(Vector2 Tile) const
	{
		if(Tile.Height >= 0 && Tile.Width >= 0)
		{
			if(Tile.Height < Dimensions.Height && Tile.Width < Dimensions.Width)
			{
				return true;
			}
		}
		return false;
	}


private:
	
	std::vector<std::vector<Tile>> Tiles;
	Vector2 Dimensions;
	int NumMines = 0;
	std::vector<Vector2> Mines;
};
