#include <iostream>
#include <SDL2/SDL.h>
#include <ctime>
#include "Button.h"
#include "GameTimer.h"
#include "Image.h"
#include "TileMap.h"
#include "Menu.h"
#include "NumberDisplay.h"

constexpr int TILE_SIZE = 23;
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

#define ALL_SCENES for(auto& scene : sceneStack)


SDL_Window* Window = nullptr;
SDL_Surface* WSurface = nullptr;

std::vector<Scene*> sceneStack; //All scenes to be drawn
GameTimer* Timer = nullptr;


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
    GAME_OVER,
    GAME_QUIT
};

GameState CurrentState = GameState::MAIN_MENU; //Set the game state
std::pair<bool, GameState> GameStateDeferred = {false, GameState::MAIN_MENU};

bool Init();
bool LoadMedia();
bool LoadFromType(int Type);
void Close();
bool EndGame(bool GameWon);
void CleanUpMarkedScenes();
void SetGameState(GameState NewState, Difficulty NewDifficulty = Difficulty::EASY);
void SetGameStateDeferred(GameState NewState)
{
    GameStateDeferred = {true, NewState};
}
Vector2 GetCenterPosition(Vector2 Size); //Returns the centered position on the screen based on size of Scene

int main(int argc, char* argv[])
{
    srand(time(nullptr));
    if(Init())
    {
        if(LoadMedia())
        {
            SDL_Event E;

            SetGameState(GameState::MAIN_MENU);
   
            while(CurrentState != GameState::GAME_QUIT)//Main loop
            {
                //Process events
                while(SDL_PollEvent(&E))
                {
                    switch(E.type)
                    {
                    case SDL_QUIT:
                        {
                            SetGameStateDeferred(GameState::GAME_QUIT);
                            break; 
                        }
                    default:
                        {
                            ALL_SCENES //Process input for scenes
                            {
                                if(scene->bReceiveInput)
                                {
                                    scene->ProcessInputEvents(E);
                                }
                            }
                        }
                    }
                }
                SDL_FillRect(WSurface, nullptr, SDL_MapRGB(WSurface->format, 0xB0, 0xB0, 0xB0)); //Draw background
                ALL_SCENES //Draw all scenes
                {
                    scene->Draw(WSurface); 
                }
                SDL_UpdateWindowSurface(Window);
                CleanUpMarkedScenes(); //Destroy scenes marked for destruction this frame
                
                if(GameStateDeferred.first)//Change game state at end of game loop
                {
                    SetGameState(GameStateDeferred.second);
                }
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
    ALL_SCENES
    {
        scene->ClearResources();
        delete scene;
        scene = nullptr;
    }
    sceneStack.clear();
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void SetGameState(GameState NewState, Difficulty NewDifficulty)
{
    GameStateDeferred.first = false;
    switch(NewState)
    {
    case GameState::MAIN_MENU:
        {

            if(!sceneStack.empty())
            {
                ALL_SCENES
                {
                    scene->bShouldDestroy = true;//Mark all scenes for destruction
                }
            }
            
            Menu* MainMenu = new Menu;
            //194 x 53 is the Button image size. Could make a function to make this more modular later
           Vector2 CenterPos = GetCenterPosition({194,53});
            
            MainMenu->AddButton(Vector2(CenterPos.x,CenterPos.y - 60), "Resources/Title/Title_Easy.png", []()
                {
                    SetGameState(GameState::IN_PROGRESS, Difficulty::EASY);                
            });
            MainMenu->AddButton(Vector2(CenterPos.x,CenterPos.y), "Resources/Title/Title_Medium.png", []()
                 {
                     SetGameState(GameState::IN_PROGRESS, Difficulty::MEDIUM);
                 });
            MainMenu->AddButton(Vector2(CenterPos.x,CenterPos.y + 60), "Resources/Title/Title_Hard.png", []()
                  {
                      SetGameState(GameState::IN_PROGRESS, Difficulty::HARD);                
              });
            MainMenu->AddButton(Vector2(CenterPos.x, CenterPos.y + 150), "Resources/Title/Title_Quit.png", []()
              {
                  SetGameState(GameState::GAME_QUIT);                
            });
            sceneStack.push_back(MainMenu);
            break;
        }
    case GameState::IN_PROGRESS:
        {
            
            sceneStack.back()->bShouldDestroy = true; //Mark main menu for destruction
            sceneStack.back()->bReceiveInput = false;
            
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
            NewMap->Initialize();
            

            NumberDisplay* Display = new NumberDisplay(DIFFICULTY, 3);
            Display->Initialize();
            Vector2 Pos =  NewMap->Position - Vector2(TILE_SIZE, TILE_SIZE);
            Pos.y -= Display->GetFontSize().Height();
            Display->Position = Pos;

            using std::placeholders::_1;
            NewMap->OnFlag = std::bind(&NumberDisplay::UpdateNumber, Display, _1);

            Timer = new GameTimer();
            Timer->Initialize();
            Vector2 Pos2 = NewMap->Position - Vector2(TILE_SIZE, TILE_SIZE);
            Pos2.y -= Timer->GetFontSize().Height();
            Pos2.x += (Width + 2) * TILE_SIZE - Timer->GetFontSize().Width() * 5;
            Timer->Position = Pos2;

            sceneStack.push_back(NewMap);
            sceneStack.push_back(Display);
            sceneStack.push_back(Timer);

            Timer->StartTimer();

            break;
        }
    case GameState::GAME_OVER:
        {
            Timer->StopTimer();
            Menu* EndMenu = new Menu;
            Vector2 CenterPos = GetCenterPosition({194,53});
            EndMenu->AddButton(Vector2(CenterPos.x, CenterPos.y), "Resources/Title/Title_Menu.png", []()
               {
                   SetGameState(GameState::MAIN_MENU);
           });
            EndMenu->AddButton(Vector2(CenterPos.x, CenterPos.y + 60), "Resources/Title/Title_Quit.png", []()
               {
                   SetGameState(GameState::GAME_QUIT);                
           });
            sceneStack.push_back(EndMenu);
            break;
        }
    case GameState::GAME_QUIT: break;
    }
    CurrentState = NewState;
}




bool EndGame(bool GameWon)
{
   ALL_SCENES{scene->bReceiveInput = false;} //Stop handling input for everything 
    
    const char* ImagePath = GameWon ? "Resources/Title/Title_Win.png" : "Resources/Title/Title_Lose.png";
    Image* GameHeading = new Image(ImagePath);
    GameHeading->Initialize();
    Vector2 Dim = GameHeading->GetSceneDimensions();
    Vector2 Pos = GetCenterPosition(Dim);
    Pos.y -= 200;
    GameHeading->Position = Pos;

    sceneStack.push_back(GameHeading);    

    SetGameStateDeferred(GameState::GAME_OVER);
    return false;
}

void CleanUpMarkedScenes()
{
    for(int i = sceneStack.size() - 1; i > -1; i--)
    {
        Scene* scene = sceneStack[i];
        if(scene->bShouldDestroy)
        {
            scene->ClearResources();
            delete scene;
            sceneStack.erase(sceneStack.begin() + i);
            std::cout << "Scene Destroyed at index : " << i << std::endl;
        }
    }
}

Vector2 GetCenterPosition(Vector2 Size)
{
    int x = (SCREEN_WIDTH - Size.x) / 2; 
    int y = (SCREEN_HEIGHT - Size.y) / 2;
    return {x ,y};
}