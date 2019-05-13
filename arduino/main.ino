#include "TimerOne.h"

#define MIC_SAMPLING_FREQUENCY  300                                         // Microphone sampling frequency in Hz.
#define MIC_SAMPLING_PERIOD     1000000 * 1 / MIC_SAMPLING_FREQUENCY        // Microphone sampling period in microseconds.
#define MIC_SAMPLING_TIME       1                                           // Microphone sampling time in seconds.
#define MIC_SAMPLING_NUMBERS    MIC_SAMPLING_TIME * MIC_SAMPLING_FREQUENCY  // Microphone sampling number of samples.

#define MIC_INPUT   A3  // Microphone input pin.
#define LED_OUTPUT  8   // LED output pin.

void ISR_Timer1 ();

bool mic_sampling_done = false;
int  mic_sampling_index = 0;
int  mic_sample = 0;

void setup()
{
  // LED initialization.
  pinMode(LED_OUTPUT, OUTPUT);
  digitalWrite(LED_OUTPUT, LOW);

  // Print program start message.
  Serial.begin(9600);
  Serial.println("Successful, program start!\n");
  delay(1000);

  // Timer 1 initialization.
  Timer1.initialize();
  Timer1.attachInterrupt(ISR_Timer1, MIC_SAMPLING_PERIOD);

  int *mic_sampling_results[MIC_SAMPLING_NUMBERS];
  while (!mic_sampling_done) {
    //mic_sampling_results[mic_sampling_index] = mic_sample;
  }
//  for (int i=0; i < MIC_SAMPLING_NUMBERS; i++) {
//      int val = mic_sampling_results[i];
//      Serial.println(val);
//  }

  // Print program finish message.
  Serial.println("Finished, program terminates!\n");
}

void loop()
{

}

void ISR_Timer1 ()
{
  if (mic_sampling_index < MIC_SAMPLING_NUMBERS) {
    mic_sample = analogRead(MIC_INPUT);
    mic_sampling_index++;
  }
  else {
    Timer1.detachInterrupt();
    mic_sampling_done = true;
    digitalWrite(LED_OUTPUT, mic_sampling_done);
  }
  //digitalWrite(LED_OUTPUT, mic_sampling_index % 2);
}
