#pragma once
#include <cstdint>

//flags


enum class Flags
{
	VISIBLE = 0x01,
	MINE = 0x02,
	FLAG = 0x04
};


class Tile
{
public:
	friend class TileMap;

	Tile();
	
	bool HasFlag(Flags Flag) const
	{
		uint8_t Set = TileFlags & static_cast<uint8_t>(Flag);
		return Set == static_cast<uint8_t>(Flag);
	}
	void SetFlag(Flags Flag)
	{
		TileFlags = TileFlags | static_cast<uint8_t>(Flag);
	}
	void ClearFlag(Flags Flag)
	{
		TileFlags = TileFlags & !static_cast<uint8_t>(Flag);
	}

private:
	int MinesInProximity = 0;

	uint8_t TileFlags = 0x00;
	
};
