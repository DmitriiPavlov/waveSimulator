#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <iostream>
#include "physics.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500
#define DELTA_T 0.01f
struct EVENT_FLAGS{
    bool MOVE_UP = false;
    bool MOVE_DOWN = false;
    bool MOVE_LEFT = false;
    bool MOVE_RIGHT = false;
};

void drawWaveLine(SDL_Renderer * renderer, const WaveLine & wave){
    SDL_Point * point_array = new SDL_Point[wave.points.size()];
    for (int i = 0; i < wave.points.size(); i++){
        point_array[i] = SDL_Point{i,(int) wave.points[i].y + SCREEN_HEIGHT/2};
    }
    SDL_RenderDrawLines(renderer, point_array, wave.points.size());
}

int main(int argc, char *argv[])
{

    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_Window* window = SDL_CreateWindow("GAME",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer * renderer = SDL_CreateRenderer(window,0,0);



    bool quit = false;
    SDL_Event e;
    EVENT_FLAGS eventFlags;

    //timer variables
    Uint64 timerStart, timerEnd;
    float elapsedTime, deltaTime;

    //temporary house for square
    SDL_Rect r = {100,100,50,50};
    float rx = 100.0f;


    WaveLine wave = WaveLine(1.0f,5.0f,0.1f,SCREEN_WIDTH);
    timerEnd = SDL_GetPerformanceCounter();
    while (!quit){
        //start timer
        timerStart = SDL_GetPerformanceCounter();

        //poll for events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                wave.impulse(e.button.x,-500.0f,10);
            }
            //handle keyboard press
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_w | e.key.keysym.sym == SDLK_UP) {
                    eventFlags.MOVE_UP = true;
                }
                if (e.key.keysym.sym == SDLK_d | e.key.keysym.sym == SDLK_RIGHT) {
                    eventFlags.MOVE_RIGHT = true;
                }
                if (e.key.keysym.sym == SDLK_a | e.key.keysym.sym == SDLK_LEFT) {
                    eventFlags.MOVE_LEFT = true;
                }
                if (e.key.keysym.sym == SDLK_s | e.key.keysym.sym == SDLK_DOWN) {
                    eventFlags.MOVE_DOWN = true;
                }
            }
            //handle keyboard releases
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_w | e.key.keysym.sym == SDLK_UP) {
                    eventFlags.MOVE_UP = false;
                }
                if (e.key.keysym.sym == SDLK_d | e.key.keysym.sym == SDLK_RIGHT) {
                    eventFlags.MOVE_RIGHT = false;
                }
                if (e.key.keysym.sym == SDLK_a | e.key.keysym.sym == SDLK_LEFT) {
                    eventFlags.MOVE_LEFT = false;
                }
                if (e.key.keysym.sym == SDLK_s | e.key.keysym.sym == SDLK_DOWN) {
                    eventFlags.MOVE_DOWN = false;
                }
            }
        }
        //drawing part of the loop
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,100,10,100,100);
        drawWaveLine(renderer,wave);

        SDL_RenderPresent(renderer);

        timerEnd = SDL_GetPerformanceCounter();


        elapsedTime = (timerEnd - timerStart)/(float)SDL_GetPerformanceFrequency() * 1000.0F;

        std::cout<<(1/(float)elapsedTime)<<" ";


        elapsedTime = (timerEnd - timerStart)/(float)SDL_GetPerformanceFrequency();
        deltaTime = 50*elapsedTime;
//        physics update part of the loop
        while (deltaTime >= DELTA_T){
            deltaTime-=DELTA_T;
            wave.tickTime(DELTA_T);
        }
        wave.tickTime(deltaTime);

        //mandatory time reset
        timerStart = timerEnd;



    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
