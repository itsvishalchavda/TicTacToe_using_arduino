
/******************* CODE BY **************/
/***************** V K CHAVDA *************/
/**************** TIC TAC TOE ***********/

// display state
// take input
// update state


#define D0 (0) 
#define D1 (1) 
#define D2 (2) 
#define D3 (3)
#define D4 (4)
#define D5 (5)
#define D6 (6)
#define D7 (7)
#define D8 (8)
#define D9 (9)
#define D10 (10) 
#define D11 (11) 
#define D12 (12) 
#define D13 (13) 

#define PIN_RGB1 D2
#define PIN_RGB2 D3
#define PIN_RGB3 D4
#define PIN_RGB4 D5
#define PIN_RGB5 D6
#define PIN_RGB6 D7
#define PIN_RGB7 D8
#define PIN_RGB8 D9
#define PIN_RGB9 D10

#define PIN_RED D11
#define PIN_GREEN D12
#define PIN_BLUE D13

#define PIN_LED_PLY1 D0 
#define PIN_LED_PLY2 D1

#define PIN_BCD_A A0
#define PIN_BCD_B A1
#define PIN_BCD_C A2
#define PIN_BCD_D A3

#define PIN_BTN_SELECT A4
#define PIN_BTN_LR A5
//#define PIN_BTN_UD A6

#define LED_ONOFF_TIME (1500) // us
#define LED_REFRESH_TIME (100) // us
#define RGB_ONOFF_TIME (1500) // us
#define RGB_REFRESH_TIME (100) // us
#define BLINK_COUNT (25) // 0-255
#define BTN_DEBOUNCE_DELAY (25) // ms


enum en_RGBState {rgb_off = 0, rgb_on = 1, rgb_blink = 2};
enum en_color {clr_none = 0, clr_red = 1, clr_green = 2, clr_blue = 3};
enum en_player {ply_none = 0, ply_player1 = 1, ply_player2 = 2};


struct Cell
{
  en_RGBState rgb;
  en_color clr;
  en_player ply;
  byte blink_count;
};

Cell* Board = NULL;
en_color Ply1Clr = clr_none;
en_color Ply2Clr = clr_none;

byte Ply1Score = 0;
byte Ply2Score = 0;

bool Ply1Turn = true;


void setup()
 {
  //Serial.begin(9600); // comment for tx/rx 
  //analogReference (EXTERNAL);
  //analogReference(DEFAULT);
  for(byte i = D0; i <= D13; i++) // D0 to D13
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  for(byte i = PIN_BCD_A; i <= PIN_BCD_D; i++) // BCD
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  pinMode(PIN_BTN_LR, INPUT_PULLUP);
  pinMode(PIN_BTN_SELECT, INPUT_PULLUP);
  //pinMode(PIN_BTN_UD, INPUT_PULLUP);
  
}


void SetColor(byte R, byte G, byte B)
{
  digitalWrite(PIN_RED, 255 - R);
  digitalWrite(PIN_GREEN, 255 - G);
  digitalWrite(PIN_BLUE, 255 - B);  
}

byte GetUserInput()
{
    ShowOutput();
    if(!digitalRead(PIN_BTN_SELECT)) 
    {
      for(byte i = 0; i <BTN_DEBOUNCE_DELAY; i++)
      {
        ShowOutput();
      }
      if(!digitalRead(PIN_BTN_SELECT)) 
      return PIN_BTN_SELECT;
    }      
    
    else if(!digitalRead(PIN_BTN_LR)) 
    {
      for(byte i = 0; i <BTN_DEBOUNCE_DELAY; i++)
      {
        ShowOutput();
      }
      if(!digitalRead(PIN_BTN_LR)) 
      return PIN_BTN_LR;
    }
    ShowOutput();
    return 0;
}

void GetUserSetting()
{
    Cell* cell = Board;
    cell->rgb = rgb_blink;
    cell->clr = clr_red;
    cell->ply = ply_player1;
    byte input = 0;
  lbl_input: 
    input = GetUserInput();
    if(input == PIN_BTN_SELECT)
    {
      if(cell->ply == ply_player1) 
      {
        Ply1Clr = cell->clr;
        cell++;
        cell->rgb = rgb_blink;
        cell->clr = (Ply1Clr == clr_red) ? clr_green : clr_red;     
        cell->ply = ply_player2;
        delay(100);        
      }
      else 
      {
        Ply2Clr = cell->clr;
        cell->clr = clr_none;
        cell->ply = ply_none;
        cell->rgb = rgb_off;
        cell--;
        cell->clr = clr_red;
        cell->ply = ply_player1;
        cell->rgb = rgb_blink;        
        return;
      }
    }
    if(input == PIN_BTN_LR)
    {
      do
      {
        cell->clr = cell->clr + 1;
        if(cell->clr > 3) cell->clr = clr_red;
      } while(cell->clr == Ply1Clr);
      goto lbl_input;
    }
    goto lbl_input;    
}

void PrintOnLED(byte led_pin, byte bcd_val)
{
    digitalWrite(led_pin, HIGH);
    digitalWrite(PIN_BCD_D, bcd_val & 0x08);  
    digitalWrite(PIN_BCD_C, bcd_val & 0x04);
    digitalWrite(PIN_BCD_B, bcd_val & 0x02);
    digitalWrite(PIN_BCD_A, bcd_val & 0x01);
    delayMicroseconds(LED_ONOFF_TIME);
    digitalWrite(led_pin, LOW);
}

void PrintOnRGB(byte row, byte col)
{
    byte index = (row * 3) + col;
    Cell* cell = Board + index;
    if(cell->rgb == rgb_on)
    {
      byte R = (cell->clr == clr_red) ? 255 : 0;
      byte G = (cell->clr == clr_green) ? 255 : 0;
      byte B = (cell->clr == clr_blue) ? 255 : 0;
      SetColor(R, G, B);
      digitalWrite(PIN_RGB1 + index, HIGH);
      delayMicroseconds(RGB_ONOFF_TIME);
      digitalWrite(PIN_RGB1 + index, LOW);
    }
    else if(cell->rgb == rgb_blink)
    {
      if(cell->blink_count >= 128)
      {          
      byte R = (cell->clr == clr_red) ? 255 : 0;
      byte G = (cell->clr == clr_green) ? 255 : 0;
      byte B = (cell->clr == clr_blue) ? 255 : 0;
      SetColor(R, G, B);
      digitalWrite(PIN_RGB1 + index, HIGH);
      delayMicroseconds(RGB_ONOFF_TIME);
      digitalWrite(PIN_RGB1 + index, LOW);
      }
      cell->blink_count++;      
    }
}

void ShowOutput()
{
  PrintOnLED(PIN_LED_PLY1, Ply1Score);
  delayMicroseconds(LED_REFRESH_TIME);    
  PrintOnLED(PIN_LED_PLY2, Ply2Score);
  if(Board)
  {
    byte row = 0;
    byte col = 0;
    for(byte row = 0; row < 3; row++)
    {
      for(byte col = 0; col < 3; col++)
      {
        PrintOnRGB(row,col);
        delayMicroseconds(RGB_REFRESH_TIME);
      }
    }    
  }
}

void InitBoard()
{
  Ply1Turn = true;
  Ply1Clr = clr_none;
  Ply2Clr = clr_none;

  setup();
  
  if(Board) 
  {
    free(Board);
    Board = NULL;
  }
  Board = (Cell*)malloc(sizeof(Cell) * 9);
  if(Board == NULL)
  {
    while(1)
    {
      digitalWrite(D13, LOW);
      delay(100);
      digitalWrite(D13, HIGH);
      delay(100);
    }
  }
  memset(Board, 0, sizeof(Cell) * 9);
}

bool IsGameOver()
{
  Cell* cell = Board;    
  if((Board)->ply != ply_none && (Board)->ply == (Board+1)->ply && (Board+1)->ply == (Board+2)->ply) 
  {
   if((Board)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }
  if((Board+3)->ply != ply_none && (Board+3)->ply == (Board+4)->ply && (Board+4)->ply == (Board+5)->ply) 
  {
   if((Board+3)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }
  if((Board+6)->ply != ply_none && (Board+6)->ply == (Board+7)->ply && (Board+7)->ply == (Board+8)->ply) 
  {
   if((Board+6)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }

  if((Board)->ply != ply_none && (Board)->ply == (Board+3)->ply && (Board+3)->ply == (Board+6)->ply) 
  {
   if((Board)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }
  if((Board+1)->ply != ply_none && (Board+1)->ply == (Board+4)->ply && (Board+4)->ply == (Board+7)->ply) 
  {
   if((Board+1)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }
  if((Board+2)->ply != ply_none && (Board+2)->ply == (Board+5)->ply && (Board+5)->ply == (Board+8)->ply) 
  {
   if((Board+2)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }

  if((Board)->ply != ply_none && (Board)->ply == (Board+4)->ply && (Board+4)->ply == (Board+8)->ply) 
  {
   if((Board)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }
  if((Board+2)->ply != ply_none && (Board+2)->ply == (Board+4)->ply && (Board+4)->ply == (Board+6)->ply) 
  {
   if((Board)->ply == ply_player1) Ply1Score++;
   else Ply2Score++;
   return true;
  }  
  return false;  
   
}

void StartGameLoop()
{
    Cell* cell = Board;
    cell->rgb = rgb_blink;
    cell->clr = Ply1Clr;
    cell->ply = ply_none;
    Ply1Turn = true;
    byte input = 0;
  lbl_input: 
    input = GetUserInput();
    if(input == PIN_BTN_SELECT)
    {
      if(Ply1Turn)
      {
        cell->rgb = rgb_on;
        cell->clr = Ply1Clr;
        cell->ply = ply_player1;
        Ply1Turn = !Ply1Turn;
      }    
      else        
      {
        cell->rgb = rgb_on;
        cell->clr = Ply2Clr;
        cell->ply = ply_player2;  
        Ply1Turn = !Ply1Turn;
      }
      if(IsGameOver()) return;
      else
      {
        while(cell->ply != ply_none)
        {
          cell++;
          if(cell > Board + 8) cell = Board;
          if(cell->ply == ply_none)
          {
            cell->rgb = rgb_blink;
            cell->clr = Ply1Turn ? Ply1Clr : Ply2Clr;
            cell->ply = ply_none;
            break;
          }
        }        
      }      
    }
    else if(input == PIN_BTN_LR)
    {
      cell->rgb = rgb_off;
      do
      {
        cell++;
        if(cell > Board + 8) cell = Board;
      }
      while(cell->ply != ply_none);
      cell->rgb = rgb_blink;
      cell->clr = Ply1Turn ? Ply1Clr : Ply2Clr;
      cell->ply = ply_none;
    }
    goto lbl_input; 
}


void loop()
{
  InitBoard();
  GetUserSetting();
  StartGameLoop();
}

// void ledtest()
// {
//   PrintOnLED(PIN_LED_PLY1, Ply1Score);
//   delayMicroseconds(1000); 
//   PrintOnLED(PIN_LED_PLY2, Ply2Score);
  
//   if(!digitalRead(PIN_BTN_SELECT)) 
//     {
//       delay(150);
//       if(!digitalRead(PIN_BTN_SELECT)) 
//       {
//       Ply1Score++;
//       //Ply2Score++;
//       if(Ply1Score > 9) 
//       {
//         Ply1Score = 0;
//         Ply2Score++;
//       }
//       if(Ply2Score > 9) Ply2Score = 0;
//       }
//     }   
// }










