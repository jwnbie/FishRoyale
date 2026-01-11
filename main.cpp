#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

#pragma comment(lib, "winmm.lib")

inline double dist(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

const int HEART_W = 40;
const int HEART_H = 35;

class Fish {
public:
    int x, y, w, h;
    bool alive;
    void* img, *mask;

    Fish(const char* imgPath, const char* maskPath, int w_, int h_) : w(w_), h(h_), alive(true) {
        int size = imagesize(0, 0, w - 1, h - 1);
        img = malloc(size);
        mask = malloc(size);

        readimagefile(imgPath, 0, 0, w - 1, h - 1);
        getimage(0, 0, w - 1, h - 1, img);
        cleardevice();
        readimagefile(maskPath, 0, 0, w - 1, h - 1);
        getimage(0, 0, w - 1, h - 1, mask);
        cleardevice();
    }

    virtual void draw() {
        if (!alive) return;
        putimage(x, y, mask, AND_PUT);
        putimage(x, y, img, XOR_PUT);
    }

    virtual ~Fish() {
        free(img);
        free(mask);
    }
};

class Player : public Fish {
    void* imgR, *maskR, *imgL, *maskL;
    bool faceRight;
    int baseY;
    int tick;
    float amplitude = 5;
    float freq = 12.0f;

public:
    int radius = 30;
    int lives = 3;
    bool visible = true;
    int respawnTimer = 0;

    Player(int startX, int startY)
        : Fish("player flip.bmp", "mask player flip.bmp", 160, 80), faceRight(true) {

        int sz = imagesize(0, 0, 160, 80);
        imgR = malloc(sz); maskR = malloc(sz);
        imgL = malloc(sz); maskL = malloc(sz);

        readimagefile("player flip.bmp", 0, 0, 160, 80); getimage(0, 0, 160, 80, imgR);
        cleardevice(); readimagefile("mask player flip.bmp", 0, 0, 160, 80); getimage(0, 0, 160, 80, maskR);

        cleardevice(); readimagefile("player.bmp", 0, 0, 160, 80); getimage(0, 0, 160, 80, imgL);
        cleardevice(); readimagefile("mask player.bmp", 0, 0, 160, 80); getimage(0, 0, 160, 80, maskL);

        x = startX;
        baseY = startY;
        y = baseY;

        img = imgR;
        mask = maskR;
    }

    void move(int dx, int dy) {
        if (!visible) return;
        x += dx;
        baseY += dy;
        if (dx > 0) faceRight = true;
        if (dx < 0) faceRight = false;

        if (x < 0) x = 0;
        if (x > getmaxx() - w) x = getmaxx() - w;
        if (baseY < 0) baseY = 0;
        if (baseY > getmaxy() - h) baseY = getmaxy() - h;
    }

    void update() {
        if (!visible) {
            if (--respawnTimer <= 0) {
                visible = true;
                x = 100; baseY = 100;
            }
            return;
        }
        tick++;
        y = baseY + amplitude * sin(tick / freq);
    }

    void draw() {
        if (!visible) return;
        img = faceRight ? imgR : imgL;
        mask = faceRight ? maskR : maskL;
        Fish::draw();
    }

    void hitByShark() {
        if (!visible) return;
        lives--;
        visible = false;
        respawnTimer = 30;
        PlaySound("chomp.wav", NULL, SND_FILENAME | SND_ASYNC);
    }

    ~Player() {
        free(imgR); free(maskR);
        free(imgL); free(maskL);
    }
};

class SmallFish : public Fish {
    int speedX, respawnTimer, baseY, tick;

public:
    SmallFish() : Fish("kecil.bmp", "mask kecil.bmp", 70, 32), respawnTimer(0), tick(0) {
        respawn();
    }

    void update() {
        if (!alive) {
            if (respawnTimer > 0) respawnTimer--;
            else respawn();
            return;
        }
        x += speedX;
        tick++;
        y = baseY + 5 * sin(tick / 10.0);
        if (x < -w) respawn();
    }

    void kill() { alive = false; respawnTimer = 60; }

    void respawn() {
        x = getmaxx() + rand() % 200;
        baseY = rand() % (getmaxy() - h);
        y = baseY;
        speedX = -(2 + rand() % 2);
        alive = true;
        tick = rand() % 1000;
    }
};

class Shark : public Fish {
    int speedX;

public:
    Shark() : Fish("hiu.bmp", "hiu mask.bmp", 252, 120) { respawn(); }

    void update() {
        x -= speedX;
        if (x < -w) respawn();
    }

    void respawn() {
        x = getmaxx() + rand() % 500;
        y = rand() % (getmaxy() - h);
        speedX = 2 + rand() % 2;
    }
};

void drawHearts(int lives, void* heartImg, void* heartMask) {
    int w = HEART_W, h = HEART_H;
    for (int i = 0; i < lives; i++) {
        int x = getmaxx() - (i + 1) * (w + 10);
        int y = 10;
        putimage(x, y, heartMask, AND_PUT);
        putimage(x, y, heartImg, XOR_PUT);
    }
}

void playOpening() {
    readimagefile("opening.bmp", 0, 0, getmaxx(), getmaxy());
    PlaySound("bubbles.wav", NULL, SND_FILENAME | SND_ASYNC);
    delay(4000);

    for (int i = 0; i < 6; i++) {
        cleardevice();
        if (i % 2 == 0)
            readimagefile("pra1.bmp", 0, 0, getmaxx(), getmaxy());
        else
            readimagefile("pra2.bmp", 0, 0, getmaxx(), getmaxy());
        delay(500);
    }

    cleardevice();
}

int main() {
    srand(time(0));
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    readimagefile("heart.bmp", 0, 0, HEART_W, HEART_H);
    int size = imagesize(0, 0, HEART_W, HEART_H);
    void* heartImg = malloc(size);
    getimage(0, 0, HEART_W, HEART_H, heartImg);

    readimagefile("mask heart.bmp", 0, 0, HEART_W, HEART_H);
    void* heartMask = malloc(size);
    getimage(0, 0, HEART_W, HEART_H, heartMask);

    playOpening();
    readimagefile("sea3.bmp", 0, 0, getmaxx(), getmaxy());
    int bgSize = imagesize(0, 0, getmaxx(), getmaxy());
    void* bgImage = malloc(bgSize);
    getimage(0, 0, getmaxx(), getmaxy(), bgImage);

    mciSendString("open \"music2.wav\" type mpegvideo alias bgm", NULL, 0, NULL);
    mciSendString("play bgm repeat", NULL, 0, NULL);

    Player player(100, 100);
    SmallFish* smalls[20];
    Shark* sharks[3];
    for (int i = 0; i < 20; i++) smalls[i] = new SmallFish();
    for (int i = 0; i < 3; i++) sharks[i] = new Shark();

    int score = 0;
    int page = 0;

    while (player.lives > 0) {
        setactivepage(page);
        setvisualpage(1 - page);
        cleardevice();
        putimage(0, 0, bgImage, COPY_PUT);

        settextstyle(BOLD_FONT, HORIZ_DIR, 3);
        setbkcolor(BLUE); setcolor(WHITE);
        char s[32]; sprintf(s, "Score: %d", score);
        outtextxy(10, 10, s);

        drawHearts(player.lives, heartImg, heartMask);

        for (int i = 0; i < 20; i++) {
            smalls[i]->update();
            if (smalls[i]->alive && player.visible &&
                dist(player.x + player.w / 2, player.y + player.h / 2,
                     smalls[i]->x + smalls[i]->w / 2, smalls[i]->y + smalls[i]->h / 2) < player.radius) {
                smalls[i]->kill();
                score += 5;
                PlaySound("makan.wav", NULL, SND_FILENAME | SND_ASYNC);
            }
            smalls[i]->draw();
        }

        for (int i = 0; i < 5; i++) {
            sharks[i]->update(); sharks[i]->draw();
            if (player.visible &&
                dist(player.x + player.w / 2, player.y + player.h / 2,
                     sharks[i]->x + sharks[i]->w / 2, sharks[i]->y + sharks[i]->h / 2) < player.radius + 40) {
                player.hitByShark();
            }
            for (int j = 0; j < 20; j++) {
                if (smalls[j]->alive &&
                    dist(sharks[i]->x + sharks[i]->w / 2, sharks[i]->y + sharks[i]->h / 2,
                         smalls[j]->x + smalls[j]->w / 2, smalls[j]->y + smalls[j]->h / 2) < 50) {
                    smalls[j]->kill();
                }
            }
        }

        player.update(); player.draw();

        if (kbhit()) {
            char ch = getch();
            if (ch == 0) ch = getch();
            switch (ch) {
                case 72: player.move(0, -5); break;
                case 80: player.move(0, 5); break;
                case 75: player.move(-5, 0); break;
                case 77: player.move(5, 0); break;
                case 27: goto exit;
            }
        }

        delay(16);
        page = 1 - page;
    }

exit:

setactivepage(0);
setvisualpage(0);
cleardevice();

PlaySound("lose.wav", NULL, SND_FILENAME | SND_ASYNC);
for (int i = 0; i < 600; i++) {
    cleardevice();
    if (i % 2 == 0)
        readimagefile("go1.bmp", 0, 0, getmaxx(), getmaxy());
    else
        readimagefile("go2.bmp", 0, 0, getmaxx(), getmaxy());
    delay(500);
}


delay(1000);

    free(heartImg); free(heartMask);
    mciSendString("stop bgm", NULL, 0, NULL);
    mciSendString("close bgm", NULL, 0, NULL);
    for (int i = 0; i < 20; i++) delete smalls[i];
    for (int i = 0; i < 3; i++) delete sharks[i];
    free(bgImage);
    closegraph();
    return 0;
}
