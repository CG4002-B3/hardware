/*
MIT License
Copyright 2021 Michael Schoeffler (https://www.mschoeffler.com)
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
 * This is an example source code of a tutorial on how to do IR communication with the Arduino Uno. 
 * This program is for a sending Arduino Uno that sends an IR signal each second.
 */

#include <IRremote.h> // >v3.0.0

#define BUZZER 4
#define ORANGE_LED 3                                            
#define IR_REC_FRONT 2

#define THRESHOLD 250
#define IR_DATA 4194631938// Raw data based on received signal (IrReceiver.decodedIRData.decodedRawData) Address 0x0102 command 0x05

//bool is_shot = false;
unsigned long current_time = 0;
uint32_t raw_data = 0;

void setup()  
{ 
  Serial.begin(9600); //initialize serial connection to print on the Serial Monitor of the Arduino IDE
  pinMode(BUZZER, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  IrReceiver.begin(IR_REC_FRONT); // Initializes the IR receiver object
}  
                               
void loop()  
{  
  // provide time for decoding of IR value
  if(millis() - current_time > THRESHOLD) {
    current_time = millis();
    if (IrReceiver.decode()) {
      Serial.println("Received something...");    
      IrReceiver.printIRResultShort(&Serial); // Prints a summary of the received data
      Serial.println();
      raw_data = IrReceiver.decodedIRData.decodedRawData;
      Serial.println(raw_data);
      
      // Register shots
      if(raw_data == IR_DATA) {
        playFeedback();
      }
      IrReceiver.resume(); // Important, enables to receive the next IR signal
    }  
  }
}  

void playFeedback() {
  digitalWrite(BUZZER, HIGH);
  digitalWrite(ORANGE_LED, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  digitalWrite(ORANGE_LED, LOW);
  delay(100);
}
