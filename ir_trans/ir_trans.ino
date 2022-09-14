
#include <IRremote.h> // Required to send & receive IR Data 
                                            
#define GUN_TRIGGER 2
#define IR_EMIT 4
#define BUZZER 5
#define THRESHOLD 250 // the debounce time; increase if the output flickers


// GUN TRIGGER STATES
byte trigger_state = LOW;
byte previous_trigger = LOW;

// DEBOUNCING
unsigned long last_pressed_time = 0;  // the last time the output pin was toggled 

void setup()  
{ 
  Serial.begin(9600); //initialize serial connection to print on the Serial Monitor of the Arduino IDE
  pinMode(GUN_TRIGGER,INPUT);
  pinMode(BUZZER,OUTPUT); 
  IrSender.begin(IR_EMIT); // Initializes IR sender
}  
                               
void loop()  
{  
  trigger_state = digitalRead(GUN_TRIGGER);

  if (trigger_state != previous_trigger && trigger_state == HIGH) {
    // DEBOUNCE
    if(millis() - last_pressed_time > THRESHOLD) {
      // SEND IR
      IrSender.sendNEC(0x0102, 0x34, true, 0); // the address 0x0102 with the command 0x34 is sent 
      Serial.print("IR Sent \n"); 
      last_pressed_time = millis();
      playBuzzer();
    }
  }
  previous_trigger = trigger_state;
}

void playBuzzer() {
 digitalWrite(BUZZER, HIGH);
 delay(50);
 digitalWrite(BUZZER, LOW);
 delay(10);
  digitalWrite(BUZZER, HIGH);
 delay(30);
 digitalWrite(BUZZER, LOW);
 delay(50);
}
