

float Normalize(int min, int max, int value)
{
  float result = -1.0 + (float)((value - min) << 1) / (max - min);
  return result < -1 ? -1 : result > 1 ? 1 : result;
}

//*** super fresh code
void updateDisplay() {
  //drive Everyday Calendar using library functions
  //draw the game field and then draw the piece
  for( int row = 0; row < GAME_ROWS; row++) {
    for( int col = 0; col < GAME_COLUMNS; col++) {
      if(gameField[row * GAME_COLUMNS + col]) {
        //turn on light
        cal_lights.setLED(col, row, 1);
      }
      else cal_lights.setLED(col, row, 0); //turn light off
    }
  }
  for( int row = 0; row < fallingPiece->Rows; row++) {
    for( int col = 0; col < fallingPiece->Columns; col++) {
        if(fallingPiece->getV(row,col)) cal_lights.setLED(currentColumn+col,currentRow+row, 1);              
    }
  }
}



void serialPlayfield() {
   //serial monitor print the board
//write it all to a buffer and then print it
  boolean buffer[GAME_ROWS][GAME_COLUMNS];
  //clear buffer
  for( int row = 0; row < GAME_ROWS; row++)
  {
    for( int col = 0; col < GAME_COLUMNS; col++)
    {      
      buffer[row][col]=false;
    }
  }
  
  for( int row = 0; row < fallingPiece->Rows; row++)
  {
    for( int col = 0; col < fallingPiece->Columns; col++)
    {
        if(fallingPiece->getV(row,col)) buffer[currentRow+row][currentColumn+col] = true; 
    }
  }

//draw playfield
    Serial.println("-----------");
  for( int row = 0; row < GAME_ROWS; row++)
  {
    Serial.print("| ");
    for( int col = 0; col < GAME_COLUMNS; col++)
    {      
      if(buffer[row][col]) Serial.print("X");
      else{
        if(gameField[row * GAME_COLUMNS+col]) 
          Serial.print("#");
          else Serial.print(".");
        }  
    }
    
    Serial.println(" |");
  }
      Serial.println("-----------");
}//this is a mess!!!


void manageTouch(){
  //modify simone's code to have control buttons
  // left, right, rotatate L, rotate R, down, drop, reset
  
  static Point previouslyHeldButton = {0xFF, 0xFF}; // 0xFF and 0xFF if no button is held
  static uint16_t touchCount = 1;
  static const uint8_t debounceCount = 1;
  static const uint16_t clearCalendarCount = 200; // ~20 seconds.  This is in units of touch sampling interval ~= 30ms.  
  Point buttonPressed = {0xFF, 0xFF};
  bool touch = cal_touch.scanForTouch();
  // Handle a button press
  if(touch)
  {
    // Brightness Buttons
    if(cal_touch.y == 31){
      if(cal_touch.x == 4){
        brightness -= 3;
      }else if(cal_touch.x == 6){
        brightness += 2;
      }
      brightness = constrain(brightness, 0, 200);
      Serial.print("Brightness: ");
      Serial.println(brightness);
      cal_lights.setBrightness((uint8_t)brightness);
    }


    
    // If all buttons aren't touched, reset debounce touch counter
    if(previouslyHeldButton.x == -1){
      touchCount = 0;
    }

    // If this button is been held, or it's just starting to be pressed and is the only button being touched
    if(((previouslyHeldButton.x == cal_touch.x) && (previouslyHeldButton.y == cal_touch.y))
    || (debounceCount == 0))
    {
      // The button has been held for a certain number of consecutive checks 
      // This is called debouncing
      if (touchCount == debounceCount){
        // Button is activated
        cal_lights.toggleLED((uint8_t)cal_touch.x, (uint8_t)cal_touch.y);
        
        Serial.print("x: ");
        Serial.print(cal_touch.x);
        Serial.print("\ty: ");
        Serial.println(cal_touch.y);
      }

//controls are left, right, rotate, drop
// left    - x: 0 y: 28
// rotLeft - x: 0 y: 29
// right   - x: 11 y: 28
// rotRight- x: 11 y: 29
// down    - x: 11 y: 30
// drop    - x: 5 y: 29
   if(debounceCount > 0) {
      if((cal_touch.x == 0) && (cal_touch.y == 28)) { //left
        moveLeft();
      }
      if((cal_touch.x == 0) && (cal_touch.y == 29)) { //rotLeft
        rotateLeft();
      }
      if((cal_touch.x == 11) && (cal_touch.y == 28)) { //right
       moveRight(); 
      }
      if((cal_touch.x == 11) && (cal_touch.y == 29)) { //rotRight
        rotateRight();
      }
      if((cal_touch.x == 5) && (cal_touch.y == 29)) { //drop
        drop();
      }    
      if((cal_touch.x == 5) && (cal_touch.y == 29)) { //down
        moveDown();
      }                    
   }
   
      // Check if the special "Reset" January 1 button is being held
      if((cal_touch.x == 11) && (cal_touch.y == 0) && (touchCount == clearCalendarCount)){
        exec_gameOver(); //reset game
      }
      
      if(touchCount < 65535){
        touchCount++;
        Serial.println(touchCount);
      }
    }
  }

  previouslyHeldButton.x = cal_touch.x;
  previouslyHeldButton.y = cal_touch.y;
}

void startGame()
{
  Serial.println("Start game");

// start game lightshow?

  //bar.clear();
  //bar.writeDisplay();

  nextPiece=NULL;
  gameLines = 0;
  loopStartTime = 0;
  newLevel(1);
  gameOver = false;
//  render();


}


void newLevel(uint8_t level)
{
  gameLevel = level;

  //bar.clear();
  //for(int b = 12;b > 12-level;b--) //bar.setBar(b-1, LED_RED);
  //bar.writeDisplay();

  if(gameLevel == 1)
  {
    emptyField();
  }

  newPiece();
}


void emptyField()
{
  for(int i = 0; i < GAME_ROWS * GAME_COLUMNS; i++ ) gameField[i] = 0;
}


void newPiece()
{
  int next;

  currentColumn = 4;
  currentRow = 0;

  if (nextPiece == NULL)
  {
    next = random(0,7);
    nextPiece = &_gamePieces[next];
  }

    if(fallingPiece != NULL) {free(fallingPiece->_data);delete fallingPiece;};
    fallingPiece = new GamePiece(*nextPiece);

  next = random(0,7);
  nextPiece = &_gamePieces[next];  

  Serial.print("Next: "); Serial.println(next);
}


boolean isValidLocation(GamePiece & piece, byte column, byte row)
{
  for (int i = 0; i < piece.Rows; i++)
    for (int j = 0; j < piece.Columns; j++)
    {
      int newRow = i + row;
      int newColumn = j + column;                    

      //location is outside of the fieled
      if (newColumn < 0 || newColumn > GAME_COLUMNS - 1 || newRow < 0 || newRow > GAME_ROWS - 1)
      {
        //piece part in that location has a valid square - not good
        if (piece(i, j) != 0) return false;
      }
      else
      {
        //location is in the field but is already taken, pice part for that location has non-empty square 
        if (gameField[newRow*GAME_COLUMNS + newColumn] != 0 && piece(i, j) != 0) return false;
      }
    }

  return true;  
}


void moveDown()
{
  if (isValidLocation(*fallingPiece, currentColumn, currentRow + 1))
  {
    currentRow +=1;
    updateDisplay();
    return;
  }


  //The piece can't be moved anymore, merge it into the game field
  for (int i = 0; i < fallingPiece->Rows; i++)
  {
    for (int j = 0; j < fallingPiece->Columns; j++)
    {
      byte value = (*fallingPiece)(i, j);
      if (value != 0) gameField[(i + currentRow) * GAME_COLUMNS + (j + currentColumn)] = value;
    }
  }

  //Piece is merged update the score and get a new pice
  updateScore();            
  newPiece();  

  Serial.println("MoveDown");
}


void drop()
{
  while (isValidLocation(*fallingPiece, currentColumn, currentRow + 1)) moveDown();
  loopStartTime = -500;
  updateDisplay();
  delay(DLY);  
}

void moveLeft()
{
  if (isValidLocation(*fallingPiece, currentColumn - 1, currentRow)) currentColumn--;
  updateDisplay();
  delay(DLY);  
}



void moveRight()
{
  if (isValidLocation(*fallingPiece, currentColumn + 1, currentRow)) currentColumn++;
  updateDisplay();
  delay(DLY);  

}


void rotateRight()
{
    rotated = fallingPiece->rotateRight();

  if (isValidLocation(*rotated, currentColumn, currentRow)) 
  {
      free(fallingPiece->_data);
      delete fallingPiece;

      fallingPiece = rotated;
  } else {free(rotated->_data);delete rotated;};

  updateDisplay();
  delay(DLY);  
}


void rotateLeft()
{
  GamePiece * rotated = fallingPiece->rotateLeft();

  if (isValidLocation(*rotated, currentColumn, currentRow)) 
  {
     delete fallingPiece;
     fallingPiece = rotated;
  }
  updateDisplay();
  delay(DLY);  
}


void updateScore()
{
  int count = 0;
  for(int row = 1; row < GAME_ROWS; row++)
  {
    boolean goodLine = true;
    for (int col = 0; col < GAME_COLUMNS; col++) if(gameField[row *GAME_COLUMNS + col] == 0) goodLine = false;

    if(goodLine)
    {
      count++;
      for (int i = row; i > 0; i--)
      {
        for (int j = 0; j < GAME_COLUMNS; j++) gameField[i *GAME_COLUMNS +j] = gameField[(i - 1)*GAME_COLUMNS+ j];
    //    render();
      }
    }
  }


  if (count > 0)
  {
    //_gameScore += count * (_gameLevel * 10);
    gameLines += count;


    int nextLevel = (gameLines / GAME_ROWS) + 1;
    int t = gameLines-(gameLines/GAME_ROWS*16);
    t = 24 - map(t,1,16,1,13);
    //for(int b = 24;b >= t;b--) //bar.setBar(b, LED_GREEN);
    //bar.writeDisplay();


    if (nextLevel > gameLevel)
    {
      gameLevel = nextLevel;
      newLevel(gameLevel);
      /****
       * create a next level light show
       * flash and show score and animate
       * clear and start new level
       */
    }
  }
}

void exec_gameOver()
{
 // delay(100);
 //****
 //create a game over light show!!!
 //clear the LEDs to fade out, or just let
 //the new playfield pop on startGame();
 //***
  
  Serial.println("GameOver");
  startGame();
}


int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
