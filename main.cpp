#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_log.h>


using namespace std;

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_TITLE "HELLO FREE FIRE"

struct App{
    SDL_Renderer *renderer;
    SDL_Window *window;
    int up;
    int down;
    int left;
    int right;
    int fire;
};

App app;

void initSDL(){
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG); // thư vien sẽ dc cấu hình để hỗ trở dạng hình ảnh PNG và JPEG

    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = SDL_WINDOW_SHOWN;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Could not initalize SDL " << SDL_GetError();
        exit(1);
    }

    app.window = SDL_CreateWindow(SCREEN_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

    if(!app.window){
        cout << "Fail to open window " << SDL_GetError();
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

    if(!app.renderer){
        cout << "Fail to create renderer " << SDL_GetError();
        exit(1);
    }

}

void doKeyDown(SDL_KeyboardEvent *event)
{
    if(event->repeat == 0){

        if(event->keysym.scancode == SDL_SCANCODE_SPACE)
        {
            app.fire = 1;
        }
        if(event->keysym.scancode == SDL_SCANCODE_UP)
        {
            app.up = 1;
        }

        if(event->keysym.scancode == SDL_SCANCODE_DOWN)
        {
            app.down = 1;
        }

        if(event->keysym.scancode == SDL_SCANCODE_LEFT)
        {
            app.left = 1;
        }

        if(event->keysym.scancode == SDL_SCANCODE_RIGHT)
        {
            app.right = 1;
        }
    }
}


void doKeyUp(SDL_KeyboardEvent *event)
{
    if(event->repeat == 0)
    {
        if(event->keysym.scancode == SDL_SCANCODE_SPACE)
        {
            app.fire = 0;
        }
        if(event->keysym.scancode == SDL_SCANCODE_UP)
        {
            app.up = 0;
        }

        if(event->keysym.scancode == SDL_SCANCODE_DOWN)
        {
            app.down = 0;
        }

        if(event->keysym.scancode == SDL_SCANCODE_LEFT)
        {
            app.left = 0;
        }

        if(event->keysym.scancode == SDL_SCANCODE_RIGHT)
        {
            app.right = 0;
        }
    }
}

void doInput(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT :
                exit(0);
                break;
            case SDL_KEYDOWN :
                doKeyDown(&event.key);
                break;
            case SDL_KEYUP :
                doKeyUp(&event.key);
                break;
            default:
                break;
        }
    }
}

void prepareScene(){
    SDL_SetRenderDrawColor(app.renderer, 0, 255, 255, 255);
    SDL_RenderClear(app.renderer);
}

void presentScene(){
    SDL_RenderPresent(app.renderer);
}

SDL_Texture *loadTexture(char *filename)
{
    SDL_Texture *texture;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ", filename);

    texture = IMG_LoadTexture(app.renderer, filename);

    if(!texture){
        cerr << "Fail to load";
        exit(1);
    }

    return texture;
}

void drawBackground(SDL_Texture *texture)
{
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = SCREEN_WIDTH;
    dest.h = SCREEN_HEIGHT;
    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blit(SDL_Texture *texture, int x, int y, int width, int height)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = width;
    dest.h = height;

    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

struct entity
{
    int x;
    int y;
    int dx;
    int dy;
    int healthy;
    SDL_Texture *texture;
};

entity player;

entity bullet;

int main(int argc, char *argv[]){

    memset(&app, 0, sizeof(App));
    memset(&player, 0, sizeof(entity));
    memset(&bullet, 0, sizeof(entity));

    initSDL();

    player.x = 0;
    player.y = 0;

    player.texture = loadTexture("player.png");
    SDL_Texture *backgroud = loadTexture("space.jpg");
    bullet.texture = loadTexture("fireball.png");

    while(1){
        prepareScene();

        doInput();

        player.x += player.dx; // cập nhật địa chỉ hiện tại khi chuẩn bị bắn viên đạn tiếp theo
        player.y += player.dx;

        if (app.up)
		{
			player.y -= 4;
		}

		if (app.down)
		{
			player.y += 4;
		}

		if (app.left)
		{
			player.x -= 4;
		}

		if (app.right)
		{
			player.x += 4;
		}

		if(app.fire == 1 && bullet.healthy == 0)
		// đọc là : đạn bắn ra rồi và chờ đến rìa màn hình để được bắn tiếp
		// sau khi bấm space, đạn được bắn ra, khi thả thay khỏi space, đạn lập tức trở thành 1 tức là đã được nạp app.fire = 0 trong if thì đạn sẽ được bắn liên tục
		//neu mà đạn đã nạp nhưng đạn chưa qua màn hình tức là healthy = 0 thì :
        {
            bullet.x = player.x; // toạ độ x viên đạn tiếp theo xuất hiện
            bullet.y = player.y; // toạ độ y viên đạn tiếp theo xuất hiện
            // x, y của viên đạn lần lượt được cập nhật là vị trí của người chơi
            bullet.dx = 15;
            bullet.dy = 0;
            // viên đạn đang di chuyển với tốc độ 15 pixel liên tục
            bullet.healthy = 1;
        }

        bullet.x += bullet.dx; // lấy từ trên if
        bullet.y += bullet.dy;  // cập nhật vị trí của viên đạn trong không gian game;

        if(bullet.x > SCREEN_WIDTH){
            bullet.healthy = 0;
        }

        drawBackground(backgroud);
        blit(player.texture, player.x, player.y, 200, 200);
        if(bullet.healthy > 0){
            blit(bullet.texture, bullet.x, bullet.y, 10, 10);
        }

        presentScene();

        SDL_Delay(20);
    }

    SDL_DestroyTexture(backgroud);
    SDL_Quit();

    return 0;
}
