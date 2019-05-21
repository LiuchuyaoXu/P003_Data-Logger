#include "TimerOne.h"

#define SAMPLING_FREQUENCY          100                                 // Sampling frequency in Hz.
#define SAMPLING_PERIOD             1000000 * 1 / SAMPLING_FREQUENCY    // Sampling period in microseconds.
#define SAMPLING_NUMBER_OF_SAMPLES  10                                   // Number of samples to be averaged.
#define SAMPLING_DIFFERENCE_MARGIN  5                                  // A blink is detected if the difference     

#define SAMPLING_INPUT    A3  // Microphone input pin.
#define BUZZER_OUTPUT     8   // Buzzer output pin.

void ISR_Timer1 ();

int state_eye_open = 1;
int sampling_threshold = 0;
int sampling_results_index = 0;
int sampling_results_average = 0;
int sampling_results[SAMPLING_NUMBER_OF_SAMPLES];

unsigned long time_millisecond = 0;
unsigned long time_millisecond_last = 0;

void setup()
{
  // Print program start message.
  Serial.begin(9600);
  //Serial.println("Successful, program start!\n");

  // LED initialization.
  pinMode(BUZZER_OUTPUT, OUTPUT);
  digitalWrite(BUZZER_OUTPUT, LOW);

  // Initialize sampling results to zero.
  int i;
  for (i = 0; i < SAMPLING_NUMBER_OF_SAMPLES; i++) {
    sampling_results[i] = 0; 
  }
  
  // Timer 1 initialization.
  Timer1.initialize();
  Timer1.attachInterrupt(ISR_Timer1, SAMPLING_PERIOD);
  delay(1000);
}

void loop()
{
  int total = 0;
  int i = 0;
  for (i = 0; i < SAMPLING_NUMBER_OF_SAMPLES; i++) {
    total += sampling_results[i];
  }
  sampling_results_average = total / SAMPLING_NUMBER_OF_SAMPLES;
}

void ISR_Timer1 ()
{
  //Serial.println(analogRead(SAMPLING_INPUT));
  
  sampling_results[sampling_results_index++] = analogRead(SAMPLING_INPUT);
  if (sampling_results_index == SAMPLING_NUMBER_OF_SAMPLES) sampling_results_index = 0;
  
  if (state_eye_open) {
    sampling_threshold = sampling_results_average - SAMPLING_DIFFERENCE_MARGIN;
    if (sampling_results[sampling_results_index] < sampling_threshold) {
      //time_millisecond = millis() - time_millisecond;
      Serial.println(time_millisecond);
      if (millis() - time_millisecond > 1000) {
        Serial.println(1);
        state_eye_open = false;
      }
    }
  }
  else {
    sampling_threshold = sampling_results_average + SAMPLING_DIFFERENCE_MARGIN;
    if (sampling_results[sampling_results_index] > sampling_threshold) {
      time_millisecond = millis();
      state_eye_open = true;
    }
  }

  //Serial.println(sampling_results[sampling_results_index]);
}
