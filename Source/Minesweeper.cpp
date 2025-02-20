#include <iostream>
#include "TileMap.h"
#include <SDL2/SDL.h>

constexpr int TILE_SIZE = 23;
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

constexpr int GRID_X = TILE_SIZE * 15;
constexpr int GRID_Y = TILE_SIZE * 15;

int DIFFICULTY = 20;


SDL_Window* Window = nullptr;
SDL_Surface* WSurface = nullptr;
SDL_Surface* TileSurface = nullptr; //Covered tile image
SDL_Surface* TileTypeSurface[11] = {nullptr}; //Uncovered tile images
SDL_Surface* BorderSurface[4] = {nullptr}; //Border tiles
TileMap* MainMap = nullptr;



enum : uint8_t
{
    INDEX_MINE = 9,
    INDEX_FLAG = 10
};


bool Init();
bool LoadMedia();
bool LoadFromType(int Type);
void Close();
bool EndGame(bool GameWon);

SDL_Rect ConstructRect(int Height, int Width, int PosX, int PosY)
{
    SDL_Rect Rect;
    Rect.h = Height;
    Rect.w = Width;
    Rect.x = PosX;
    Rect.y = PosY;
    return Rect;
}

int main(int argc, char* argv[])
{
    if(Init())
    {
        if(LoadMedia())
        {

            bool Quit = false;
            bool GameOver = false;
            SDL_Event E;
            
            int Width = GRID_X / TILE_SIZE;
            int Height = GRID_Y / TILE_SIZE;
            int RemainingMines = DIFFICULTY, RemainingFlags = DIFFICULTY;
            MainMap = new TileMap(Vector2(Width, Height), RemainingMines);
            int OFFSET_X = (SCREEN_WIDTH  - GRID_X) / 2;
            int OFFSET_Y = (SCREEN_HEIGHT - GRID_Y) / 2;
            
            
            while(!Quit)//Main loop
            {
                //Process events
                while(SDL_PollEvent(&E))
                {
                    switch(E.type)
                    {
                        case SDL_QUIT:
                            Quit = true; break;

                        case SDL_MOUSEBUTTONDOWN:
                            {
                                if(!GameOver)//Only handle tile mouse events while game is in progress
                                {
                                    int x = (E.button.x - OFFSET_X) / TILE_SIZE;
                                    int y = (E.button.y - OFFSET_Y) / TILE_SIZE;
                                    Vector2 Tile(x, y);
                                    if(E.button.button == SDL_BUTTON_LEFT)
                                    {
                                        MainMap->ShowTile(Tile);
                                        if(MainMap->GetTile(Tile).HasFlag(Flags::MINE))
                                        {
                                            GameOver = true;
                                            MainMap->RevealMines();
                                            EndGame(false);
                                        }
                                    }
                                    else if(E.button.button == SDL_BUTTON_RIGHT)
                                    {
                                        MainMap->MarkTile(Tile, RemainingFlags, RemainingMines);
                                        if(RemainingFlags == 0 && RemainingMines == 0)
                                        {
                                            GameOver = true;
                                            EndGame(true);
                                        }
                                    }
                                }
                                break;
                            }

                    default: break;
                    }
                }
                 SDL_FillRect(WSurface, nullptr, SDL_MapRGB(WSurface->format, 0xB0, 0xB0, 0xB0)); //Draw background
                //Draw array of tiles
                for(int w = 0; w < Width; w++)
                {
                    for(int h = 0; h < Height; h++)
                    {
                        Vector2 CurrentTile(w,h);
                        SDL_Rect Destination = ConstructRect(TILE_SIZE,TILE_SIZE,w*TILE_SIZE + OFFSET_X,h*TILE_SIZE + OFFSET_Y);
                        //Cover the underlying tile if it hasn't been exposed yet
                        if(!MainMap->GetTileVisibility(CurrentTile))
                        {
                            if(MainMap->GetTile(CurrentTile).HasFlag(Flags::FLAG))
                            {
                                SDL_BlitSurface(TileTypeSurface[INDEX_FLAG], nullptr, WSurface, &Destination);
                            }
                            else
                            {
                                SDL_BlitSurface(TileSurface, nullptr, WSurface, &Destination);
                            }
                           
                        }
                        else //tile is visible. blit the correct texture based on type
                        {
                            int I;
                            if(MainMap->GetTile(CurrentTile).HasFlag(Flags::MINE)){I = INDEX_MINE;}
                            else
                            {
                                I = MainMap->GetTouchingMines(CurrentTile);
                            }
                            SDL_BlitSurface(TileTypeSurface[I], nullptr, WSurface, &Destination);
                        }
                    }
                }
                SDL_UpdateWindowSurface(Window);
            }
        }
        else
        {
            std::cout << "Failed to load media\n";
        }
    }
    else
    {
        std::cout << "Failed to initialize\n";
    }

    Close();
    return 0;
}

bool Init()
{
    bool Success = false;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Error: Could not initialize video: " << SDL_GetError() << "\n";
    }
    else
    {
        Window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,  SDL_WINDOW_SHOWN);
        if(Window == nullptr)
        {
            std::cout << "Failed to create SDL window: " << SDL_GetError() << "\n";
        }
        else
        {
            WSurface = SDL_GetWindowSurface(Window);
            SDL_FillRect(WSurface, nullptr, SDL_MapRGB(WSurface->format, 0x0, 0xFF, 0x0));
            Success = true;
        }
    }

    return Success;
}

bool LoadMedia()
{
    TileSurface = SDL_LoadBMP("Resources/Tile.bmp");
    if(TileSurface == nullptr)
    {
        std::cout << "Failed to load Tile.bmp" << SDL_GetError() << "\n";
        return false;
    }
    for(int i = 0; i < 11; i++)
    {
        LoadFromType(i);
    }
    return true;
}

bool LoadFromType(int Type)
{
    const char* Path = TileMap::TileResources.at(Type);
    if(Path != nullptr)
    {
        TileTypeSurface[Type] = SDL_LoadBMP(Path);
        if(TileTypeSurface[Type] == nullptr)
        {
            std::cout << "Failed to load resource to TileTypeSurface:" << SDL_GetError() << "\n";
            return false;
        }
    }
    return true;
}

void Close()
{
    delete(MainMap);
    SDL_DestroyWindow(Window);
    SDL_FreeSurface(TileSurface);
    for(auto Surface : TileTypeSurface)
    {
        SDL_FreeSurface(Surface);
    }
    for(auto Surface : BorderSurface)
    {
        SDL_FreeSurface(Surface);
    }
    SDL_Quit();
}

bool EndGame(bool GameWon)
{
    if(GameWon)
    {
        std::cout << "Congrats!\nYou Won!" << std::endl;
    }
    else
    {
        std::cout << "You Lose :(" << std::endl;
    }
    return false;
}
