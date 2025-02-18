#pragma once
#include <cstdint>

struct Vector2
{
	Vector2(uint32_t x = 0, uint32_t y = 0) : Width(x), Height(y) {}
	uint32_t Width;
	uint32_t Height;
};

class TileMap
{
public:
	
	TileMap(Vector2 Dim);
	~TileMap();

	bool GetTileState(Vector2 Tile);
	void SetTileState(Vector2 Tile, bool Value);

private:
	bool** Tiles = nullptr;

	Vector2 Dimensions;
	
};
