#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <chrono>
#include <iostream>
#include <map>

SDL_Window * init(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char name[50])
{
    bool success = true;
    SDL_Surface * screensurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Window* window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("no window! %s\n", SDL_GetError());
        }
        else
        {
            return window;
        }
    }
    return NULL;
}

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const int mask[8][2] = {{0,1},{-1,0},{1,0},{0,-1}, {1,1},{-1,-1},{1,-1},{-1,1}};

int genimage(bool * pixelarray, int arraylength) {
    FILE *imagebuffer;
    int x, y, pixel, height=SCREEN_HEIGHT, width=SCREEN_WIDTH;  
    imagebuffer = fopen("image.ppm", "wb");
    if (imagebuffer == NULL) {
        printf("ERROR: Cannot open output file");
        exit(EXIT_FAILURE);
    }
    fprintf(imagebuffer, "P6\n");
    fprintf(imagebuffer, "%d %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    fprintf(imagebuffer, "1\n");

    int imagelength = arraylength * 3;
    
    bool imagearray[imagelength];
    for (int i = 0; i < imagelength; i += 3) 
    {
        imagearray[i] = pixelarray[i/3];
        imagearray[i+1] = 0;
        imagearray[i+2] = 0;
    }
    fwrite(imagearray, 1, imagelength, imagebuffer);
    fclose(imagebuffer);

}

SDL_Surface * load_media(const char imagePath[50])
{
    SDL_Surface * image = IMG_Load( imagePath );
    if ( image == NULL )
    {
        printf( "unable to load image! %s\n", IMG_GetError() );
        return NULL;
    }
    else
    {
        return image;
    }
}


int main(int argc, char* args[])
{
    int x, y;
    Uint32 buttons;

    bool quit = false;
    int arraysize = SCREEN_WIDTH * SCREEN_HEIGHT;
    bool mainarray[SCREEN_WIDTH][SCREEN_HEIGHT];
    bool bufferarray[SCREEN_WIDTH][SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            mainarray[i][j] = rand() % 2;
            bufferarray[i][j] = 0;
        }
    }
   
    SDL_Event e;
    SDL_Window * window = init(SCREEN_WIDTH, SCREEN_HEIGHT, "game of ife");
    SDL_Surface * screensurface = SDL_GetWindowSurface(window);
    SDL_Surface * loadimage = load_media("image.ppm");
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    break;
            }
            buttons = SDL_GetMouseState(&x, &y);
            if ((buttons & SDL_BUTTON_LMASK) != 0) {
            for (int k = 0; k < 8; k++) {
                bufferarray[x+mask[k][0]][y+mask[k][1]] = 1;
            }
            printf("%d, %d", x, y);
        }


        }

// Calculation loop //
        for (int i = 1; i < SCREEN_WIDTH + 1; i++) {
            for (int j = 1; j < SCREEN_HEIGHT + 1; j++) {
                int adjcellcount = 0;
                for (int k = 0; k < 8; k++) {
                    adjcellcount += mainarray[i+mask[k][0]][j+mask[k][1]];
                }
                if (mainarray[i][j] == 1) {
                    if (adjcellcount == 2 || adjcellcount == 3) {
                        bufferarray[i][j] = 1;
                    } else {
                        bufferarray[i][j] = 0;
                    }
                } else {
                    if (adjcellcount == 3) {
                        bufferarray[i][j] = 1;
                    } else {
                        bufferarray[i][j] = 0;
                    }
                }
            }
        } 
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            for (int j = 0; j < SCREEN_HEIGHT; j++) {
                mainarray[i][j] = bufferarray[i][j];
                bufferarray[i][j] = 0;
            }
        }
//////////////////////
        

        
    
        bool mainarray1d[arraysize];
        
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                mainarray1d[i* SCREEN_WIDTH + j] = mainarray[i][j];
            }
        }

        genimage(mainarray1d, arraysize);
        loadimage = load_media("image.ppm");
        SDL_BlitSurface(loadimage, NULL, screensurface, NULL);
        SDL_UpdateWindowSurface(window);
        SDL_FreeSurface(loadimage);
    }
    SDL_DestroyWindow( window );
    SDL_Quit();
    //for (int i = 0; i < arraysize; i++) {
    //    std::cout << mainarray1d[i];
    //}
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            std::cout << mainarray[i][j];
        }
    }
    return 0;
}



