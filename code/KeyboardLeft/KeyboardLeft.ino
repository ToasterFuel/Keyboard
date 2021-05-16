#include <Keyboard.h>

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])


//In ms
#define TIME_TO_INITIAL_REPEAT 800
#define TIME_TO_SEQUENTIAL_REPEAT 25

#define COLUMN_COUNT 7
int columnPins[COLUMN_COUNT] = {20, 19, 18, 15, 14, 16, 10};

#define ROW_COUNT 5
int rowPins[ROW_COUNT] = {8, 9, 7, 6, 4};

enum buttonDirection
{
  DOWN,
  UP,
  HOLD
};

//TODO add function key for the 0 character
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

char repeatableSpecial[] =
  {
    KEY_BACKSPACE,
    KEY_DELETE,
    KEY_TAB,
    KEY_RETURN,
    KEY_ESC
  };

bool buttonState[ROW_COUNT][COLUMN_COUNT];

int timeToRepeatPress = TIME_TO_INITIAL_REPEAT;

int lastXPressed = -1;
int lastYPressed = -1;
unsigned long timePressed = 0;

bool isRepeatableSpecial(char key)
{
  if(key >= 0)
  {
    return false;
  }
  for(int i = 0; i < SIZE_OF_ARRAY(repeatableSpecial); i++)
  {
    if(repeatableSpecial[i] == key)
    {
      return true;
    }
  }
  return false;
}

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
    if(lastXPressed == x && lastYPressed == y)
    {
      lastXPressed = -1;
      lastYPressed = -1;
    }

    return UP;
  }

  char key = buttons[x][y];
  if(key < 0)
  {
    key = getSpecialKey(key);
    if(!isRepeatableSpecial(key))
    {
      if(buttonState[x][y])
      {
        return HOLD;
      }
      return DOWN;
    }
  }

  if(!buttonState[x][y])
  {
    lastXPressed = x;
    lastYPressed = y;
    timePressed = millis();
    timeToRepeatPress = TIME_TO_INITIAL_REPEAT;
    
    return DOWN;
  }

  if(!(lastXPressed == x && lastYPressed == y))
  {
    return HOLD;
  }

  if(millis() - timePressed > timeToRepeatPress)
  {
    timePressed = millis();
    timeToRepeatPress = TIME_TO_SEQUENTIAL_REPEAT;
    return DOWN;
  }

  return HOLD;
}

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

void loop()
{
  for(int y = 0; y < COLUMN_COUNT; y++)
  {
    digitalWrite(columnPins[y], HIGH);
    for(int x = 0; x < ROW_COUNT; x++)
    {
      char key = buttons[x][y];
      buttonDirection dir = isButtonPressed(x, y);
      if(dir == DOWN)
      {
        if(key < 0)
        {
          key = getSpecialKey(key);
          if(isRepeatableSpecial(key))
          {
            Keyboard.write(key);
          }
          else if(!buttonState[x][y])
          {
            Keyboard.press(key);
          }
        }
        else
        {
          Keyboard.write(key);
        }
        buttonState[x][y] = true;
      }
      else if(dir == UP)
      {
        if(key < 0)
        {
          key = getSpecialKey(key);
          if(buttonState[x][y] && !isRepeatableSpecial(key))
          {
            Keyboard.release(key);
          }
        }
        buttonState[x][y] = false;
      }
    }
    digitalWrite(columnPins[y], LOW);
  }
}
