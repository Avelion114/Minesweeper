#include "../Public/TileMap.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "../Public/Tile.h"
#include <map>
#include <ostream>



TileMap::TileMap(Vector2 Dim, int TileSize, int MinesToSpawn) : Dimensions(Dim), TILE_SIZE(TileSize), NumMines(MinesToSpawn)
{
	Tiles = std::vector<std::vector<Tile>>(Dim.Height(), std::vector<Tile>(Dim.Width()));
	RemainingFlags = MinesToSpawn;
	RemainingMines = MinesToSpawn;
	bReceiveInput = true;
}

TileMap::~TileMap()
{
	for(auto IT : Tiles)
	{
		IT.clear();
	}
	Tiles.clear();
}

void TileMap::Draw(SDL_Surface* DrawSurface)
{
	int Width = Dimensions.Width();
	int Height = Dimensions.Height();
	//Draw array of tiles
	for(int w = 0; w < Width; w++)
	{
		for(int h = 0; h < Height; h++)
		{
			Vector2 CurrentTile(w,h);
			SDL_Rect Destination = {w*TILE_SIZE + Position.x,h*TILE_SIZE + Position.y,TILE_SIZE,TILE_SIZE};
			//Cover the underlying tile if it hasn't been exposed yet
			if(!GetTileVisibility(CurrentTile))
			{
				if(GetTile(CurrentTile).HasFlag(Flags::FLAG))
				{
					SDL_BlitSurface(TileTypeSurface[INDEX_FLAG], nullptr, DrawSurface, &Destination);
				}
				else
				{
					SDL_BlitSurface(TileSurface, nullptr, DrawSurface, &Destination);
				}
                           
			}
			else //tile is visible. blit the correct texture based on type
			{
				int I;
				if(GetTile(CurrentTile).HasFlag(Flags::MINE)){I = INDEX_MINE;}
				else
				{
					I = GetTouchingMines(CurrentTile);
				}
				SDL_BlitSurface(TileTypeSurface[I], nullptr, DrawSurface, &Destination);
			}

			//Draw Border Tiles
			DrawBorderTiles(w, h, Width, Height, Destination, DrawSurface);
                       
		}
	}
}

void TileMap::ProcessInputEvents(SDL_Event& E)
{
	 switch(E.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                {
                       
                        int x = (E.button.x - Position.x) / TILE_SIZE;
                        int y = (E.button.y - Position.y) / TILE_SIZE;
                        Vector2 Tile(x, y);
                        if(E.button.button == SDL_BUTTON_LEFT)
                        {
                            ShowTile(Tile);
                            if(GetTile(Tile).HasFlag(Flags::MINE))
                            {
                                RevealMines();
                            	if(OnGameEnded)
                            	{
                            		OnGameEnded(false);
                            	}
                            }
                        }
                        else if(E.button.button == SDL_BUTTON_RIGHT)
                        {
                            MarkTile(Tile);
                            if(AllBombsDiffused())
                            {
                            	if(OnGameEnded)
                            	{
                            		OnGameEnded(true);
                            	}
                            }
                        }
                }
        }
}

void TileMap::Initialize()
{
	if(LoadResources())
	{
		GenerateTiles();
	}
}

void TileMap::DrawBorderTiles(int w, int h, int Width, int Height, SDL_Rect Destination, SDL_Surface* DrawSurface)
{
	if(w == 0) // Left side
	{
		SDL_Rect D2 = Destination;
		D2.x -= TILE_SIZE;
		SDL_Rect Source = {BORDER_L * TILE_SIZE, 0,TILE_SIZE, TILE_SIZE};
		SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
		if(h == 0)//Top Left corner
		{
			D2.y -= TILE_SIZE;
			Source.x = BORDER_TL * TILE_SIZE;
			SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
		}
		else if(h == Height - 1) //Bottom Left Corner
		{
			D2.y += TILE_SIZE;
			Source.x = BORDER_BL * TILE_SIZE;
			SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
		}
	}
	else if (w == Width - 1)//Right side
	{
		SDL_Rect D2 = Destination;
		D2.x += TILE_SIZE;
		SDL_Rect Source = {BORDER_R * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
		SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
		if(h == 0)//Top Right corner
		{
			D2.y -= TILE_SIZE;
			Source.x = BORDER_TR * TILE_SIZE;
			SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
		}
		else if(h == Height - 1) //Bottom Right Corner
		{
			D2.y += TILE_SIZE;
			Source.x = BORDER_BR * TILE_SIZE;
			SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
		}
	}
	if(h == 0)//Top
	{
		SDL_Rect D2 = Destination;
		D2.y -= TILE_SIZE;
		SDL_Rect Source = {BORDER_T * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
		SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
	}
	else if(h == Height - 1)//Bottom
	{
		SDL_Rect D2 = Destination;
		D2.y += TILE_SIZE;
		SDL_Rect Source = {BORDER_B * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
		SDL_BlitSurface(BorderSurface, &Source, DrawSurface, &D2);
	}
}

void TileMap::ShowTile(Vector2 Tile)
{
	auto T = &Tiles[Tile.Height()][Tile.Width()];
	if(T->HasFlag(Flags::VISIBLE) || T->HasFlag(Flags::FLAG))
	{
		return;
	}
	

	T->SetFlag(Flags::VISIBLE);
	if(T->MinesInProximity == 0)
	{
		
		for(int h = Tile.Height() - 1; h < Tile.Height() + 2; h++)
		{
			for(int w = Tile.Width() - 1; w < Tile.Width() + 2; w++)
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
	if(!IsValidTile(Tile)) return;
	auto T = &Tiles[Tile.Height()][Tile.Width()];
	if(T->HasFlag(Flags::VISIBLE)) return;
	
	if(T->HasFlag(Flags::FLAG))
	{
		T->ClearFlag(Flags::FLAG);
		RemainingFlags++;
		if(T->HasFlag(Flags::MINE))
		{
			RemainingMines++;
		}
	}
	else if(RemainingFlags > 0)
	{
		T->SetFlag(Flags::FLAG);
		RemainingFlags--;
		if(T->HasFlag(Flags::MINE))
		{
			RemainingMines--;
		}
	}
	if(OnFlag)OnFlag(RemainingFlags);
}

void TileMap::RevealMines()
{
	for(auto Tile : Mines)
	{
		Tiles[Tile.Height()][Tile.Width()].SetFlag(Flags::VISIBLE);
	}
}

bool TileMap::LoadResources()
{
	Resources = std::map<int, const char*>
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
	
	TileSurface = SDL_LoadBMP("Resources/Tile.bmp");
	if(TileSurface == nullptr)
	{
		std::cout << "Failed to load Tile.bmp" << SDL_GetError() << "\n";
		return false;
	}
	BorderSurface = SDL_LoadBMP("Resources/Border_Tileset.bmp");
	if(BorderSurface == nullptr)
	{
		std::cout << "Failed to load Border_Tileset.bmp" << SDL_GetError() << "\n";
		return false;
	}
	
	for(int i = 0; i < 11; i++)
	{
		const char* Path = Resources.at(i);
		if(Path != nullptr)
		{
			TileTypeSurface[i] = SDL_LoadBMP(Path);
			if(TileTypeSurface[i] == nullptr)
			{
				std::cout << "Failed to load resource to TileTypeSurface:" << SDL_GetError() << "\n";
				return false;
			}
		}
	}
	return true;
}

void TileMap::ClearResources()
{
	
	SDL_FreeSurface(TileSurface);
	TileSurface = nullptr;
	for(auto& Surface : TileTypeSurface)
	{
		SDL_FreeSurface(Surface);
		Surface = nullptr;
	}
}

void TileMap::GenerateTiles()
{
	
	//Generate random mines
	for(int i = 0; i < NumMines; i++)
	{
		int H = rand() % Dimensions.Height();
		int W = rand() % Dimensions.Width();

		if(Tiles[H][W].HasFlag(Flags::MINE))//If it's already a mine, try another one
		{
			i--;
			continue;
		}
		Tiles[H][W].SetFlag(Flags::MINE);
		Mines.emplace_back(W,H);

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
		Tiles[Tile.Height()][Tile.Width()].MinesInProximity++;
	}
	else
	{
		printf("Invalid Tile: %i, %i\n", Tile.Height(), Tile.Width());
	}
}

bool TileMap::GetTileVisibility(Vector2 Tile)
{
	return Tiles[Tile.Height()][Tile.Width()].HasFlag(Flags::VISIBLE);
}
