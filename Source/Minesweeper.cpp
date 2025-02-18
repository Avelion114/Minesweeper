#include <iostream>
#include <SDL2/SDL.h>


int main(int argc, char* argv[])
{
    //Declare null window pointer
    SDL_Window* Window = nullptr;

    //Try init video, if unsucessful print error message
    if(SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        
        //Create window
        Window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,  SDL_WINDOW_SHOWN);
        //Ensure window was created
        if(Window)
        {
            //Update window surface to actually see the window
            SDL_UpdateWindowSurface(Window);
            //Wait 2 seconds
            SDL_Delay(2000);
        }
        else
        {
            std::cout << "Failed to create SDL window: " << SDL_GetError() << "\n";
        }
    }
    else
    {
        std::cout << "SDL_Init_Video Error: " << SDL_GetError() << "\n";
    }

    //Destroy window
    //Quit SDL
    SDL_DestroyWindow(Window);
    SDL_Quit();    
  
    return 0;
}