/*
    Hello, I am Felipe Resende and I love old consoles architecture.
    The code below is the implementation of a simple pong game and is my first Game Boy game.
    You can read, modify and use to study.
    You can build it with gbdk-2.95 (C) 2000 Michael Hope <michaelh@earthling.net> - http://gbdk.sourceforge.net/
*/

#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "Sprites.c"

//All game constants 
const UINT8 BALL_SPRITE = 0;
const UINT8 PADDLE_SPRITE = 1;
const UINT8 BALL = 0;
const UINT8 PADDLE_TOP = 1;
const UINT8 PADDLE_BOTTON = 2;
const UINT8 ENEMY_PADDLE_TOP = 3;
const UINT8 ENEMY_PADDLE_BOTTON = 4;

//All game variables

//The position of ball is a division between this values by 8.
//This emulate the slow movement and avoid to move the ball one pixel per update.
//Is necessary move 8 units to move 1 pixel
INT16 ballPositionX = 672; // 672/8 = 84 or the center of screen in X
INT16 ballPositionY = 608; // 608/8 = 76 or the center of screen in Y

INT8 ballDirectionX = -1; // define direction in x. -1 is left, 1 is right
INT8 ballDirectionY = 0; // define direction in y. 1 is down, -1 is up

INT16 paddle = 72; // the height position of paddle in screen

INT16 enemyPaddle = 72; // the height position of paddle in screen

// score of the players. I use int because is more easy to print
int playerScore = 0;
int enemyScore = 0;

void Render(){

    //Apply all transformations in the objects

    move_sprite(BALL, ballPositionX/8, ballPositionY/8);
    move_sprite(PADDLE_TOP, 8, paddle);
    move_sprite(PADDLE_BOTTON, 8, paddle+8);

    move_sprite(ENEMY_PADDLE_TOP, 160,enemyPaddle);
    move_sprite(ENEMY_PADDLE_BOTTON, 160,enemyPaddle+8);
}

void UpdateScore(){

    //scroll the window with very interesting effect. This was an accident, but I like it
    printf(" \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    //I literally spent 3 hours to discovery that need to use %u instead %d. I don't know why
    printf("     %u       %u",playerScore,enemyScore);
}

// Reset all default values to restate game
void RestartGame(int ballDirection){
    ballPositionX = 672; // center of screen when divide by 8
    ballPositionY = 608; // center of screen when divide by 8
    ballDirectionX = ballDirection;
    ballDirectionY = 0; // reset the vertical movement of ball
    paddle = 72; // paddle initial position
    enemyPaddle = 72; // paddle initial position
    Render(); // refresh the screen
    UpdateScore(); //update the score
    delay(2000);
}

void main(){

    //SETUP THE GAME ===================

    //Initialize the game font
    font_t ibm_font;
    font_init();
    ibm_font = font_load(font_ibm);

    //print a Tile screen
    printf(" \n\n\n\n\n\n");
    printf("        Pong\n\n         by \n\n   Felipe Resende");

    //wait in the "Title Screen"
    delay(2000);

    //load 2 srpites: ball and paddle
    set_sprite_data(0, 2, Sprites);

    //draw ball
    set_sprite_tile(BALL, BALL_SPRITE);

    //The paddle are made with two sprites, the top and botton. Below we add two paddles.
    //draw player paddle
    set_sprite_tile(PADDLE_TOP, PADDLE_SPRITE);
    set_sprite_tile(PADDLE_BOTTON, PADDLE_SPRITE);

    //draw enemy paddle
    set_sprite_tile(ENEMY_PADDLE_TOP, PADDLE_SPRITE);
    set_sprite_tile(ENEMY_PADDLE_BOTTON, PADDLE_SPRITE);

    //So... I create a Render function with the the same code below, but if I use that here, the collision not work
    //Is it a problem with stack?
    move_sprite(BALL, ballPositionX/8, ballPositionY/8);
    move_sprite(PADDLE_TOP, 8, paddle);
    move_sprite(PADDLE_BOTTON, 8, paddle+8);
    move_sprite(ENEMY_PADDLE_TOP, 160,enemyPaddle);
    move_sprite(ENEMY_PADDLE_BOTTON, 160,enemyPaddle+8);

    SHOW_SPRITES;
    UpdateScore();
    
    delay(2000);

    // MAIN LOOP ===================

    while(1){
        
        //update ball position
        ballPositionX += ballDirectionX*8; // the 8 number specify that the horizontal velocity is constant
        ballPositionY += ballDirectionY;


        //check collision with player paddle
        if(ballPositionX/8 == 12){
            if(ballPositionY/8 >= paddle - 8 && ballPositionY/8 <= paddle + 16){
                ballDirectionX = 1; // change the direction to right
                // the y direction is based on distance between the center of ball and center of paddle
                ballDirectionY = ballPositionY/8 - paddle - 4; // the +4 offset align the paddle center with the ball
            }
        }

        //check collision with enemy paddle
        if(ballPositionX/8 == 156){
            if(ballPositionY/8 >= enemyPaddle - 8 && ballPositionY/8 <= enemyPaddle + 16){
                ballDirectionX = -1;// change the direction to left
                // the y direction is based on distance between the center of ball and center of paddle
                ballDirectionY = ballPositionY/8 - enemyPaddle - 4; // the +4 offset align the paddle center with the ball
            }
        }
        
        // if player paddle don't catch the ball
        if(ballPositionX/8 < 8){
            enemyScore += 1;
            RestartGame(1);
        }

        // if enemy paddle don't catch the ball
        if(ballPositionX/8 > 164){
            playerScore += 1;
            RestartGame(-1);
        }

        // when the paddle touch the top or the botton of screen
        if(ballPositionY/8 <= 16){
            ballDirectionY *= -1;
        }
        if(ballPositionY/8 >= 152){
            ballDirectionY *= -1;
        }

        // read the inputs
        switch(joypad()){
            case J_UP:
                paddle -= 1; // move the paddle position to up
                break;
            case J_DOWN:
                paddle += 1; // move the paddle position to down
                break;
        }

        // IA of the paddle enemy.
        // if the paddle position is above than ball position, the paddle will move up
        // the +4 is the offset to align the center of paddle with the ball
        if(enemyPaddle+4 < ballPositionY/8){
            enemyPaddle += 1;

        // if the paddle position is below than ball position, the paddle will move up
        }else if(enemyPaddle+4 > ballPositionY/8){ 
            enemyPaddle -= 1;
        }

        // the player paddle limit of movement on top of screen
        if(paddle < 24){
            paddle = 24;
        }

        // the player paddle limit of movement on botton of screen
        if(paddle > 128){
            paddle = 128;
        }

        // the enemy player paddle limit of movement on top of screen
        if(enemyPaddle < 24){
            enemyPaddle = 24;
        }

        // the enemy paddle limit of movement on botton of screen
        if(enemyPaddle > 128){
            enemyPaddle = 128;
        }

        // update routine
        Render(); //update all transformations in sprites
        delay(16); //run the game next to 60 fps
        
    }    
}