#include "TimerOne.h"

#define SAMPLING_FREQUENCY          100                                 // Sampling frequency in Hz.
#define SAMPLING_PERIOD             1000000 * 1 / SAMPLING_FREQUENCY    // Sampling period in microseconds.
#define SAMPLING_NUMBER_OF_SAMPLES  10                                  // Number of samples to be averaged.
#define SAMPLING_MARGIN             3                                   // A blink will be captured if a change in the sampling reading larger than this margin is detected.    
#define TIME_BETWEEN_BLINKS         1000                                // Minimum time between blinks captures in miliseconds.

#define SAMPLING_INPUT    A3  // ADC sampling input pin.
#define BUZZER_OUTPUT     9   // Buzzer output pin.

void ISR_Timer1 ();

int sampling_results_index {0};
int sampling_results_total {0};
int sampling_results_average {0};
int sampling_results_threshold {0};
int sampling_results[SAMPLING_NUMBER_OF_SAMPLES] {};

int state_eye_open {true};
unsigned long sampling_last_time_ms {0};

void setup()
{
  // Print program start message.
  Serial.begin(9600);
  //Serial.println("Successful, program start!\n");

  // Initialize digital pin for buzzer.
  pinMode(BUZZER_OUTPUT, OUTPUT);
  digitalWrite(BUZZER_OUTPUT, LOW);
  
  // Initialize timer one.
  Timer1.initialize();
  Timer1.attachInterrupt(ISR_Timer1, SAMPLING_PERIOD);
  delay(1000);
}

void loop()
{
//  if (Serial.available()) {
//    char command = Serial.read();
////    Serial.println(command);
//    if (command == '1') {
//      tone(BUZZER_OUTPUT, 2048, 10);
////      for (int i = 0; i < 2048; i++) {
////        tone(BUZZER_OUTPUT, i, 50);
////      }
//    }
//  }

  if (Serial.available()) {
    int i = 0;
    char command[7];
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
  int sampling_result = analogRead(SAMPLING_INPUT);
//  sampling_results[sampling_results_index] = sampling_result;
//  sampling_results_total += sampling_result;
//  sampling_results_index++;
//  if(sampling_results_index == SAMPLING_NUMBER_OF_SAMPLES) {
//    sampling_results_index = 0;
//  }
//  sampling_results_total -= sampling_results[sampling_results_index];
//  sampling_results_average = sampling_results_total / SAMPLING_NUMBER_OF_SAMPLES;

  Serial.println(sampling_result);
  Serial.println('\n');

//  switch (state_eye_open) {
//    case true:
//      if (sampling_result < sampling_results_threshold) {
//        if (millis() - sampling_last_time_ms > TIME_BETWEEN_BLINKS) {
//          Serial.println("Blink!!!!!");
//          sampling_results_threshold = sampling_results_average + SAMPLING_MARGIN;
//        }
//        state_eye_open = false;
//      }
//      else {
//        sampling_results_threshold = sampling_results_average - SAMPLING_MARGIN;
//      }
//      break;
//    case false:
//      if (sampling_result > sampling_results_threshold) {
//        sampling_results_threshold = sampling_results_average - SAMPLING_MARGIN;
//        sampling_last_time_ms = millis();
//        state_eye_open = true;
//      }
//      else {
//        sampling_results_threshold = sampling_results_average + SAMPLING_MARGIN;
//      }
//      break;
//  }
}
