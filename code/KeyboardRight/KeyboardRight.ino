#include <Wire.h>
#include <Keyboard.h>

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])

#define KEY_DOWN 0x01
#define KEY_UP 0x02

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

bool buttonState[ROW_COUNT][COLUMN_COUNT];
byte messageData[2];

void sendMessageData(byte type, int x, int y)
{
  messageData[0] = type;
  messageData[1] = (0xF & x) << 4;
  messageData[1] &= 0xF0;
  messageData[1] |= (0xF & y);
  sendData();
}

/*
 * Data should be in the format of 2 bytes.
 * The first saying KEY_UP or KEY_DOWN
 * The send is x and y together with x being the top 4 bits
 * and bottom being the bottom 4 bits
 */
void sendData()
{
  Wire.beginTransmission(1);
  Wire.write(messageData, 2);
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

void setup()
{
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
      buttonDirection buttonDir = getButtonDirection(x, y);
      if(buttonDir == FIRST_DOWN)
      {
        sendMessageData(KEY_DOWN, x, y);
      }
      else if(buttonDir == FIRST_UP)
      {
        sendMessageData(KEY_UP, x, y);
      }
    }
    digitalWrite(columnPins[y], LOW);
  }
}
