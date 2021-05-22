#include <Wire.h>
#include <Keyboard.h>

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])

#define KEY_DOWN 0x01
#define KEY_UP 0x02
#define SPECIAL_DOWN 0x03
#define SPECIAL_UP 0x04

#define COLUMN_COUNT 11
int columnPins[COLUMN_COUNT] = {10, 16, 14, 15, 18, 19, 20, 21, 1, 0, 4};

#define ROW_COUNT 5
int rowPins[ROW_COUNT] = {5, 6, 7, 8, 9};

enum buttonDirection
{
  FIRST_DOWN,
  FIRST_UP,
  DOWN,
  UP
};

char buttons[ROW_COUNT][COLUMN_COUNT] = 
  {
    {0, '6', '7', '8', '9', '0', 0, 0, '/', '*', '-' },
    {']', 'y', 'u', 'i', 'o', 'p', '\\', '7', '8', '9', '+' },
    {-1, 'h', 'j', 'k', 'l', ';', '\'', '4', '5', '6', -4 },
    {-2, 'n', 'm', ',', '.', '/', -3, '1', '2', '3', '.' },
    {' ', -4, -5, -6, -7, -8, -9, 0, 0, 0, '0'}
  };

char specialKeys[] =
  {
    0x00,
    KEY_BACKSPACE,   //-1
    KEY_DELETE,      //-2
    KEY_RIGHT_SHIFT, //-3
    KEY_RETURN,      //-4
    KEY_RIGHT_ALT,   //-5
    KEY_LEFT_ARROW,  //-6
    KEY_DOWN_ARROW,  //-7
    KEY_UP_ARROW,    //-8
    KEY_RIGHT_ARROW  //-9
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

void sendDownMessage(char key)
{
  if(key < 0)
  {
    key = getSpecialKey(key); 
    sendSpecialMessage(true, key);
  }
  else
  {
    sendKeyMessage(true, key);
  }
}

void sendUpMessage(char key)
{
  if(key < 0)
  {
    key = getSpecialKey(key); 
    sendSpecialMessage(false, key);
  }
  else
  {
    sendKeyMessage(false, key);
  }
}

void sendKeyMessage(bool down, char key)
{
  if(down)
  {
    sendMessage(KEY_DOWN, key);
  }
  else
  {
    sendMessage(KEY_UP, key);
  }
}

void sendSpecialMessage(bool down, char key)
{
  if(down)
  {
    sendMessage(SPECIAL_DOWN, key);
  }
  else
  {
    sendMessage(SPECIAL_UP, key);
  }
}

void sendMessage(byte type, char key)
{
  Wire.beginTransmission(1);
  Wire.write(type);
  Wire.write(key);
  Wire.endTransmission(true);
}

buttonDirection getButtonDirection(int x, int y)
{
  if(digitalRead(rowPins[x]) == HIGH)
  {
    if(!buttonState[x][y])
    {
      buttonState[x][y] = true;
      return FIRST_DOWN;
    }
    return DOWN;
  }

  if(buttonState[x][y])
  {
    buttonState[x][y] = false;
    return FIRST_UP;
  }
  return UP;
}

void setup() {
  Wire.begin();

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
      if(buttons[x][y] == 0)
      {
        continue;
      }
      char key = buttons[x][y];
      buttonDirection buttonDir = getButtonDirection(x, y);
      if(buttonDir == FIRST_DOWN)
      {
        sendDownMessage(key);
      }
      else if(buttonDir == FIRST_UP)
      {
        sendUpMessage(key);
      }
    }
    digitalWrite(columnPins[y], LOW);
  }
}
