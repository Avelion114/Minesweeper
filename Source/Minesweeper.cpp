#include <iostream>
#include "TileMap.h"
#include <SDL2/SDL.h>

const int TILE_SIZE = 23;
const int SCREEN_WIDTH = TILE_SIZE * 16;
const int SCREEN_HEIGHT = TILE_SIZE * 10;


SDL_Window* Window = nullptr;
SDL_Surface* WSurface = nullptr;
SDL_Surface* TileSurface = nullptr; //Tile image to load

bool Init();
bool LoadMedia();
void Close();
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
            SDL_Event E;
            
            int Width = SCREEN_WIDTH / TILE_SIZE;
            int Height = SCREEN_HEIGHT / TILE_SIZE;
            TileMap MainMap(Vector2(Width, Height));
            
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
                                if(E.button.button == SDL_BUTTON_LEFT)
                                {
                                    int x = E.button.x / TILE_SIZE;
                                    int y = E.button.y / TILE_SIZE;
                                    MainMap.SetTileState(Vector2(x,y), false);
                                }                                
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
                        if(MainMap.GetTileState(Vector2(w, h)))
                        {
                            SDL_Rect Destination = ConstructRect(TILE_SIZE,TILE_SIZE,w*TILE_SIZE,h*TILE_SIZE);
                            SDL_BlitSurface(TileSurface, nullptr, WSurface, &Destination);
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
    return true;
}

void Close()
{
    SDL_DestroyWindow(Window);
    SDL_FreeSurface(TileSurface);
    SDL_Quit();
}