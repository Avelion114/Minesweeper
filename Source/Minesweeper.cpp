#include <iostream>
#include "TileMap.h"
#include <SDL2/SDL.h>

constexpr int TILE_SIZE = 23;
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

constexpr int GRID_X = TILE_SIZE * 10;
constexpr int GRID_Y = TILE_SIZE * 15;

int DIFFICULTY = 20;


SDL_Window* Window = nullptr;
SDL_Surface* WSurface = nullptr;
SDL_Surface* TileSurface = nullptr; //Covered tile image
SDL_Surface* TileTypeSurface[11] = {nullptr}; //Uncovered tile images
SDL_Surface* BorderSurface = nullptr; //Border tiles
TileMap* MainMap = nullptr;



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


bool Init();
bool LoadMedia();
bool LoadFromType(int Type);
void Close();
void DrawBorderTiles(int w, int h, int Width, int Height, SDL_Rect D2);
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
                //Draw Border
                int BWidth = Width + 2;
                int BHeight = Height + 2;
                for(int bw = 0; bw < BWidth; bw++)
                {
                    for(int bh = 0; bh < BHeight; bh++)
                    {
                        SDL_Rect Destination = ConstructRect(TILE_SIZE, TILE_SIZE, bw*TILE_SIZE + (OFFSET_X - TILE_SIZE), bh*TILE_SIZE + (OFFSET_Y - TILE_SIZE));
                    }
                }

                
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

                        //Draw Border Tiles
                        DrawBorderTiles(w, h, Width, Height, Destination);
                       
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
    BorderSurface = SDL_LoadBMP("Resources/Border_Tileset.bmp");
    if(TileSurface == nullptr)
    {
        std::cout << "Failed to load Border_Tileset.bmp" << SDL_GetError() << "\n";
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
    SDL_FreeSurface(BorderSurface);
    for(auto Surface : TileTypeSurface)
    {
        SDL_FreeSurface(Surface);
    }
    SDL_Quit();
}

void DrawBorderTiles(int w, int h, int Width, int Height, SDL_Rect Destination)
{
     if(w == 0) // Left side
    {
        SDL_Rect D2 = Destination;
        D2.x -= TILE_SIZE;
        SDL_Rect Source = ConstructRect(TILE_SIZE, TILE_SIZE, BORDER_L * TILE_SIZE, 0);
        SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
        if(h == 0)//Top Left corner
        {
            D2.y -= TILE_SIZE;
            Source.x = BORDER_TL * TILE_SIZE;
            SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
        }
        else if(h == Height - 1) //Bottom Left Corner
        {
            D2.y += TILE_SIZE;
            Source.x = BORDER_BL * TILE_SIZE;
            SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
        }
    }
    else if (w == Width - 1)//Right side
    {
        SDL_Rect D2 = Destination;
        D2.x += TILE_SIZE;
        SDL_Rect Source = ConstructRect(TILE_SIZE, TILE_SIZE, BORDER_R * TILE_SIZE, 0);
        SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
        if(h == 0)//Top Right corner
        {
            D2.y -= TILE_SIZE;
            Source.x = BORDER_TR * TILE_SIZE;
            SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
        }
        else if(h == Height - 1) //Bottom Right Corner
        {
            D2.y += TILE_SIZE;
            Source.x = BORDER_BR * TILE_SIZE;
            SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
        }
    }
    if(h == 0)//Top
    {
        SDL_Rect D2 = Destination;
        D2.y -= TILE_SIZE;
        SDL_Rect Source = ConstructRect(TILE_SIZE, TILE_SIZE, BORDER_T * TILE_SIZE, 0);
        SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
    }
    else if(h == Height - 1)//Bottom
    {
        SDL_Rect D2 = Destination;
        D2.y += TILE_SIZE;
        SDL_Rect Source = ConstructRect(TILE_SIZE, TILE_SIZE, BORDER_B * TILE_SIZE, 0);
        SDL_BlitSurface(BorderSurface, &Source, WSurface, &D2);
    }
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
