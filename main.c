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

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 256;


//QR world square
//const int mask[4][2] = {{0,1},{-1,0},{1,0},{0,-1}};

//QR world diagonal
//const int mask[4][2] = {{1,1},{-1,-1},{1,-1},{-1,1}};

//Conways Game of Life
const int mask[8][2] = {{1,1},{-1,-1},{1,-1},{-1,1},{0,1},{-1,0},{1,0},{0,-1}};

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

    const size_t imagelength = 3 * SCREEN_WIDTH * SCREEN_HEIGHT * 2;
    
    bool imagearray[imagelength];
    for (int i = 0; i < imagelength; i += 3) 
    {
        imagearray[i] = pixelarray[i/3];
        imagearray[i+1] = pixelarray[i/3];
        imagearray[i+2] = pixelarray[i/3];
    }
    fwrite(imagearray, 2, imagelength, imagebuffer);
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
    setvbuf (stdout, NULL, _IONBF, 0);
    fflush(stdout);
    int x, y;
    int brushwidth = 8;
    int brushheight = 8;
    bool quit = false;
    bool pause = false;
    bool mousevar = false;
    const int arraysize = 307200;
    bool mainarray[SCREEN_WIDTH][SCREEN_HEIGHT];
    bool bufferarray[SCREEN_WIDTH][SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            mainarray[i][j] = 0;
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
                    switch(e.key.keysym.sym) {
                        case SDLK_SPACE:
                            pause = pause - 1;
                            break;
                        case SDLK_EQUALS:
                            brushwidth++;
                            brushheight++;
                            break;
                        case SDLK_MINUS:
                            brushwidth--;
                            brushheight--;
                            break;
			case SDLK_ESCAPE:
			    quit = true;
			    break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mousevar = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mousevar = false;
                    break;
            }
        }
        
        SDL_GetMouseState( &x, &y );
        if (mousevar == true) {
            for (int i = 0; i < brushwidth; i++) {
                for (int j = 0; j < brushheight; j++) {
                    mainarray[x+i-(brushwidth/2)][y+j-(brushheight/2)] = true;
                }
            }
        }
        if(!pause) {
            for (int i = 1; i < SCREEN_WIDTH + 1; i++) {
                for (int j = 1; j < SCREEN_HEIGHT + 1; j++) {
                    int adjcellcount = 0;
                    for (int k = 0; k < masklength; k++) {
                        adjcellcount += mainarray[i+mask[k][0]][j+mask[k][1]];
                    }
                    if (mainarray[i][j] == 1) {
                        if (adjcellcount == 2 || adjcellcount == 3) {
                            bufferarray[i][j] = 1;
                        } else {
                            bufferarray[i][j] = 0;
                        }
                    } else {
                        //uncomment || adjcellcount == 4 if u ar using qr rules
                        if (adjcellcount == 3/* || adjcellcount == 4*/) {
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
        }

        
    
        bool mainarray1d[arraysize];
        
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                mainarray1d[i* SCREEN_WIDTH + j] = mainarray[j][i];
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
    //for (int i = 0; i < SCREEN_WIDTH; i++) {
    //    for (int j = 0; j < SCREEN_HEIGHT; j++) {
    //        std::cout << mainarray[i][j];
    //    }
    //}
    return 0;
}



