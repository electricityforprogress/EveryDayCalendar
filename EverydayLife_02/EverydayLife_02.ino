/***
 * Everyday Life - Conway's Game of Life on Simone's Everyday Calendar
 * width: 12
 * height: 28
 * controls for seeding, refresh, pause
 * time delay for refresh
 * touch to add a seed
 * July31 - slow down
 * Aug31 - speed up
 * Oct31 - turn off
 * Nov31 - refresh
 */

#include <EverydayCalendar_lights.h>
#include <EverydayCalendar_touch.h>


#define WORLDMAXX 28   //19 //length
#define WORLDMAXY 12 //height
byte world[WORLDMAXX+1][WORLDMAXY+1];
int worldCheck = 0;
int prevCheck = 0;
int checkCount = 0;
unsigned long prevMillis = 0;
unsigned long resetTime = 30000;
int stepcount;
int refreshRate = 535;
 
void initworld();
 
int isalive(int x, int y);
int countneighbor(int x, int y);


EverydayCalendar_touch cal_touch;
EverydayCalendar_lights cal_lights;
int16_t brightness = 5;


void setup() {
  randomSeed(analogRead(0));
  Serial.begin(115200);

  
  // Initialize LED functionality
  cal_lights.configure();
  cal_lights.setBrightness(brightness);
  cal_lights.begin();
  // Initialize touch functionality
  cal_touch.configure();
  cal_touch.begin();
   delay(1500);
  
  //  Serial.println("Everyday Life - Conway's Calendar");
     initworld();
}

void controlButtonLights() {
  cal_lights.setLED(6,30,1);
    cal_lights.setLED(7,30,1);
    cal_lights.setLED(9,30,1);
      cal_lights.setLED(11,30,1);
      
}

void loop() {
  int i, j;
 
  for (i = 0; i<= WORLDMAXX; i++) {
    for (j = 0; j<= WORLDMAXY; j++) {
      isalive(i,j);
    }
  }



  for (i = 0; i<= WORLDMAXX; i++) {
    for (j = 0; j<= WORLDMAXY; j++) {
      world[i][j] = world[i][j] >> 1;       
    }
  }


  //update display
  updateDisplay();
 

  //serialWorld();
  updateButtons();

  //serialWorld();

  if(prevMillis + resetTime < millis()) {
    initworld(); prevMillis = millis();
  }

  
  
  //change this to a sezy fade in and out of the prev and current board
  delay(refreshRate);


}

typedef struct {
   int8_t    x;
   int8_t    y;
} Point;

void updateDisplay() {

    // Fade out
//  for(int b = 200; b >= 0; b--){
//    cal_lights.setBrightness(b);
//    delay(4);
//  }
  
  for( int row = 0; row < WORLDMAXX; row++) {
    for( int col = 0; col < WORLDMAXY; col++) {
      if(world[row][col]>0) {
        //turn on light
        cal_lights.setLED(col, row, 1);
      }
      else cal_lights.setLED(col, row, 0); //turn light off
    }
  }

//show refresh rate at bottom
     for(byte i = 0; i < 8; i++) {
       if( map(refreshRate,0,1500,0,8) >= i ) cal_lights.setLED(i+2,29,1);
       else cal_lights.setLED(i+2,29,0);
     }
     controlButtonLights();
}

void updateButtons() {

     // Brightness Buttons and other controls

 static Point previouslyHeldButton = {0xFF, 0xFF}; // 0xFF and 0xFF if no button is held
  static uint16_t touchCount = 1;
  static const uint8_t debounceCount = 1;
  static const uint16_t clearCalendarCount = 200; // ~20 seconds.  This is in units of touch sampling interval ~= 30ms.  
  Point buttonPressed = {0xFF, 0xFF};
  bool touch = cal_touch.scanForTouch();
  // Handle a button press
  if(touch) {

    if(cal_touch.y <= 31) {
      world[cal_touch.y][cal_touch.x] = !world[cal_touch.y][cal_touch.x]; 
      Serial.print("Touch "); 
      Serial.print(cal_touch.x); Serial.print(","); Serial.println(cal_touch.y);
    }
 
    if(cal_touch.y == 31){
      if(cal_touch.x == 4){
        brightness -= 3;
      }else if(cal_touch.x == 6){
        brightness += 2;
      }
      brightness = constrain(brightness, 0, 200);
   //   Serial.print("Brightness: ");
   //   Serial.println(brightness);
      cal_lights.setBrightness((uint8_t)brightness);
    }

        // If all buttons aren't touched, reset debounce touch counter
    if(previouslyHeldButton.x == -1){
      touchCount = 0;
    }
    
    if(debounceCount > 0) {
      if((cal_touch.x == 11) && (cal_touch.y == 30)) {
        initworld();
      }
     if((cal_touch.x == 9) && (cal_touch.y==30)) {
       //turn all lights off, wait for any touch
       cal_lights.setBrightness(0);
       delay(1000);
       while(!cal_touch.scanForTouch()) {
        delay(50);
       }
       cal_lights.setBrightness(brightness);
     }
          if((cal_touch.x == 6) && (cal_touch.y==30)) {
       //slow down
       if(refreshRate <= 1500) refreshRate += 100;
       Serial.print("Refresh "); Serial.println(refreshRate);
     }
          if((cal_touch.x == 7) && (cal_touch.y==30)) {
       //speed up
       if(refreshRate >= 100) refreshRate -= 100;
       Serial.print("Refresh "); Serial.println(refreshRate);
     }
    }
  }
}

void serialWorld(){
 byte i = 0;
 byte j = 0;
    for (i=0; i<=WORLDMAXX; i++) {
      for (j=0; j<=WORLDMAXY; j++) {
      Serial.print(world[i][j]);
    } Serial.println();
  } Serial.println();
}

void initworld() {
  int i, j;
  for (i=0; i<=WORLDMAXX; i++) {
    for (j=0; j<=WORLDMAXY; j++) {
      world[i][j] = !random(3);
    }
    
  } 
  
  stepcount = 0;
  return;
}
 
// Calculate the next state for the square
int isalive(int x, int y) {
  int count = countneighbor(x, y);
  if (count == 2) { // 2 neighbors, stay the same
    world[x][y] += world[x][y] << 1;
  }
  if (count == 3) { // 3 neighbors, must be alive
    world[x][y] += 2;
  }
  // any other number of neighbors, must be dead
  return count;
}
 
int countneighbor(int x, int y) {
  int count = 0;
  if (x > 0) {
    count += world[x-1][y] & 1;
    if (y > 0) {
      count += world[x-1][y-1] & 1;
    }
    if (y < WORLDMAXY) {
      count += world[x-1][y+1] & 1;
    }
  }
  if (x < WORLDMAXX) {
    count += world[x+1][y] & 1;
    if (y > 0) {
      count += world[x+1][y-1] & 1;
    }
    if (y < WORLDMAXY) {
      count += world[x+1][y+1] & 1;
    }
  }
  if (y > 0) {
    count += world[x][y-1] & 1;
  }
  if (y < WORLDMAXY) {
    count += world[x][y+1] & 1;
  }
  
  return count;
}
 
