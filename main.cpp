#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <iostream>
#include "physics.h"
#define SCREEN_WIDTH 1400
#define SCREEN_HEIGHT 700
#define DELTA_T 0.2f
#define RESOLUTION 4
#define BLEND_SIZE 8
#define BLENDING false
struct EVENT_FLAGS{
    bool MOVE_UP = false;
    bool MOVE_DOWN = false;
    bool MOVE_LEFT = false;
    bool MOVE_RIGHT = false;
};

void drawWaveLine(SDL_Renderer * renderer, const WaveLine & wave){
    SDL_Point * point_array = new SDL_Point[wave.points.size()];
    for (int i = 0; i < wave.points.size(); i++){
        point_array[i] = SDL_Point{i,(int) wave.y[2*i] + SCREEN_HEIGHT/2};
    }
    SDL_RenderDrawLines(renderer, point_array, wave.points.size());
}

int round(int x){
    return (x/RESOLUTION);
}
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void drawWave2D(SDL_Renderer * renderer, Wave2D & wave){
    //we will start off by just adding 128 to every y value
    int height;
    SDL_Rect blendRect{0,0,RESOLUTION+2*BLEND_SIZE,RESOLUTION+2*BLEND_SIZE};
    SDL_Rect rect{0,0,RESOLUTION,RESOLUTION};
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for (int x = 0; x<wave.w/2; x++){
        for (int y = 0; y<wave.h; y++){
            height = sgn<float>(wave.getX(wave.y,x,y))*sqrt(abs(wave.getX(wave.y,x,y))/128.0f) * 128.0f/2;

            SDL_SetRenderDrawColor(renderer,0, 128+height,128+height , 255);
            rect.x = x*RESOLUTION; rect.y = y*RESOLUTION;
            SDL_RenderFillRect(renderer,&rect);
            if (BLENDING) {
            SDL_SetRenderDrawColor(renderer,0, 0,128+height , 10);
            blendRect.x = x*RESOLUTION-BLEND_SIZE; blendRect.y = y*RESOLUTION-BLEND_SIZE;
            SDL_RenderFillRect(renderer,&blendRect);
            }
        }
    }
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
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    bool quit = false;
    SDL_Event e;
    EVENT_FLAGS eventFlags;

    //timer variables
    Uint64 timerStart, timerEnd;
    float elapsedTime, deltaTime;

    //temporary house for square
    SDL_Rect r = {100,100,50,50};
    float rx = 100.0f;


//    WaveLine wave = WaveLine(0.2f,20.0f,0.05f,SCREEN_WIDTH);
    Wave2D wave2D = Wave2D(1.0f,20.0f,0.2f,SCREEN_WIDTH/RESOLUTION, SCREEN_HEIGHT/RESOLUTION);
    timerEnd = SDL_GetPerformanceCounter();

    while (!quit){
        //start timer
        timerStart = SDL_GetPerformanceCounter();

        //poll for events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (SDL_GetMouseState(nullptr,nullptr) & SDL_BUTTON_LMASK){
//                wave.newImpulse(e.button.x,-500.0f,10);
                  wave2D.impulse(round(e.button.x),round(e.button.y),7000.0f,1.0f);
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
        SDL_SetRenderDrawColor(renderer,0,0,128,128);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,100,10,100,100);
//        drawWaveLine(renderer,wave);
        drawWave2D(renderer,wave2D);
        SDL_RenderPresent(renderer);

        timerEnd = SDL_GetPerformanceCounter();


        elapsedTime = (timerEnd - timerStart)/(float)SDL_GetPerformanceFrequency() * 1000.0F;

//        std::cout<<(1000.0f/(float)elapsedTime)<<" ";
        std::cout.flush();


        elapsedTime = (timerEnd - timerStart)/(float)SDL_GetPerformanceFrequency();
        deltaTime = 5*elapsedTime;
//        physics update part of the loop
        while (deltaTime >= DELTA_T){
            deltaTime-=DELTA_T;
//            wave.tickTimeRK4(DELTA_T);
            wave2D.tickRK4(DELTA_T);
        }
//        wave.tickTime(deltaTime);
        wave2D.tickRK4(deltaTime);

        //mandatory time reset
        timerStart = timerEnd;



    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
