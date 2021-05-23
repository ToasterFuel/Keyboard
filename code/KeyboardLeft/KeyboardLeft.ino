#include <Keyboard.h>
#include <Wire.h>

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])

#define KEY_DOWN 0x01
#define KEY_UP 0x02
#define SPECIAL_DOWN 0x03
#define SPECIAL_UP 0x04

#define COLUMN_COUNT 7
int columnPins[COLUMN_COUNT] = {20, 19, 18, 15, 14, 16, 10};

#define ROW_COUNT 5
int rowPins[ROW_COUNT] = {8, 9, 7, 6, 4};

enum buttonDirection
{
  FIRST_DOWN,
  FIRST_UP,
  DOWN,
  UP
};

//TODO add function key for the 0 character (possible the function keys??)
char buttons[ROW_COUNT][COLUMN_COUNT] = 
  {
    {'`', '1', '2', '3', '4', '5', 0 },
    {-4, 'q', 'w', 'e', 'r', 't', '[' },
    {-5, 'a', 's', 'd', 'f', 'g', -6 },
    {-1, 'z', 'x', 'c', 'v', 'b', -7 },
    {-3, '-', '=', -8, -9, ' ', -10 }
  };

char specialKeys[] =
  {
    0x00,
    KEY_LEFT_SHIFT, //-1
    KEY_LEFT_ALT,   //-2
    KEY_LEFT_CTRL,  //-3
    KEY_TAB,        //-4
    KEY_ESC,        //-5
    KEY_BACKSPACE,  //-6
    KEY_DELETE,     //-7
    KEY_LEFT_GUI,   //-8
    KEY_LEFT_ALT,   //-9
    KEY_RETURN     //-10
  };

bool buttonState[ROW_COUNT][COLUMN_COUNT];

char getSpecialKey(char key)
{
  if(key >= 0)
  {
    return key;
  }
  int index = -1 * key;
  if(index >= SIZE_OF_ARRAY(specialKeys))
  {
    return 0;
  }
  return specialKeys[index];
}

buttonDirection isButtonPressed(int x, int y)
{
  if(digitalRead(rowPins[x]) != HIGH)
  {
    if(buttonState[x][y])
    {
      buttonState[x][y] = false;
      return FIRST_UP;
    }

    return UP;
  }

  if(!buttonState[x][y])
  {
    buttonState[x][y] = true;
    return FIRST_DOWN;
  }

  return DOWN;
}

void masterSendingData(int numberOfBytes)
{
  int readNumber = 0;
  bool isKeyUp = false;
  while(Wire.available())
  {
    byte b = Wire.read();
    //Commands come in pairs, first is type, second is key
    if(readNumber % 2 == 0)
    {
      switch(b)
      {
        case KEY_DOWN:
          isKeyUp = false;
          break;
        case KEY_UP:
          isKeyUp = true;
          break;
        case SPECIAL_DOWN:
          isKeyUp = false;
          break;
        case SPECIAL_UP:
          isKeyUp = true;
          break;
      }
    }
    else
    {
      if(isKeyUp)
      {
        Keyboard.release(b);
      }
      else
      {
        Keyboard.press(b);
      }
    }
    readNumber += 1;
  }
}

void setup()
{
  Wire.begin(1);
  Wire.onReceive(masterSendingData);
  for(int i = 0; i < COLUMN_COUNT; i++)
  {
    pinMode(columnPins[i], OUTPUT);
    digitalWrite(columnPins[i], LOW);
  }
  for(int i = 0; i < ROW_COUNT; i++)
  {
    pinMode(rowPins[i], INPUT);
    digitalWrite(rowPins[i], LOW); 
  }
  for(int x = 0; x < ROW_COUNT; x++)
  {
    for(int y = 0; y < COLUMN_COUNT; y++)
    {
      buttonState[x][y] = false;
    }
  }
}

void loop()
{
  for(int y = 0; y < COLUMN_COUNT; y++)
  {
    digitalWrite(columnPins[y], HIGH);
    for(int x = 0; x < ROW_COUNT; x++)
    {
      char key = buttons[x][y];
      key = getSpecialKey(key);
      buttonDirection dir = isButtonPressed(x, y);
      if(dir == FIRST_DOWN)
      {
        Keyboard.press(key);
      }
      else if(dir == FIRST_UP)
      {
        Keyboard.release(key);
      }
    }
    digitalWrite(columnPins[y], LOW);
  }
}
