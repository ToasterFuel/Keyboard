#include <Keyboard.h>

//In ms
#define TIME_TO_INITIAL_REPEAT 1000
#define TIME_TO_SEQUENTIAL_REPEAT 25

#define COLUMN_COUNT 2
int columnPins[COLUMN_COUNT] = {10, 16};

#define ROW_COUNT 2
int rowPins[ROW_COUNT] = {9, 8};

char buttons[ROW_COUNT][COLUMN_COUNT] = 
  {
    {'0', '1'},
    {'2', '3'},
  };

bool buttonState[ROW_COUNT][COLUMN_COUNT];

int timeToRepeatPress = TIME_TO_INITIAL_REPEAT;

void setup()
{
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

int lastXPressed = -1;
int lastYPressed = -1;
unsigned long timePressed = 0;

//TODO take into account modifiers?
bool isButtonPressed(int x, int y)
{
  if(digitalRead(rowPins[x]) != HIGH)
  {
    buttonState[x][y] = false;
    if(lastXPressed == x && lastYPressed == y)
    {
      lastXPressed = -1;
      lastYPressed = -1;
    }

    return false;
  }

  if(!buttonState[x][y])
  {
    lastXPressed = x;
    lastYPressed = y;
    timePressed = millis();
    buttonState[x][y] = true;
    timeToRepeatPress = TIME_TO_INITIAL_REPEAT;
    
    return true;
  }

  if(!(lastXPressed == x && lastYPressed == y))
  {
    return false;
  }

  if(millis() - timePressed > timeToRepeatPress)
  {
    timePressed = millis();
    timeToRepeatPress = TIME_TO_SEQUENTIAL_REPEAT;
    return true;
  }

  return false;
}

void loop()
{
  for(int y = 0; y < COLUMN_COUNT; y++)
  {
    digitalWrite(columnPins[y], HIGH);
    for(int x = 0; x < ROW_COUNT; x++)
    {
      if(isButtonPressed(x, y))
      {
        Keyboard.write(buttons[x][y]);
      }
    }
    digitalWrite(columnPins[y], LOW);
  }
}
