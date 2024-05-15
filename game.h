#ifndef _G__H
#define _G__H

#include <bits/stdc++.h>
#include <list>
#include <SDL.h>
#include <SDL_image.h>
#include "definition.h"
#include "graphics.h"
#include "structs.h"

bool playgame = false;

bool isGameRunning = true;

void initPlayer(Entity& player) {
    player.x = 100;
    player.y = 100;
    player.health = 1;
    player.side = SIDE_PLAYER;
    player.reload = 0;
}

void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy)
{
	int steps = max(abs(x1 - x2), abs(y1 - y2));

	if (steps == 0)
	{
		*dx = *dy = 0;
		return;
	}

	*dx = (x1 - x2);
	*dx /= steps;

	*dy = (y1 - y2);
	*dy /= steps;
}

struct Game {
    Entity player;
    list<Entity*> bullets;
	list<Entity*> fighters;
	list<Entity*> bulletboss;

	Sprite explosion;
	Star stars[MAX_STARS];

    SDL_Texture *bulletTexture, *enemyTexture, *enemyBulletTexture, *background, *explosionTexture,*heartTexture, *bossTexture, *khung, *khung2, *roll,
    *bulletbossTexture, *playTexture, *muteTexture, *unmuteTexture, *playbutton1Texture, *playbutton2Texture, *quit1buttonTexture, *quit2buttonTexture;
    Mix_Chunk *gJump;Mix_Music *gMusic;

    Entity *boss = new Entity;
    int bossheath = 10;

    int enemySpawnTimer;
    int stageResetTimer;
    int playerdiecount = 7;
    int score = 0;
    int backgroundX = 0;

    int dx = 0;
    int dy = 0;

    int a = 1;

    bool check_explosion = false;
    int bulletCollidesX = 0;
    int bulletCollidesY = 0;

    int bulletCollidesX1 = 100;
    int bulletCollidesY1 = 100;

    void empty(list<Entity*>& entities) {
        while (!entities.empty()) {
            Entity* e = entities.front();
            entities.pop_front();
            if (e != &player) delete e;
        }
    }

    void reset()
    {
        empty(fighters);
        empty(bullets);
        empty(bulletboss);
        fighters.push_back(&player);
	    initPlayer(player);
        enemySpawnTimer = 0;
        stageResetTimer = 10;
        boss->reload = 1;
	}

    void init(Graphics& graphics)
    {
        player.texture = graphics.loadTexture("player.png");
        SDL_QueryTexture(player.texture, NULL, NULL, &player.w, &player.h);
        bulletTexture = graphics.loadTexture("img/tinyBlackBox.png");
        enemyTexture = graphics.loadTexture("img/enemy.png");
        enemyBulletTexture = graphics.loadTexture("img/enemyBullet.png");
        background = graphics.loadTexture("img/back.png");
        explosionTexture = graphics.loadTexture("blast.png");
        heartTexture = graphics.loadTexture("heart.png");
        explosion.init(explosionTexture, BLAST_FRAMES, BLAST_CLIPS);
        gMusic = loadMusic("meo.mp3");
        gJump = loadSound("bomb.mp3");
        bossTexture = graphics.loadTexture("boss.png");
        bulletbossTexture = graphics.loadTexture("bulletboss.png");
        playTexture = graphics.loadTexture("Play.png");
        muteTexture = graphics.loadTexture("mute.png");
        unmuteTexture = graphics.loadTexture("unmute.png");
        playbutton1Texture = graphics.loadTexture("Playbutton.png");
        playbutton2Texture = graphics.loadTexture("Playbutton2.png");
        quit1buttonTexture = graphics.loadTexture("quit.png");
        quit2buttonTexture = graphics.loadTexture("quit2.png");
        khung = graphics.loadTexture("khung.png");
        khung2 = graphics.loadTexture("khung2.png");
        roll = graphics.loadTexture("roll.png");
        reset();
    }

    void cleanmenu()
    {
        SDL_DestroyTexture(muteTexture);
        SDL_DestroyTexture(unmuteTexture);
        SDL_DestroyTexture(playTexture);
        SDL_DestroyTexture(playbutton1Texture);
        SDL_DestroyTexture(playbutton2Texture);
        SDL_DestroyTexture(quit1buttonTexture);
        SDL_DestroyTexture(quit2buttonTexture);
    }

    void drawplay(Graphics &graphics)
    {
        graphics.renderTexture(playTexture, 0 , 0);
    }

    void drawmute(Graphics &graphics)
    {
        graphics.renderTexture(muteTexture, 20 , 20);
    }

    void drawunmute(Graphics &graphics)
    {
        graphics.renderTexture(unmuteTexture, 20 , 20);
    }

    void playgmusic()
    {
        play(gMusic);
    }

    void drawplaybutton1(Graphics &graphics)
    {
        graphics.renderTexture(playbutton1Texture, 450 , 350);
    }

    void drawplaybutton2(Graphics &graphics)
    {
        graphics.renderTexture(playbutton2Texture, 400 , 310);
    }

    void drawquitbutton1(Graphics &graphics)
    {
        graphics.renderTexture(quit1buttonTexture, 700 , 350);
    }

    void drawquitbutton2(Graphics &graphics)
    {
        graphics.renderTexture(quit2buttonTexture, 650 , 310);
    }

    void yourscore(Graphics &graphics)
    {
        TTF_Font* font = loadFont("static\\OpenSans_SemiCondensed-SemiBold.ttf",20);
        SDL_Color color = {255, 165, 0, 255};
        string liveText = "Your score: " + to_string(score);
        SDL_Texture *scoretext = renderText(liveText.c_str(), font, color, graphics.renderer);
        graphics.renderTexture(scoretext, 100, 65);
    }

    void fireBullet()
    {
        Entity *bullet = new Entity();
        bullets.push_back(bullet);

        bullet->x = player.x;
        bullet->y = player.y;
        bullet->y += (player.h / 2) - (bullet->h / 2);
        bullet->dx = PLAYER_BULLET_SPEED;
        bullet->health = 1;
        bullet->texture = bulletTexture;
        bullet->side = SIDE_PLAYER;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

        player.reload = PLAYER_RELOAD;
    }

    void fireEnemyBullet(Entity* enemy)
    {
        Entity *bullet = new Entity();
        bullets.push_back(bullet);

        bullet->x = enemy->x;
        bullet->y = enemy->y;
        bullet->health = 1;
        bullet->texture = enemyBulletTexture;
        bullet->side = SIDE_ALIEN;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

        bullet->x += (enemy->w / 2) - (bullet->w / 2);
        bullet->y += (enemy->h / 2) - (bullet->h / 2);

        calcSlope(player.x + (player.w / 2), player.y + (player.h / 2),
                  enemy->x, enemy->y, &bullet->dx, &bullet->dy);
        bullet->dx *= ENEMY_BULLET_SPEED - 2;
        bullet->dy *= ENEMY_BULLET_SPEED - 2;

        enemy->reload = 500;///(rand() % FRAME_PER_SECOND * 2);
    }

    void fireBossBullet()
    {
        Entity *bullet = new Entity();
        bulletboss.push_back(bullet);

        bullet->x = boss->x;
        bullet->y = boss->y;
        bullet->health = 1;
        bullet->texture = bulletbossTexture;
        bullet->side = SIDE_ALIEN;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

        bullet->x += (boss->w / 2) - (bullet->w / 2);
        bullet->y += (boss->h / 2) - (bullet->h / 2);

        calcSlope(player.x + (player.w / 2), player.y + (player.h / 2),
                  boss->x, boss->y, &bullet->dx, &bullet->dy);
        bullet->dx *= ENEMY_BULLET_SPEED - 5;
        bullet->dy *= ENEMY_BULLET_SPEED - 4;

                    //(rand() % FRAME_PER_SECOND * 2);
    }

    void doPlayer(int keyboard[])
    {
        if (player.health == 0) return;

        player.dx = player.dy = 0;

        if (player.reload > 0) player.reload--;
        if (keyboard[SDL_SCANCODE_UP]) player.dy = -PLAYER_SPEED;
        if (keyboard[SDL_SCANCODE_DOWN]) player.dy = PLAYER_SPEED;
        if (keyboard[SDL_SCANCODE_LEFT]) player.dx = -PLAYER_SPEED;
        if (keyboard[SDL_SCANCODE_RIGHT]) player.dx = PLAYER_SPEED;
        if (keyboard[SDL_SCANCODE_LCTRL] && player.reload == 0) fireBullet();
    }

    bool bulletHitFighter(Entity *b)
    {
        for (Entity* fighter: fighters) {
            if (fighter->side != b->side && b->collides(fighter)) {
                fighter->health = 0;
                return true;
            }
        }
        return false;
    }

    void genBoss(Entity *& boss)
    {
        boss->x = 1000;
        boss->y = 200;
        boss->side = SIDE_ALIEN;
        boss->x += dx;
        boss->y += dy;
        boss->health = bossheath;
        boss->texture = bossTexture;
        SDL_QueryTexture(bossTexture, NULL, NULL, &boss->w, &boss->h);
    }

    bool bulletPlayerfireBoss(Entity *b)
    {
        if(b->side == SIDE_PLAYER and b->collides(boss)){
            return true;
        }
        return false;
    }

    void drawBoss(Graphics &graphics)
    {
        graphics.renderTexture(boss->texture, boss->x, boss->y);
    }

    bool collidesPlayerEnemy()
    {
        for(Entity *a : fighters){
            for(Entity *b : fighters){
                if(a->side != b->side and a->collides(b)){
                    a->health = 0;
                    b->health = 0;
                    bulletCollidesX1 = a->x;
                    bulletCollidesY1 = a->y;
                    playerdiecount--;
                    return true;
                }
            }
        }
        return false;
    }

    void doBullets()
    {
        auto it = bullets.begin();
        while (it != bullets.end()) {
            auto temp = it++;
            Entity* b = *temp;
            b->move();
            if (bulletHitFighter(b)) {
                check_explosion = true;
                bulletCollidesX = b->x;
                bulletCollidesY = b->y;
                if(b->side == SIDE_ALIEN ){
                    playerdiecount--;
                }
                if(b->side == SIDE_PLAYER){
                    score++;
                }
                delete b;
                bullets.erase(temp);
            }

            if(bulletPlayerfireBoss(b)){
                bossheath--;
                check_explosion = true;
                bulletCollidesX = b->x;
                bulletCollidesY = b->y;
                if(bossheath > 0){
                    delete b;
                    bullets.erase(temp);
                }
            }
            if(b->offScreen())
            {
                delete b;
                bullets.erase(temp);
            }
        }
    }

    void doBulletBoss()
    {
        auto it = bulletboss.begin();
        while (it != bulletboss.end()) {
            auto temp = it++;
            Entity* b = *temp;
            b->move();
            if (bulletHitFighter(b)) {
                check_explosion = true;
                bulletCollidesX = b->x;
                bulletCollidesY = b->y;
                playerdiecount -= 2;
                delete b;
                bullets.erase(temp);
                continue;
            }
            if(b->offScreen())
            {
                delete b;
                bullets.erase(temp);
            }
        }
    }

    void doEnemies() {
        for (Entity* e: fighters) {
            if (e != &player && player.health != 0 && --e->reload <= 0)
                fireEnemyBullet(e);
        }
    }

    void doBoss() {
        if (boss->reload-- > 0){
            fireBossBullet();
        }
    }

    void spawnEnemies(void) {

        if (--enemySpawnTimer <= 0) {
            Entity *enemy = new Entity();
            fighters.push_back(enemy);
            enemy->x = SCREEN_WIDTH;
            enemy->y = rand() % SCREEN_HEIGHT;
            enemy->dx = -(2 + (rand() % 3)) ;
            enemy->health = 1;
            enemy->reload = FRAME_PER_SECOND * (1 + (rand() % 3));
            enemy->side = SIDE_ALIEN;
            enemy->texture = enemyTexture;
            SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);
            enemySpawnTimer = 30 + (rand() % 60);
        }
    }

    void doFighters(void)
    {
        auto it = fighters.begin();
        it++;

        while (it != fighters.end()) {
            auto temp = it++;
            Entity* fighter = *temp;

            fighter->move();
            if (fighter->x < -fighter->w) fighter->health = 0;

            if (fighter->health == 0) {
                delete fighter;
                fighters.erase(temp);
                continue;
            }

        }

        player.move();
        if (player.x < 0) player.x = 0;
        else if (player.x >= SCREEN_WIDTH - player.w)
            player.x = SCREEN_WIDTH - player.w;
        if (player.y < 0) player.y = 0;
        else if (player.y >= SCREEN_HEIGHT - player.h)
            player.y = SCREEN_HEIGHT - player.h;
	}

	void doBackground(void) {
        if (--backgroundX < -SCREEN_WIDTH)
        {
            backgroundX = 0;
        }
    }

    void doStarfield(void) {
        for (int i = 0 ; i < MAX_STARS ; i++) {
            stars[i].x -= stars[i].speed;

            if (stars[i].x < 0)
            {
                stars[i].x = SCREEN_WIDTH + stars[i].x;
            }
        }
    }

    void doLogic(int keyboard[]) {
        doBackground();
        if (player.health == 0 && --stageResetTimer <= 0){
            reset();
        }
        doPlayer(keyboard);
        doFighters();
        doEnemies();
        doBullets();
        spawnEnemies();
        genBoss(boss);
        doBulletBoss();
    }

    void drawBackground(SDL_Renderer* renderer) {
        SDL_Rect dest;
        for (int x = backgroundX ; x < SCREEN_WIDTH ; x += SCREEN_WIDTH) {
            dest.x = x;
            dest.y = 0;
            dest.w = SCREEN_WIDTH;
            dest.h = SCREEN_HEIGHT;

            SDL_RenderCopy(renderer, background, NULL, &dest);
        }
    }

    void drawExplosion(Graphics& graphics, int x, int y){
        while(explosion.currentFrame < BLAST_FRAMES){
            explosion.ticky();
            graphics.render(x, y, explosion);
        }
        explosion.currentFrame = 0;
    }

    void endgamewin(Graphics &graphics)
    {
        TTF_Font* font = loadFont("static\\OpenSans_SemiCondensed-SemiBold.ttf", 60);
        SDL_Color color = {255, 255, 255, 255};
        SDL_Texture *endgametexture = renderText("CONGRATULATION! YOU WIN", font, color, graphics.renderer);
        graphics.renderTexture(endgametexture, 500, 600);

    }

    void endgamelose(Graphics &graphics)
    {
        TTF_Font* font = loadFont("static\\OpenSans_SemiCondensed-SemiBold.ttf", 60);
        SDL_Color color = {255, 255, 255, 255};
        SDL_Texture *endgametexture = renderText("YOU LOSE!", font, color, graphics.renderer);
        graphics.renderTexture(endgametexture, 500, 600);
    }


    void draw(Graphics& graphics)
    {

        if(boss->x > 900){
            dx += -2;
        }
        else {
            dx += 0;
            doBoss();
        }

        drawBackground(graphics.renderer);

        graphics.renderTexture(khung, 0, 0);
        graphics.renderTexture(khung2, 850, 0);
        graphics.renderTexture(roll, 370, 0);

        for(int heart = 1; heart <= playerdiecount ; heart++){
            graphics.renderTexture(heartTexture, 70 + 30* heart, 50);
        }

        for(int heartboss = 1; heartboss <= bossheath ; heartboss++){
            graphics.renderTexture(heartTexture, 900 + 30* heartboss, 40);
        }

		for (Entity* b: bullets)
            graphics.renderTexture(b->texture, b->x, b->y);

        for (Entity* b: bulletboss)
            graphics.renderTexture(b->texture, b->x, b->y);

        for (Entity* b: fighters)
            if (b->health > 0)
                graphics.renderTexture(b->texture, b->x, b->y);

        if (check_explosion == true){
            drawExplosion(graphics, bulletCollidesX, bulletCollidesY);
            play(gJump);
            check_explosion = false;
        }

        if(collidesPlayerEnemy()){
            drawExplosion(graphics, bulletCollidesX1, bulletCollidesY1);
            play(gJump);
        }

        yourscore(graphics);


        if(playerdiecount <= 0) {
            endgamelose(graphics);
            isGameRunning = false;
        }

        if(boss->health > 0){
            drawBoss(graphics);

        }else
        {
            endgamewin(graphics);
            isGameRunning = false;
        }

    }
};

void waitForKeyPress() {
    SDL_Event event;
    bool keyPressed = false;

    while (!keyPressed) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keyPressed = true;
            }
        }
    }
}

#endif // _LOGIC__H


