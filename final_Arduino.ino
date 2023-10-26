#include <LiquidCrystal.h>

#define joyX A0
#define joyY A1
#define joystickButtonPin 4  // Digital pin for the joystick button

struct JoystickState {
  int button;
  int direction;
};
int currentScreen = 0;   // 0 = main menu, 1 = Classic, 2 = Time Lapse, 3 = Paused, 4 = End Screen
int subScreenValue = 0;  // 0 = Classic, 1 = Time Lapse
int score = 0;
int shipPosition = 0;  //x coordinate of space ship
int tick = 100;        //tick rate
int currentFrame = 0;
int hearts = 3;
int UFOsXPOS[3] = { -1, -1, -1 };  //-1 = no UFO, 0-15 = xPos
//infoForBullet {0,0,0} index 0: bullet Flying? index 1: LCD number, index 2: xPos index 3: frame #
int infoForBullet[3][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};
int currentNumberOfBullets = 0;
byte ship[8] = {
  B00000,
  B00000,
  B00100,
  B10101,
  B11111,
  B01010,
  B00000,
  B00000,
};
byte ufo[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B10101,
  B01110,
  B00000,
};
byte shoot1[8] = {
  B00000,
  B00000,
  B10101,
  B10101,
  B11111,
  B01010,
  B00000,
};
byte shoot2[8] = {
  B00000,
  B10001,
  B00100,
  B10101,
  B11111,
  B01010,
  B00000,
};
byte shoot3[8] = {
  B10001,
  B00000,
  B00100,
  B10101,
  B11111,
  B01010,
  B00000,
};
byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};

byte bulletFlying[7][8] = {
  {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
  },
  {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
    B00000,
  },
  {
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
    B00000,
    B00000,
  },
  {
    B00000,
    B00000,
    B00000,
    B10001,
    B00000,
    B00000,
    B00000,
  },
  {
    B00000,
    B00000,
    B10001,
    B00000,
    B00000,
    B00000,
    B00000,
  },
  {
    B00000,
    B10001,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
  },
  {
    B10001,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
  }
};


LiquidCrystal lcd(27, 26, 25, 24, 23, 22);
LiquidCrystal lcd2(13, 12, 11, 10, 9, 8);
void setup() {
  Serial.begin(9600);
  pinMode(joystickButtonPin, INPUT_PULLUP);  // Set the button pin as an input with a pull-up resistor
  lcd.begin(16, 2);
  lcd2.begin(16, 2);
  //define custom Characters for LED
  lcd.createChar(0, ufo);
  lcd.createChar(1, heart);

  lcd2.createChar(0, ship);
  lcd2.createChar(1, shoot1);
  lcd2.createChar(2, shoot2);
  lcd2.createChar(3, shoot3);
}

void loop() {
  JoystickState _joystickState = joystickState();
  //Serial.println(_joystickState.button);
  //Serial.println(_joystickState.direction);
  //lcd.print(_joystickState.button);
  //lcd.print(_joystickState.direction);
  /*if(currentScreen == 0){
    mainMenu(_joystickState.button, _joystickState.direction);
  }else if(currentScreen == 1){
    classicGame(_joystickState.button, _joystickState.direction);
  }else if(currentScreen == 2){
    timeLapse(_joystickState.button, _joystickState.direction);
  }*/
  classicGame(_joystickState.button, _joystickState.direction);

  delay(tick);
}

JoystickState joystickState() {
  // Read the analog values of the joystick axes
  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);
  // Read the state of the joystick button
  int buttonState = digitalRead(joystickButtonPin);
  // Define threshold values for left, right, up, and down
  int threshold = 100;
  JoystickState state;

  // Check if the joystick button is clicked
  if (buttonState == LOW) {
    state.button = 1;
  } else {
    state.button = 0;
  }

  // Check if the joystick is moved left, right, up, or down
  if (xValue < (512 - threshold)) {
    state.direction = 1;  // Left
  } else if (xValue > (512 + threshold)) {
    state.direction = 2;  // Right
  } else if (yValue < (512 - threshold)) {
    state.direction = 3;  // Up
  } else if (yValue > (512 + threshold)) {
    state.direction = 4;  // Down
  } else {
    state.direction = 0;  // Centered
  }
  return state;
}


void mainMenu(int clicked, int direction) {
  if (clicked == 1) {
    if (subScreenValue == 0) {
      currentScreen = 1;
    } else if (subScreenValue == 1) {
      currentScreen = 2;
    }
    clearScreens();
  } else {
    if (direction == 3) {
      subScreenValue = 0;
    } else if (direction == 4) {
      subScreenValue = 1;
    }
    lcd.setCursor(0, 0);
    if (subScreenValue == 0) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print("1. Classic");
    lcd2.setCursor(0, 0);
    if (subScreenValue == 1) {
      lcd2.print(">");
    } else {
      lcd2.print(" ");
    }
    lcd2.print("2. Time Lapse");
  }
}

int findIndex(int arr[], int size, int target) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == target) {
      return i; // Return the index of the element if found
    }
  }
  // If the element is not found, you can return -1 to indicate that it was not found.
  return -1;
}

int randomNumber() {
  int randomNum = random(0, 16);
  if (findIndex(UFOsXPOS, 3, randomNum) != -1) {
    randomNum = randomNumber();
  }
  return randomNum;
}

void classicGame(int clicked, int direction) {
  int overRideFrame = 0;
  if (clicked == 1) {
    if (currentFrame == 0 and currentNumberOfBullets < 3) {
      Serial.println("starting Shoot");
      currentFrame = 1;
      overRideFrame = 1;
    }
  }
  if (currentFrame != 0 and overRideFrame == 0) {
    currentFrame += 1;
  }
  if (currentFrame == 4) {
    currentFrame = 0;
    //find the next available bullet location in array
    for (int i = 0; i < 3; i++) {
      if (infoForBullet[i][0] == 0) {
        infoForBullet[i][0] = 1;
        infoForBullet[i][1] = 2;
        infoForBullet[i][2] = shipPosition;
        infoForBullet[i][3] = 0;
        currentNumberOfBullets += 1;
        break;
      }
    }
  }
  if (currentNumberOfBullets != 0) {
    //Serial.println("-----Chunk START-----");
    //Serial.println("currentNumberOfBullets:");
    //Serial.print(currentNumberOfBullets);
    //Serial.println(infoForBullet[0][0] + infoForBullet[1][0] + infoForBullet[2][0]);
    for(int i = 0; i < 3; i++){
      if(infoForBullet[i][0] == 1){
        bulletFlyingHandling(infoForBullet[i][3], infoForBullet[i][1], infoForBullet[i][2], i);
        infoForBullet[i][3] = infoForBullet[i][3] + 1;
        if (infoForBullet[i][3] > 7) {
          Serial.println("Hit Limit");
          if (infoForBullet[i][1] == 2) {
            //check if UFO is in the same position as bullet
            bool hit = false;
            for (int xx = 0; xx < 3; xx++) {
              if (UFOsXPOS[xx] == infoForBullet[i][2]) {
                //write a space over the UFO
                lcd.setCursor(UFOsXPOS[xx], 1);
                lcd.print(" ");
                UFOsXPOS[xx] = -1;
                score += 1;
                hit = true;
              }
            }
            if(hit == false){
            infoForBullet[i][1] = 1;//move bullet to LCD 1
            infoForBullet[i][3] = 0;
            }else{
              Serial.println("Hit UFO");
              infoForBullet[i][0] = 0;
              currentNumberOfBullets -= 1;
            }
          } else {
            infoForBullet[i][0] = 0;
            currentNumberOfBullets -= 1;
          }
        }
      }
    }
    //Serial.println("-----Chunk END-----");
  }
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.print(score);
  lcd.print(" ");
  lcd.print(hearts);
  lcd.write(byte(1));
  //UFO printing
  //check if any UFOs are in array
  int totalUFOs = 0;
  for (int i = 0; i < 3; i++) {
    if (UFOsXPOS[i] != -1) {
      lcd.setCursor(UFOsXPOS[i], 1);
      lcd.write(byte(0));
      totalUFOs += 1;
    }
  }
    // spawn new UFO
    for (int i = 0; i < 3; i++) {
      if (UFOsXPOS[i] == -1) {
        int randomXPOS = randomNumber();
        UFOsXPOS[i] = randomXPOS;
        break;
      }
    }
  


  lcd2.setCursor(shipPosition, 1);
  lcd2.print(" ");
  if (direction == 1) {
    if (shipPosition != 15) {
      shipPosition = shipPosition + 1;
    }
  } else if (direction == 2) {
    if (shipPosition != 0) {
      shipPosition = shipPosition - 1;
    }
  }
  lcd2.setCursor(shipPosition, 1);
  lcd2.write(byte(currentFrame));
}

void clearScreens() {
  lcd.clear();
  lcd2.clear();
}

void timeLapse(int clicked, int direction) {
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.print(score);
  lcd.print(" 3");
  lcd.write(byte(5));
}

void bulletFlyingHandling(int FrameRequested, int lcdnumber, int xPos, int bulletNumber) {
  Serial.println("-----Bullet START-----");
  Serial.println(FrameRequested);
  Serial.println(lcdnumber);
  Serial.println(xPos);
  Serial.println(bulletNumber); 
  Serial.println("-----Bullet END-----");

  if(FrameRequested > 7 or lcdnumber > 2 or xPos > 15 or bulletNumber > 2){
    Serial.println("ERROR");
    //kill ardunio
    while(true){
      lcd.clear();
      lcd2.clear();
      lcd.print("ERROR");
      lcd2.print("ERROR");
      delay(tick);
    }
  }

  int finalBulletNumber = 4 + (bulletNumber);
  //if FrameRequested is 7, than Clean up bullet
  if (FrameRequested == 7) {
    if (lcdnumber == 1) {
      lcd.setCursor(xPos, 1);
      lcd.print(" ");
    } else {
      lcd2.setCursor(xPos, 0);
      lcd2.print(" ");
    }
    return;
  }
  if (lcdnumber == 1) {
    lcd.createChar(finalBulletNumber, bulletFlying[FrameRequested]);
    lcd.setCursor(xPos, 1);
    lcd.write(byte(finalBulletNumber));
  } else {
    lcd2.createChar(finalBulletNumber, bulletFlying[FrameRequested]);
    lcd2.setCursor(xPos, 0);
    lcd2.write(byte(finalBulletNumber));
  }
}