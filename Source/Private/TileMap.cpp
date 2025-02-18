#include "../Public/TileMap.h"

#include <algorithm>

TileMap::TileMap(Vector2 Dim)
{
	Dimensions = Dim;
	Tiles = new bool*[Dimensions.Height];
	for(int i = 0; i < Dimensions.Height; i++)
	{
		Tiles[i] = new bool[Dimensions.Width];
		std::fill(Tiles[i], Tiles[i] + Dimensions.Width, true);
	}
}

TileMap::~TileMap()
{
	for(int i = 0; i < Dimensions.Height; i++)
	{
		delete[] Tiles[i];
	}
	delete[] Tiles;
	Tiles = nullptr;
}

void TileMap::SetTileState(Vector2 Tile, bool Value)
{
	Tiles[Tile.Height][Tile.Width] = Value;
}

bool TileMap::GetTileState(Vector2 Tile)
{
	return Tiles[Tile.Height][Tile.Width];
}
