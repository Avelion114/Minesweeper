#include <iostream>
#include <SDL2/SDL.h>

#include "Button.h"
#include "TileMap.h"
#include "Menu.h"

constexpr int TILE_SIZE = 23;
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

SDL_Window* Window = nullptr;
SDL_Surface* WSurface = nullptr;

std::vector<Scene*> sceneStack; //All scenes to be drawn

enum class Difficulty : uint8_t
{
    EASY,
    MEDIUM,
    HARD
};

enum class GameState
{
    MAIN_MENU = 0,
    IN_PROGRESS,
    GAME_OVER
};

GameState CurrentState = GameState::MAIN_MENU; //Set the game state

bool Init();
bool LoadMedia();
bool LoadFromType(int Type);
void Close();
bool EndGame(bool GameWon);
void CleanUpMarkedScenes();
void SetGameState(GameState NewState, Difficulty NewDifficulty = Difficulty::EASY);

int main(int argc, char* argv[])
{
    if(Init())
    {
        if(LoadMedia())
        {

            bool Quit = false;
            SDL_Event E;

            SetGameState(GameState::MAIN_MENU);
   
            while(!Quit)//Main loop
            {
                //Process events
                while(SDL_PollEvent(&E))
                {
                    switch(E.type)
                    {
                    case SDL_QUIT:
                        Quit = true; break;
                    default:
                        {
                            if(sceneStack.back())//Only handle top level scene input
                            {
                                sceneStack.back()->ProcessInputEvents(E);
                            }
                            break;
                        }
                    }
                }
                SDL_FillRect(WSurface, nullptr, SDL_MapRGB(WSurface->format, 0xB0, 0xB0, 0xB0)); //Draw background
                for(auto scene : sceneStack) //Draw all scenes
                {
                    scene->Draw(WSurface); 
                }
                SDL_UpdateWindowSurface(Window);
                CleanUpMarkedScenes(); //Destroy scenes marked for destruction this frame
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
 
    return true;
}

void Close()
{
    for(auto scene : sceneStack)
    {
        delete scene;
    }
    sceneStack.clear();
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void SetGameState(GameState NewState, Difficulty NewDifficulty)
{
    switch(NewState)
    {
    case GameState::MAIN_MENU:
        {
            Menu* MainMenu = new Menu;
            //194 x 53 is the Button image size. Could make a function to make this more modular later
            int CENTER_X = (SCREEN_WIDTH / 2) - (194 / 2); 
            int CENTER_Y = (SCREEN_HEIGHT / 2) - (53 / 2);
            
            MainMenu->AddButton(Vector2(CENTER_X,CENTER_Y - 60), "Resources/Title/Title_Easy.png", []()
                {
                    SetGameState(GameState::IN_PROGRESS, Difficulty::EASY);                
            });
            MainMenu->AddButton(Vector2(CENTER_X,CENTER_Y), "Resources/Title/Title_Medium.png", []()
                 {
                     SetGameState(GameState::IN_PROGRESS, Difficulty::MEDIUM);
                 });
            MainMenu->AddButton(Vector2(CENTER_X,CENTER_Y + 60), "Resources/Title/Title_Hard.png", []()
                  {
                      SetGameState(GameState::IN_PROGRESS, Difficulty::HARD);                
              });
            sceneStack.push_back(MainMenu);
            break;
        }
    case GameState::IN_PROGRESS:
        {
            
            sceneStack.back()->bShouldDestroy = true; //Mark menu for destruction

            int Width = 0, Height = 0, DIFFICULTY = 0;
            switch(NewDifficulty)
            {
            case Difficulty::EASY:{Width = 10; Height = 10; DIFFICULTY = 10; break;}
            case Difficulty::MEDIUM:{Width = 15; Height = 15; DIFFICULTY = 20; break;}
            case Difficulty::HARD:{Width = 15; Height = 15; DIFFICULTY = 50; break;}
            }

            TileMap* NewMap = new TileMap(Vector2(Width, Height), 23, DIFFICULTY);
            int PosX = (SCREEN_WIDTH  - (Width * TILE_SIZE)) / 2;
            int PosY = (SCREEN_HEIGHT - (Height * TILE_SIZE)) / 2;
            NewMap->Position = Vector2(PosX, PosY);
            NewMap->SetOnGameEnded(&EndGame);
            sceneStack.push_back(NewMap);

            break;
        }
    case GameState::GAME_OVER:
        {
            sceneStack.push_back(new Menu);
            break;
        }
    }
    CurrentState = NewState;
}




bool EndGame(bool GameWon)
{
    SetGameState(GameState::GAME_OVER);
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

void CleanUpMarkedScenes()
{
    for(int i = 0; i < sceneStack.size(); i++)
    {
        Scene* scene = sceneStack[i];
        if(scene->bShouldDestroy)
        {
            delete scene;
            sceneStack.erase(sceneStack.begin() + i);
            std::cout << "Scene Destroyed at index : " << i << std::endl;
        }
    }
}
