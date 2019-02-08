#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"

#include "picture.h"
#include "system_func.h"
#include "system_const.h"

//------------------------------------------------------------------------------------------------------------------------------------
// Constants definition
//------------------------------------------------------------------------------------------------------------------------------------

#define GAME_WIDTH_MIN 0
#define GAME_WIDTH_MAX 127
#define GAME_HEIGHT_MIN 8
#define GAME_HEIGHT_MAX 63
#define BULLET_SIZE 1 

//------------------------------------------------------------------------------------------------------------------------------------
// Game functions declaration
//------------------------------------------------------------------------------------------------------------------------------------

void Draw_Plane(void);
void Clear_Plane(void);
void Draw_Enemy(void);
void Clear_Enemy(void);
void Draw_Bullet(void);
void Clear_Bullet(void);

//------------------------------------------------------------------------------------------------------------------------------------
// Type definition
//------------------------------------------------------------------------------------------------------------------------------------

typedef enum {welcome_screen, game_rules, game_init, enemy_init, main_game, end_game} STATES;

typedef struct {
    int8_t x;
    int8_t y;
} POSITION;

//------------------------------------------------------------------------------------------------------------------------------------
// Variables declaration
//------------------------------------------------------------------------------------------------------------------------------------

STATES state;
POSITION plane, bullet, enemy;
uint8_t i, key, plane_dx;

char score_txt[] = "0";
uint8_t score;

uint8_t bullet_dy;
int8_t enemy_dx;

uint8_t enemy_x[] = {13,89,32,54,49};

//------------------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------------------

int main(void)
{
    uint32_t i = 0;
    //--------------------------------
    //System initialization
    //--------------------------------
    System_Config();
    KeyPadEnable();

    //--------------------------------
    //SPI3 initialization
    //--------------------------------
    SPI3_Config();

    //--------------------------------
    //LCD initialization
    //--------------------------------    
    LCD_start();
    LCD_clear();

    //--------------------------------
    //Variables initialisation
    //--------------------------------    
    state = welcome_screen;
    key = 0;
    score = 0;

    enemy_dx = 4;
    plane_dx = 3;
    bullet_dy = 4;

    plane.x = GAME_WIDTH_MAX / 2;
    plane.y = GAME_HEIGHT_MAX - 8;

    //--------------------------------
    //LCD dynamic content
    //--------------------------------              
    while (1) {
        switch(state) {
            case welcome_screen:
								LCD_clear();
                draw_LCD(WELCOME_SCREEN);
                printS(16, 0, "SHOOT DEM");
                CLK_SysTickDelay(1000000);
                printS_5x7(8, 16, "Press any key to play");

                while (key == 0) key = KeyPadScanning();
                CLK_SysTickDelay(1000000);
                key = 0;

                LCD_clear();
                state = game_rules;
                break;

            case game_rules:
                printS_5x7(1, 0, "Use Keypad to control");
								printS_5x7(1, 8, "4: LEFT   6: RIGHT");
								printS_5x7(1, 32, "Shoot 5 enemies to win");
								printS_5x7(1, 56, "Press any key to continue!");
                
								while (key == 0) key = KeyPadScanning();
												CLK_SysTickDelay(1000000);
								key = 0;

								LCD_clear();
								state = game_init;
								break;

            case game_init:
                // Score board
                sprintf(score_txt, "%d", score);
								printS_5x7(48, 0, score_txt);

                Draw_Plane();
                CLK_SysTickDelay(1000000);

                state = enemy_init;
                break;

            case enemy_init:
                enemy.x = enemy_x[score];
                enemy.y = GAME_HEIGHT_MIN;

                Draw_Enemy();
                state = main_game;
                break;

            case main_game:
                // SCORE BOARD
                sprintf(score_txt, "%d", score);
								printS_5x7(48, 0, score_txt);

                // PLANE MOVEMENT
                for (i = 0; i < 100000; i++) key = KeyPadScanning();
                
                switch(key) {
                    // Move to left
                    case 4:
                        Clear_Planezz();
                        // Prevent left overfly
                        if (plane.x < plane_dx)
                            plane.x = 0;
                        else
                            plane.x -= plane_dx;
                        Draw_Plane();
                        break;

                    // Move to right
                    case 6:
                        Clear_Plane();
                        plane.x += plane_dx;
                        // Prevent right overfly
                        if (plane.x >= GAME_WIDTH_MAX - 8) plane.x = GAME_WIDTH_MAX - 8;
                        Draw_Plane();
                        break;

                    default: break;
                }

                // BULLET
                Clear_Bullet();
                // Shoot new bullet when current bullet reaches top boundary
                if (bullet.y < bullet_dy) {
                    bullet.x = plane.x + 3;
                    bullet.y = plane.y - 2;
                } else bullet.y -= bullet_dy;

                Draw_Bullet();

                // ENEMY
                Clear_Enemy();
                // Bounce to the right when reaching left boundary
                enemy.x += enemy_dx;
                if (enemy.x <= 0) {
                    enemy.x = 0;
                    enemy_dx = - enemy_dx;
                }
                // Bounce to the left when reaching right boundary
                if (enemy.x >= GAME_WIDTH_MAX - 8) {
                    enemy.x = GAME_WIDTH_MAX - 8;
                    enemy_dx = - enemy_dx;
                }
                Draw_Enemy();

                // SCORING CONDITION
                if (bullet.x >= enemy.x && bullet.x + 1 <= enemy.x + 8) {
                    if (bullet.y >= GAME_HEIGHT_MIN && bullet.y + 1 <= GAME_HEIGHT_MIN + 8) {
                        score++;
                        Clear_Enemy();
                        Clear_Bullet();
                        bullet.x = plane.x + 3;
                        bullet.y = plane.y - 2;
                        if (score == 5)
                            state = end_game;
                        else
                            state = enemy_init;
                    }
                }

                key = 0;
                break;

            case end_game:
                LCD_clear();
                printS(16, 16, "YOU WIN!!");
                CLK_SysTickDelay(1000000);
                printS_5x7(8, 32, "Press any key to replay");

                while (key == 0) key = KeyPadScanning();
                CLK_SysTickDelay(1000000);
                key = 0;
                state = welcome_screen;
                break;

            default: break;
        }
    }
}

void Draw_Plane(void) {
    draw_Bmp8x8(plane.x, plane.y, 1, 0, PLANE);
}

void Clear_Plane(void) {
    fill_Rectangle(plane.x, plane.y, plane.x + 8, plane.y + 8, 0, 0);
}

void Draw_Enemy(void) {
    draw_Bmp8x8(enemy.x, enemy.y, 1, 0, ENEMY);
}

void Clear_Enemy(void) {
    fill_Rectangle(enemy.x, enemy.y, enemy.x + 8, enemy.y + 8, 0, 0);
}

void Draw_Bullet(void) {
    fill_Rectangle(bullet.x, bullet.y, bullet.x + 1, bullet.y + 1, 1, 0);
}

void Clear_Bullet(void) {
    fill_Rectangle(bullet.x, bullet.y, bullet.x + 1, bullet.y + 1, 0, 0);
}
