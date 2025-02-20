#include "../Public/TileMap.h"

#include <ctime>
#include <iostream>

#include "../Public/Tile.h"
#include <map>
#include <ostream>

#include <bitset>

std::map<int, const char*> TileMap::TileResources =
{
	{0, nullptr},
	{1, "Resources/One.bmp"},
	{2, "Resources/Two.bmp"},
	{3, "Resources/Three.bmp"},
	{4, "Resources/Four.bmp"},
	{5, "Resources/Five.bmp"},
	{6, "Resources/Six.bmp"},
	{7, "Resources/Seven.bmp"},
	{8, "Resources/Eight.bmp"},
	{9, "Resources/Mine.bmp"},
	{10, "Resources/Flag.bmp"}
};

TileMap::TileMap(Vector2 Dim)
{
	Dimensions = Dim;
	Tiles = std::vector<std::vector<Tile>>(Dim.Height, std::vector<Tile>(Dim.Width));
	GenerateTiles();
}

TileMap::~TileMap()
{
	for(auto IT : Tiles)
	{
		IT.clear();
	}
	Tiles.clear();
}

void TileMap::ShowTile(Vector2 Tile)
{
	auto T = &Tiles[Tile.Height][Tile.Width];
	if(T->HasFlag(Flags::VISIBLE) || T->HasFlag(Flags::FLAG))
	{
		return;
	}
	

	T->SetFlag(Flags::VISIBLE);
	if(T->MinesInProximity == 0)
	{
		
		for(int h = Tile.Height - 1; h < Tile.Height + 2; h++)
		{
			for(int w = Tile.Width - 1; w < Tile.Width + 2; w++)
			{
				Vector2 NextTile(w,h);
				if(NextTile == Tile){continue;}
				//consider adding an array to track already checked tiles so we don't double count
				printf("Next Tile %i, %i\n", w, h);
				if(IsValidTile(NextTile))
				{
					//Tiles[NextTile.Height][NextTile.Width].SetFlag(Flags::VISIBLE);
					ShowTile(NextTile);
				}
			}
			std::cout << "H Value" << h << std::endl;
		}
	}
}

void TileMap::MarkTile(Vector2 Tile)
{
	auto T = &Tiles[Tile.Height][Tile.Width];
	if(T->HasFlag(Flags::VISIBLE)) return;
	
	if(T->HasFlag(Flags::FLAG))
	{
		T->ClearFlag(Flags::FLAG);
	}
	else
	{
		T->SetFlag(Flags::FLAG);
	}
	std::bitset<8> B(T->TileFlags);
	std::cout << B << std::endl;
}

void TileMap::GenerateTiles()
{
	int NumMines = 20; //Will be set with difficulty
	srand(time(NULL));
	//Generate random mines
	for(int i = 0; i < NumMines; i++)
	{
		int H = rand() % Dimensions.Height;
		int W = rand() % Dimensions.Width;

		if(Tiles[H][W].HasFlag(Flags::MINE))//If it's already a mine, try another one
		{
			i--;
			continue;
		}
		Tiles[H][W].SetFlag(Flags::MINE);

		//Tell tiles around this mine that it is touching a mine
		for(int h = H - 1; h < H + 2; h++)
		{
			for(int w = W - 1; w < W + 2; w++)
			{
				TryIncrementMines(Vector2(w, h));
			}
		}
	}
}

void TileMap::TryIncrementMines(Vector2 Tile)
{
	if(IsValidTile(Tile))
	{
		Tiles[Tile.Height][Tile.Width].MinesInProximity++;
	}
	else
	{
		printf("Invalid Tile: %i, %i\n", Tile.Height, Tile.Width);
	}
}

bool TileMap::GetTileVisibility(Vector2 Tile)
{
	return Tiles[Tile.Height][Tile.Width].HasFlag(Flags::VISIBLE);
}
