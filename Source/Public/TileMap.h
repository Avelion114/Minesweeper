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
};

class TileMap
{
public:

	static std::map<int, const char*> TileResources;
	
	TileMap(Vector2 Dim);
	~TileMap();

	bool GetTileVisibility(Vector2 Tile);
	void ShowTile(Vector2 Tile, bool SkipCheck = false);
	void MarkTile(Vector2 Tile);

	Tile& GetTile(Vector2 Tile)
	{
		return Tiles[Tile.Height][Tile.Width];
	}

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
	int GetTouchingMines(Vector2 Tile)
	{
		return Tiles[Tile.Height][Tile.Width].MinesInProximity;
	}

private:
	
	std::vector<std::vector<Tile>> Tiles;
	Vector2 Dimensions;
};
