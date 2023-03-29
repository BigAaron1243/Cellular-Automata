#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>


SDL_Window * init(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char name[50])
{
    bool success = true;
    SDL_Surface * screensurface = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
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

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//QR world square
//const int mask[8] = {0,1,-1,0,1,0,0,-1};

//QR world diagonal
//const int mask[8] = {1,1,-1,-1,1,-1,-1,1};
//const int rules[10] = {0, 0, 0, 0, 1, 1, 0, 1, 1, 0};

//Conways Game of Life
int mask[16] = {1,1,-1,-1,1,-1,-1,1,0,1,-1,0,1,0,0,-1};
bool rules[18] = {0,0, 0,0, 0,1, 1,1, 0,0, 0,0, 0,0, 0,0, 0,0};

//Maze
//const int mask[16] = {1,1,-1,-1,1,-1,-1,1,0,1,-1,0,1,0,0,-1};
//const bool rules[18] = {0,0, 0,1, 0,1, 1,1, 0,1, 0,1, 0,0, 0,0, 0,0};

//Wall
//const int mask[16] = {1,1,-1,-1,1,-1,-1,1,0,1,-1,0,1,0,0,-1};
//const bool rules[18] = {0,0, 0,0, 0,1, 0,1, 1,1, 1,1, 1,0, 1,0, 1,0};

const int masklength = 8;

void genimage(bool * pixelarray, int arraylength) {
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

    const size_t imagelength = 3 * arraylength;
    
    bool* imagearray = (bool*)malloc(imagelength * sizeof(bool));
    for (int i = 0; i < imagelength; i += 3) 
    {
        imagearray[i] = pixelarray[i/3];
        imagearray[i+1] = pixelarray[i/3];
        imagearray[i+2] = pixelarray[i/3];
    }
    fwrite(imagearray, 1, imagelength, imagebuffer);
    fclose(imagebuffer);
	//system("pnmtojpeg image.ppm > result.jpg");
}
bool * step(bool * mainarray, bool * rules, int * mask, int arraysize) {

    bool* marray = (bool*)malloc(sizeof(bool) * arraysize);
    bool* barray = (bool*)malloc(sizeof(bool) * arraysize);
    for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
        for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
            int adjcellcount = 0;
            for (int k = 0; k < masklength; k++) {
                adjcellcount += mainarray[(i + mask[2 * k]) * SCREEN_HEIGHT + j + mask[2 * k + 1]];
            }
            barray[i * SCREEN_HEIGHT + j] = rules[2 * adjcellcount + mainarray[i * SCREEN_HEIGHT + j]];
        }
    } 
    
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            marray[i * SCREEN_HEIGHT + j] = barray[i * SCREEN_HEIGHT + j];
            barray[i * SCREEN_HEIGHT +j] = 0;
        }
    }
    return marray;
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
    setvbuf (stdout, NULL, _IONBF, 0);
    fflush(stdout);
    int x, y;
    int brushwidth = 8;
    int brushheight = 8;
    bool quit = false;
    bool pause = false;
    bool mousevar = false;
    const int arraysize = SCREEN_WIDTH * SCREEN_HEIGHT;
    bool* mainarray = (bool*)malloc(sizeof(bool) * arraysize);
    bool* bufferarray = (bool*)malloc(sizeof(bool) * arraysize);
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            mainarray[i * SCREEN_HEIGHT + j] = 0;
            bufferarray[i * SCREEN_HEIGHT + j] = 0;
        }
    }
    SDL_Event e;
    SDL_Window * window = init(SCREEN_WIDTH, SCREEN_HEIGHT, "game of ife");
    SDL_Surface * screensurface = SDL_GetWindowSurface(window);
    SDL_Surface * loadimage = load_media("image.ppm");
    int whichbutton = 0;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                    
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_SPACE:
                            pause = pause - 1;
                            break;
                        case SDLK_EQUALS:
                            brushwidth++;
                            brushheight++;
                            break;
                        case SDLK_MINUS:
                            if (brushwidth > 0) {brushwidth--;}
                            if (brushheight > 0) {brushheight--;}
                            break;
                        case SDLK_i:
                    
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    }
                    break;
                case SDL_TEXTINPUT:
                    printf(e.text.text);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mousevar = true;
                    if (SDL_BUTTON_LEFT == e.button.button) { 
                        whichbutton = 1;
                    } else {
                        whichbutton = 2;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    mousevar = false;
                    break;
            }
        }
        
        SDL_GetMouseState( &x, &y );
        if (mousevar == true && x < SCREEN_WIDTH - brushwidth/2 && x > 0 + brushwidth/2 && y < SCREEN_HEIGHT - brushheight/2 && y > 0 + brushwidth/2) {
            for (int i = 0; i < brushwidth; i++) {
                for (int j = 0; j < brushheight; j++) {
                    if (whichbutton == 1) {
                        mainarray[(x+i-(brushwidth/2)) * SCREEN_HEIGHT + y + (j-(brushheight/2))] = true;
                    } else {
                    
                        mainarray[(x+i-(brushwidth/2)) * SCREEN_HEIGHT + y + (j-(brushheight/2))] = false;
                    }
                }
            }
        }
        if(!pause) {
            mainarray = step(mainarray, rules, mask, arraysize);
            
            /*for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
                for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
                    int adjcellcount = 0;
                    for (int k = 0; k < masklength; k++) {
                        adjcellcount += mainarray[(i + mask[2 * k]) * SCREEN_HEIGHT + j + mask[2 * k + 1]];
                    }
                    bufferarray[i * SCREEN_HEIGHT + j] = rules[2 * adjcellcount + mainarray[i * SCREEN_HEIGHT + j]];
                }
            } 
            
            for (int i = 0; i < SCREEN_WIDTH; i++) {
                for (int j = 0; j < SCREEN_HEIGHT; j++) {
                    mainarray[i * SCREEN_HEIGHT + j] = bufferarray[i * SCREEN_HEIGHT + j];
                    bufferarray[i * SCREEN_HEIGHT +j] = 0;
                }
            }
            */
        }

        
    
        bool* mainarray1d = (bool*)malloc(arraysize * sizeof(bool));
        
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                mainarray1d[i* SCREEN_WIDTH + j] = mainarray[j * SCREEN_HEIGHT + i];
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

    return 0;
}



