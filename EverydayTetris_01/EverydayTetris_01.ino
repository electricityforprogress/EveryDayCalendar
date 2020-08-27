//Everyday Tetris
//Everyday Calendar implementation of Tetris game (2013 Valentin Ivanov)
//Play field is usually 10 wide, we have 12 cols
//     effectively 28 rows (darn Feb), pretty short tetris

//controls are left, right, rotate, drop
// left    - x: 2 y: 28
// rotLeft - x: 0 y: 29
// right   - x: 11 y: 28
// rotRight- x: 11 y: 29
// down    - x: 11 y: 30
// drop    - x: 5 y: 29

//score ... hmm perhpas some method
//leveling and speed increase ... hmm

#include "GamePiece.h"
#include <EverydayCalendar_lights.h>
#include <EverydayCalendar_touch.h>

EverydayCalendar_touch cal_touch;
EverydayCalendar_lights cal_lights;
int16_t brightness = 128;

const int GAME_COLUMNS = 12;
const int GAME_ROWS = 28;
int DLY = 210;
byte gameField[GAME_COLUMNS*GAME_ROWS];

// Movement SETUP
int prevBtn = 1;
int prevX = 0;
int prevY = 0;
int bri = 32;

int delayTime = 5;

unsigned long loopStartTime = 0;
byte p1[4] = {1, 1, 1, 1};  
byte p2[6] = {2, 2, 2, 0, 2, 0};
byte p3[6] = {3, 0, 3, 0, 3, 3};
byte p4[6] = {0, 4, 0, 4, 4, 4};
byte p5[6] = {5, 5, 0, 0, 5, 5};
byte p6[6] = {0, 6, 6, 6, 6, 0};
byte p7[4] = {7, 7, 7, 7 };

GamePiece  _gamePieces[7] = 
{
  GamePiece(2, 2, p1 ),
  GamePiece(3, 2, p2 ),
  GamePiece(3, 2, p3 ),
  GamePiece(2, 3, p4 ),
  GamePiece(2, 3, p5 ),
  GamePiece(2, 3, p6 ),
  GamePiece(4, 1, p7 )
};

GamePiece * fallingPiece = NULL;
GamePiece * rotated = NULL;
GamePiece * nextPiece = NULL;

byte gameLevel = 1;
byte currentRow = 0;
byte currentColumn = 0;
byte gameLines = 0;
boolean gameOver = false;
boolean displayMode = true;

typedef struct {
   int8_t    x;
   int8_t    y;
} Point;


void setup() {
  Serial.begin(115200);
  Serial.println("Everyday Tetris - Welcome!");
    
  randomSeed(analogRead(A0));
  // Initialize LED functionality
  cal_lights.configure();
  cal_lights.setBrightness(200);
  cal_lights.begin();
  // Initialize touch functionality
  cal_touch.configure();
  cal_touch.begin();
  
  startGame();

}

void loop() {

  //scan for touches, move on release

  manageTouch();

   //play game
    if( millis() - loopStartTime > (300 / (gameLevel * 0.40)) )  {
      if( !gameOver ) {        
        moveDown();
        gameOver = !isValidLocation(*fallingPiece, currentColumn, currentRow);       
        if(gameOver) exec_gameOver();
      }
      loopStartTime = millis(); //update gametime
      updateDisplay();
      //serialPlayfield();
    }
    
  
  //</playgame>

  //updateDisplay();  //now that the game loop passed and rendered, show it!
}
