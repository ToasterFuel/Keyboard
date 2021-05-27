#include <Keyboard.h>
#include <Wire.h>

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])

#define KEY_DOWN 0x01
#define KEY_UP 0x02

#define RIGHT_COLUMN_COUNT 11
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

char buttonsLeft[ROW_COUNT][COLUMN_COUNT] = 
  {
    {'`', '1', '2', '3', '4', '5', 0 },
    {KEY_TAB, 'q', 'w', 'e', 'r', 't', '[' },
    {KEY_ESC, 'a', 's', 'd', 'f', 'g', KEY_BACKSPACE },
    {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', KEY_DELETE },
    {KEY_LEFT_CTRL, '-', '=', KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RETURN }
  };


char buttonsRight[ROW_COUNT][RIGHT_COLUMN_COUNT] = 
  {
    {0, '6', '7', '8', '9', '0', KEY_RIGHT_CTRL, 0, '/', '*', '-' },
    {']', 'y', 'u', 'i', 'o', 'p', '\\', '7', '8', '9', '+' },
    {KEY_BACKSPACE, 'h', 'j', 'k', 'l', ';', '\'', '4', '5', '6', KEY_RETURN },
    {KEY_DELETE, 'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT, '1', '2', '3', '.' },
    {KEY_RETURN, ' ', KEY_RIGHT_ALT, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, 0, 0, 0, '0'}
  };

bool buttonState[ROW_COUNT][COLUMN_COUNT];

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
      }
    }
    else
    {
      int x = (0xF0 & b) >> 4;
      int y = (0x0F & b);
      char key = buttonsRight[x][y];
      if(isKeyUp)
      {
        Keyboard.release(key);
      }
      else
      {
        Keyboard.press(key);
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
      buttonDirection dir = isButtonPressed(x, y);
      char key = buttonsLeft[x][y];
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
