#include <Keyboard.h>
#include <Wire.h>

#define NUM_SLASH '\334'
#define NUM_STAR  '\335'
#define NUM_MINUS '\336'
#define NUM_PLUS  '\337'
#define NUM_ENTER '\340'
#define NUM_1     '\341'
#define NUM_2     '\342'
#define NUM_3     '\343'
#define NUM_4     '\344'
#define NUM_5     '\345'
#define NUM_6     '\346'
#define NUM_7     '\347'
#define NUM_8     '\350'
#define NUM_9     '\351'
#define NUM_0     '\352'
#define NUM_DOT   '\353'

#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])
#define GET_KEY(l, t, x, y) (*buttons[l].t)[x][y]

#define KEY_DOWN 0x01
#define KEY_UP 0x02

#define RIGHT_COLUMN_COUNT 11
#define COLUMN_COUNT 7
int columnPins[COLUMN_COUNT] = {20, 19, 18, 15, 14, 16, 10};

#define ROW_COUNT 5
int rowPins[ROW_COUNT] = {8, 9, 7, 6, 4};

enum side
{
  LEFT,
  RIGHT
};

enum buttonDirection
{
  FIRST_DOWN,
  FIRST_UP,
  DOWN,
  UP
};

typedef char leftButtons[ROW_COUNT][COLUMN_COUNT];
typedef char rightButtons[ROW_COUNT][RIGHT_COLUMN_COUNT];

struct layerButtons
{
  leftButtons* left;
  rightButtons* right;
};

struct layerConfig
{
  side s;
  int x;
  int y;
  int layerIndex;
};

leftButtons defaultLeft = 
{
  {'`', '1', '2', '3', '4', '5', 0 },
  {KEY_TAB, 'q', 'w', 'e', 'r', 't', '[' },
  {KEY_ESC, 'a', 's', 'd', 'f', 'g', KEY_BACKSPACE },
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', KEY_DELETE },
  {KEY_LEFT_CTRL, '-', '=', KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RETURN }
};

rightButtons defaultRight = 
{
  {0, '6', '7', '8', '9', '0', KEY_RIGHT_CTRL, 0, NUM_SLASH, NUM_STAR, NUM_MINUS },
  {']', 'y', 'u', 'i', 'o', 'p', '\\', '7', NUM_8, NUM_9, NUM_PLUS },
  {KEY_BACKSPACE, 'h', 'j', 'k', 'l', ';', '\'', NUM_4, NUM_5, NUM_6, NUM_ENTER },
  {KEY_DELETE, 'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT, NUM_1, NUM_2, NUM_3, NUM_DOT },
  {KEY_RETURN, ' ', KEY_RIGHT_ALT, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, 0, 0, 0, NUM_0 }
};

leftButtons functionLeft =
{
  {0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, 0 },
  {0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0 }
};

rightButtons functionRight =
{
  {0, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int activeLayer = 0;

layerButtons buttons[]
{
  { &defaultLeft, &defaultRight },
  { &functionLeft, &functionRight }
};

layerConfig layerConfigs[]
{
  {LEFT, 0, 6, 1}
};

bool buttonState[ROW_COUNT][COLUMN_COUNT];

int getLayerKey(side s, int x, int y)
{
  for(int i = 0; i < SIZE_OF_ARRAY(layerConfigs); i++)
  {
    layerConfig current = layerConfigs[i];
    if(current.y == y && current.x == x && current.s == s)
    {
      return current.layerIndex;
    }
  }
  return -1;
}

char getKey(side s, buttonDirection dir, int x, int y)
{
  int layerIndex = getLayerKey(s, x, y);
  if(layerIndex > 0)
  {
    if(dir == FIRST_DOWN)
    {
      activeLayer = layerIndex;
    }
    else if(dir == FIRST_UP)
    {
      activeLayer = 0;
    }
  }

  if(s == LEFT)
  {
    int key = GET_KEY(activeLayer, left, x, y);
    if(activeLayer != 0 && key == 0)
    {
      key = GET_KEY(0, left, x, y);
    }
    return key;
  }
  return GET_KEY(activeLayer, right, x, y);
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
  buttonDirection dir = UP;
  while(Wire.available())
  {
    byte b = Wire.read();
    //Commands come in pairs, first is type, second is key
    if(readNumber % 2 == 0)
    {
      switch(b)
      {
        case KEY_DOWN:
          dir = FIRST_DOWN;
          break;
        case KEY_UP:
          dir = FIRST_UP;
          break;
      }
    }
    else
    {
      int x = (0xF0 & b) >> 4;
      int y = (0x0F & b);
      char key = getKey(RIGHT, dir, x, y);
      if(dir == FIRST_UP)
      {
        Keyboard.release(key);
      }
      else if(dir == FIRST_DOWN)
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
      char key = getKey(LEFT, dir, x, y);
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
