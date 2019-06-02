#include "TimerOne.h"

#define SAMPLING_FREQUENCY    1000                                // Sampling frequency in Hz.
#define SAMPLING_PERIOD       1000000 * 1 / SAMPLING_FREQUENCY    // Sampling period in microseconds.

#define SAMPLING_INPUT        A3  // ADC sampling input pin.
#define BUZZER_OUTPUT         9   // Buzzer output pin.

void ISR_Timer1 ();

void setup()
{
  // Open serial port.
  Serial.begin(9600);

  // Initialize digital pin for buzzer.
  pinMode(BUZZER_OUTPUT, OUTPUT);
  digitalWrite(BUZZER_OUTPUT, LOW);
  
  // Initialize timer one.
  Timer1.initialize();
  Timer1.attachInterrupt(ISR_Timer1, SAMPLING_PERIOD);
}

void loop()
{
  if (Serial.available()) {
    int i {};
    char command[7] {};
    command[0] = Serial.read();
    if (command[0] == '1') {
      i++;
      while (i < 7) {
        if (Serial.available()) {
          command[i] = Serial.read();
          i++;
        }
      }
      int frequency = (command[2] - '0') * 1000 + (command[3] - '0') * 100 + (command[4] - '0') * 10 + (command[5] - '0');
      tone(BUZZER_OUTPUT, frequency, 10);
    }
  }
}

void ISR_Timer1 ()
{
  // Transmit one sampling result over serial port.
  Serial.print(analogRead(SAMPLING_INPUT));
  Serial.print('\n');
}
