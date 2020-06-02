
//Using libs SDL, glibc
#include "SDL2/SDL.h"    //SDL version 2.0
#include <cstdio>

int SCREEN_WIDTH = 640;    //window height
int SCREEN_HEIGHT = 480;   //window width

//function prototypes
//initilise SDL
int init(int w, int h, int argc, char *args[]);

typedef struct ball_s {

    int x, y; /* position on the screen */
    int w, h; // ball width and height
    int dx, dy; /* movement vector */
} ball_t;

typedef struct paddle {

    int x, y;
    int w, h;
    int dx;
} paddle_t;

// Program globals
static ball_t ball;
static paddle_t paddle[2];
int score[] = {0, 0};
int width, height;        //used if fullscreen

SDL_Window *window = NULL;    //The window we'll be rendering to
SDL_Renderer *renderer;        //The renderer SDL will use to draw to the screen

//surfaces
static SDL_Surface *screen;
static SDL_Surface *title;
static SDL_Surface *numbermap;
static SDL_Surface *end;

//textures
SDL_Texture *screen_texture;

//initialize starting position and sizes of game elements
static void init_game() {

    ball.x = screen->w / 2;
    ball.y = screen->h / 2;
    ball.w = 10;
    ball.h = 10;
    ball.dy = 1;
    ball.dx = 1 * (screen->w / 480);

    paddle[0].x = 15;
    paddle[0].y = screen->h / 2 - 50;
    paddle[0].w = 5;
    paddle[0].h = 50;
    paddle[0].dx = 0;

    paddle[1].x = screen->w - 15 - 10;
    paddle[1].y = screen->h / 2 - 50;
    paddle[1].w = 5;
    paddle[1].h = 50;
    paddle[1].dx = 0;
}

int check_score() {

    int i;

    //loop through player scores
    for (i = 0; i < 2; i++) {

        //check if score is @ the score win limit
        if (score[i] == 10) {

            //reset scores
            score[0] = 0;
            score[1] = 0;

            //return 1 if player 1 score @ limit
            if (i == 0) {

                return 1;

                //return 2 if player 2 score is @ limit
            } else {

                return 2;
            }
        }
    }

    //return 0 if no one has reached a score of 10 yet
    return 0;
}

//if return value is 1 collision occurred. if return is 0, no collision.
int check_collision(ball_t ball, paddle_t paddle) {

    int ballLeft, paddleLeft;
    int ballRight, paddleRight;
    int ballTop, paddleTop;
    int ballBottom, paddleBottom;

    ballLeft = ball.x;
    ballRight = ball.x + ball.w;
    ballTop = ball.y;
    ballBottom = ball.y + ball.h;

    paddleLeft = paddle.x;
    paddleRight = paddle.x + paddle.w;
    paddleTop = paddle.y;
    paddleBottom = paddle.y + paddle.h;


    if (ballLeft > paddleRight) {
        return 0;
    }

    if (ballRight < paddleLeft) {
        return 0;
    }

    if (ballTop > paddleBottom) {
        return 0;
    }

    if (ballBottom < paddleTop) {
        return 0;
    }

    return 1;
}

/* This routine moves each ball by its motion vector. */
static void move_ball() {
    if (ball.dx > 10)
        ball.dx = 10;

    /* Move the ball by its motion vector. */
    ball.x += ball.dx;
    ball.y += ball.dy;

    /* the ball around if it hits the edge of the screen. */
    if (ball.x < 0) {

        score[1] += 1;
        init_game();
    }

    if (ball.x > screen->w - 10) {

        score[0] += 1;
        init_game();
    }

    if (ball.y < 0 || ball.y > screen->h - 10) {

        ball.dy = -ball.dy;
    }

    //check for collision with the paddle
    int i;

    for (i = 0; i < 2; i++) {

        int c = check_collision(ball, paddle[i]);

        //collision detected
        if (c == 1) {
            //ball on left side
            if (ball.x < screen->w / 2) {
                ball.dx -= 1;
                ball.x = paddle[0].x + paddle[0].w + 1;
                ball.dx *= -1;
                //ball on right side and paddle hitting head-on
            } else if (ball.x < paddle[1].x + (paddle[1].w / 2)) {
                ball.dx *= ball.dx > 0 ? -1 : 1;
                ball.dx += paddle[1].dx - 1;
                ball.x = paddle[1].x - ball.w - 1 + paddle[1].dx;
            } else {
                ball.dx *= ball.dx < 0 ? -1 : 1;
                ball.dx += paddle[1].dx + 1;
                ball.x = paddle[1].x + paddle[1].w + paddle[1].dx + 1;
            }

            //change ball angle based on where on the paddle it hit
            int hit_pos = (paddle[i].y + paddle[i].h) - ball.y;

            if (hit_pos >= 0 && hit_pos < 7) {
                ball.dy = 4;
            } else if (hit_pos >= 7 && hit_pos < 14) {
                ball.dy = 3;
            } else if (hit_pos >= 14 && hit_pos < 21) {
                ball.dy = 2;
            } else if (hit_pos >= 21 && hit_pos < 28) {
                ball.dy = 1;
            } else if (hit_pos >= 28 && hit_pos < 32) {
                ball.dy = 0;
            } else if (hit_pos >= 32 && hit_pos < 39) {
                ball.dy = -1;
            } else if (hit_pos >= 39 && hit_pos < 46) {
                ball.dy = -2;
            } else if (hit_pos >= 46 && hit_pos < 53) {
                ball.dy = -3;
            } else if (hit_pos >= 53 && hit_pos <= 60) {
                ball.dy = -4;
            }
        }
    }
}

static void move_paddle_ai() {

    int center = paddle[0].y + 25;
    int screen_center = screen->h / 2 - 25;
    int ball_speed = ball.dy;

    if (ball_speed < 0) {

        ball_speed = -ball_speed;
    }

    //ball moving right
    if (ball.dx > 0) {

        //return to center position
        if (center < screen_center) {

            paddle[0].y += ball_speed;

        } else {

            paddle[0].y -= ball_speed;
        }

        //ball moving left
    } else {

        //ball moving down
        if (ball.dy > 0) {

            if (ball.y > center) {

                paddle[0].y += ball_speed;

            } else {

                paddle[0].y -= ball_speed;
            }
        }

        //ball moving up
        if (ball.dy < 0) {

            if (ball.y < center) {

                paddle[0].y -= ball_speed;

            } else {

                paddle[0].y += ball_speed;
            }
        }

        //ball moving stright across
        if (ball.dy == 0) {

            if (ball.y < center) {

                paddle[0].y -= 5;

            } else {

                paddle[0].y += 5;
            }
        }
    }
}

static void move_paddle(int d, int l) {
    switch (d) {
        case 1:
            // if the down arrow is pressed move paddle down
            if (paddle[1].y >= screen->h - paddle[1].h)
                paddle[1].y = screen->h - paddle[1].h;
            else
                paddle[1].y += 5;
            break;
        case -1:
            // if the up arrow is pressed move paddle down
            if (paddle[1].y <= 0)
                paddle[1].y = 0;
            else
                paddle[1].y -= 5;
        default:
            break;
    }
    switch (l) {
        case -1:
            // if the right arrow is pressed move paddle right
            if (paddle[1].x >= screen->w - paddle[1].w - 10) {
                paddle[1].x = screen->w - paddle[1].w - 10;
                paddle[1].dx = 0;
            } else {
                paddle[1].x += 5;
                paddle[1].dx = 5;
            }
            break;
        case 1:
            // if the left arrow is pressed move paddle left
            if (paddle[1].x <= screen->w / 2 + 10) {
                paddle[1].x = screen->w / 2 + 10;
                paddle[1].dx = 0;
            } else {
                paddle[1].x -= 5;
                paddle[1].dx = -5;
            }
            break;
        default:
            paddle[1].dx = 0;
            break;
    }
}

static void draw_game_over(int p) {

    SDL_Rect p1;
    SDL_Rect p2;
    SDL_Rect cpu;
    SDL_Rect dest;

    p1.x = 0;
    p1.y = 0;
    p1.w = end->w;
    p1.h = 75;

    p2.x = 0;
    p2.y = 75;
    p2.w = end->w;
    p2.h = 75;

    cpu.x = 0;
    cpu.y = 150;
    cpu.w = end->w;
    cpu.h = 75;

    dest.x = (screen->w / 2) - (end->w / 2);
    dest.y = (screen->h / 2) - (75 / 2);
    dest.w = end->w;
    dest.h = 75;


    switch (p) {

        case 1:
            SDL_BlitSurface(end, &p1, screen, &dest);
            break;
        case 2:
            SDL_BlitSurface(end, &p2, screen, &dest);
            break;
        default:
            SDL_BlitSurface(end, &cpu, screen, &dest);
    }
}

static void draw_menu() {

    SDL_Rect src;
    SDL_Rect dest;

    src.x = 0;
    src.y = 0;
    src.w = title->w;
    src.h = title->h;

    dest.x = (screen->w / 2) - (src.w / 2);
    dest.y = (screen->h / 2) - (src.h / 2);
    dest.w = title->w;
    dest.h = title->h;

    SDL_BlitSurface(title, &src, screen, &dest);
}

static void draw_background() {

    SDL_Rect src;

    //draw bg with net
    src.x = 0;
    src.y = 0;
    src.w = screen->w;
    src.h = screen->h;

    //draw the backgorund
    //int r = SDL_FillRect(screen,&src,0);

    //if (r !=0){

    //	printf("fill rectangle faliled in func draw_background()");
    //}
}

static void draw_net() {

    SDL_Rect net;

    net.w = 3;
    net.h = 30;
    net.x = screen->w / 2 - (net.w / 2);
    net.y = -5;

    //draw the net
    int i, r;

    while (net.y < screen->h) {

        r = SDL_FillRect(screen, &net, SDL_MapRGB(screen->format, 127, 185, 4));

        if (r != 0) {

            printf("fill rectangle faliled in func draw_net()");
        }

        net.y = net.y + net.h + 8;
    }
}

static void draw_ball() {

    SDL_Rect src;

    src.x = ball.x;
    src.y = ball.y;
    src.w = ball.w;
    src.h = ball.h;

    int r = SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 225, 225, 225));

    if (r != 0) {

        printf("fill rectangle faliled in func drawball()");
    }
}

static void draw_paddle() {

    SDL_Rect src;
    int i;

    for (i = 0; i < 2; i++) {

        src.x = paddle[i].x;
        src.y = paddle[i].y;
        src.w = paddle[i].w;
        src.h = paddle[i].h;

        int r = SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 255, 255, 255));

        if (r != 0) {

            printf("fill rectangle faliled in func draw_paddle()");
        }
    }
}

static void draw_player_0_score() {

    SDL_Rect src;
    SDL_Rect dest;

    src.x = 0;
    src.y = 0;
    src.w = 64;
    src.h = 64;

    dest.x = (screen->w / 2) - src.w - 12; //12 is just padding spacing
    dest.y = 0;
    dest.w = 64;
    dest.h = 64;

    if (score[0] > 0 && score[0] < 10) {

        src.x += src.w * score[0];
    }

    SDL_BlitSurface(numbermap, &src, screen, &dest);
}

static void draw_player_1_score() {

    SDL_Rect src;
    SDL_Rect dest;

    src.x = 0;
    src.y = 0;
    src.w = 64;
    src.h = 64;

    dest.x = (screen->w / 2) + 12;
    dest.y = 0;
    dest.w = 64;
    dest.h = 64;

    if (score[1] > 0 && score[1] < 10) {

        src.x += src.w * score[1];
    }

    SDL_BlitSurface(numbermap, &src, screen, &dest);
}

int main(int argc, char *args[]) {

    //SDL Window setup
    if (init(SCREEN_WIDTH, SCREEN_HEIGHT, argc, args) == 1) {

        return 0;
    }

    SDL_GetWindowSize(window, &width, &height);

    int sleep = 0;
    int quit = 0;
    int state = 0;
    int r = 0;
    Uint32 next_game_tick = SDL_GetTicks();

    // Initialize the ball position data.
    init_game();

    //render loop
    while (quit == 0) {

        //check for new events every frame
        SDL_PumpEvents();

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        int d = keystate[SDL_SCANCODE_DOWN] - keystate[SDL_SCANCODE_UP],
                l = keystate[SDL_SCANCODE_LEFT] - keystate[SDL_SCANCODE_RIGHT];

        if (keystate[SDL_SCANCODE_ESCAPE]) {
            quit = 1;
        }

        if (d || l) {
            move_paddle(d, l);
        } else {
            paddle[1].dx = 0;
        }


        //draw background
        SDL_RenderClear(renderer);
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 61, 51, 51));

        //display main menu
        if (state == 0) {

            if (keystate[SDL_SCANCODE_SPACE]) {

                state = 1;
            }

            //draw menu
            draw_menu();

            //display gameover
        } else if (state == 2) {

            if (keystate[SDL_SCANCODE_SPACE]) {
                state = 0;
                //delay for a little bit so the space bar press dosnt get triggered twice
                //while the main menu is showing
                SDL_Delay(500);
            }

            if (r == 1) {

                //if player 1 is AI if player 1 was human display the return value of r not 3
                draw_game_over(3);

            } else {

                //display gameover
                draw_game_over(r);
            }

            //display the game
        } else if (state == 1) {

            //check score
            r = check_score();

            //if either player wins, change to game over state
            if (r == 1) {

                state = 2;

            } else if (r == 2) {

                state = 2;
            }

            //paddle ai movement
            move_paddle_ai();

            //* Move the balls for the next frame.
            move_ball();

            //draw net
            draw_net();

            //draw paddles
            draw_paddle();

            //* Put the ball on the screen.
            draw_ball();

            //draw the score
            draw_player_0_score();

            //draw the score
            draw_player_1_score();
        }

        SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->w * sizeof(Uint32));
        SDL_RenderCopy(renderer, screen_texture, NULL, NULL);

        //draw to the display
        SDL_RenderPresent(renderer);

        //time it takes to render  frame in milliseconds
        next_game_tick += 1000 / 60;
        sleep = next_game_tick - SDL_GetTicks();

        if (sleep >= 0) {

            SDL_Delay(sleep);
        }
    }

    //free loaded images
    SDL_FreeSurface(screen);
    SDL_FreeSurface(title);
    SDL_FreeSurface(numbermap);
    SDL_FreeSurface(end);

    //free renderer and all textures used with it
    SDL_DestroyRenderer(renderer);

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;

}

int init(int width, int height, int argc, char *args[]) {

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {

        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    int i;

    for (i = 0; i < argc; i++) {

        //Create window
        if (strcmp(args[i], "-f")) {

            window = SDL_CreateWindow("Pong - a CG project by Mehul and Charishma", SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        } else {
            SDL_DisplayMode displayMode;
            SDL_GetCurrentDisplayMode(0, &displayMode);
            width = SCREEN_WIDTH = displayMode.w;
            height = SCREEN_HEIGHT = displayMode.h;

            window = SDL_CreateWindow("Pong - a CG project by Mehul and Charishma", SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                                      SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    }

    if (window == NULL) {

        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    //create the screen sruface where all the elemnts will be drawn onto (ball, paddles, net etc)
    screen = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);

    if (screen == NULL) {

        printf("Could not create the screen surfce! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    //create the screen texture to render the screen surface to the actual display
    screen_texture = SDL_CreateTextureFromSurface(renderer, screen);

    if (screen_texture == NULL) {

        printf("Could not create the screen_texture! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    //Load the title image
    title = SDL_LoadBMP("title.bmp");

    if (title == NULL) {

        printf("Could not Load title image! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    //Load the numbermap image
    numbermap = SDL_LoadBMP("numbermap.bmp");

    if (numbermap == NULL) {

        printf("Could not Load numbermap image! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    //Load the gameover image
    end = SDL_LoadBMP("gameover.bmp");

    if (end == NULL) {

        printf("Could not Load title image! SDL_Error: %s\n", SDL_GetError());

        return 1;
    }

    // Set the title colourkey.
    Uint32 colorkey = SDL_MapRGB(title->format, 255, 0, 255);
    SDL_SetColorKey(title, SDL_TRUE, colorkey);
    SDL_SetColorKey(numbermap, SDL_TRUE, colorkey);
    SDL_SetColorKey(end, SDL_TRUE, colorkey);

    return 0;
}
