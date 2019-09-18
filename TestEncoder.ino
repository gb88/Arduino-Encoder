/**************************************************************************
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Created by Giovanni Bonomini
 **************************************************************************/
//encoder pin mapping
#define CLK       2
#define DT        3

#define ENCODER_DX  1
#define ENCODER_SX  2
#define ENCODER_NONE 0

volatile uint8_t encoderValue = 0;

//ISR gestione rotazione encoder
void encoder_isr ()
{
  static uint8_t l_consecutive = 0;
  static uint8_t r_consecutive = 0;
  static uint8_t lastEncoded = 0;
  uint8_t MSB = digitalRead(CLK); //MSB = most significant bit
  uint8_t LSB = digitalRead(DT); //LSB = least significant bit

  uint8_t encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  uint8_t sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
  {
    l_consecutive++;
    r_consecutive = 0;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
  {
    r_consecutive++;
    l_consecutive = 0;
  }

  if (r_consecutive >= 4)
  {
    encoderValue = ENCODER_SX;
    r_consecutive = 0;
  }

  if (l_consecutive >= 4)
  {
    encoderValue = ENCODER_DX;
    l_consecutive = 0;
  }
  lastEncoded = encoded; //store this value for next time
} // ISR

void setup()
{
  Serial.begin(9600);
  //setup encoder pin
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  attachInterrupt(0, encoder_isr, CHANGE);   // interrupt 0 is always connected to pin 2
  attachInterrupt(1, encoder_isr, CHANGE);   // interrupt 1 is always connected to pin 3
}


void loop()
{
  if ( encoderValue == ENCODER_DX)
  {
    Serial.println(F("DX"));
    encoderValue = ENCODER_NONE;
  }
  else if ( encoderValue == ENCODER_SX)
  {
    Serial.println(F("SX"));
    encoderValue = ENCODER_NONE;
  }
} // loop
