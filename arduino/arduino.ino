#include "TimerOne.h"

#define SAMPLING_FREQUENCY  100                                     // Sampling frequency in Hz.
#define SAMPLING_PERIOD     1000000 * 1 / SAMPLING_FREQUENCY        // Sampling period in microseconds.
//#define SAMPLING_TIME       1                                           // Sampling time in seconds.
//#define SAMPLING_NUMBERS    SAMPLING_TIME * SAMPLING_FREQUENCY  // Sampling number of samples.

#define SAMPLING_INPUT    A3  // Microphone input pin.
#define LED_OUTPUT        8   // LED output pin.

void ISR_Timer1 ();

//bool sampling_done = false;
int  sampling_index = 0;
//int  sample = 0;

void setup()
{
  // Print program start message.
  Serial.begin(9600);
  Serial.println("Successful, program start!\n");

  // LED initialization.
  pinMode(LED_OUTPUT, OUTPUT);
  digitalWrite(LED_OUTPUT, LOW);

  // Timer 1 initialization.
  Timer1.initialize();
  delay(100);
  Timer1.attachInterrupt(ISR_Timer1, SAMPLING_PERIOD);

//  int *sampling_results[SAMPLING_NUMBERS];
//  while (!sampling_done) {
//    //sampling_results[sampling_index] = sample;
//  }
//  for (int i=0; i < SAMPLING_NUMBERS; i++) {
//      int val = sampling_results[i];
//      Serial.println(val);
//  }
//
//  // Print program finish message.
//  Serial.println("Finished, program terminates!\n");
}

void loop()
{

}

void ISR_Timer1 ()
{
//  if (sampling_index < SAMPLING_NUMBERS) {
//    sample = analogRead(SAMPLING_INPUT);
//    sampling_index++;
//  }
//  else {
//    Timer1.detachInterrupt();
//    sampling_done = true;
//    digitalWrite(LED_OUTPUT, sampling_done);
//  }
  Serial.println(analogRead(SAMPLING_INPUT));
  digitalWrite(LED_OUTPUT, sampling_index % 2);
  sampling_index++;
}
