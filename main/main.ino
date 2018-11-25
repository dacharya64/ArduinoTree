/*
   This is the main file for DANM 219 final project
   It uses an input heartbeat sensor to control audio output and LED light strands.

   NOTE: To use, please make sure PulseSensor Playground is installed.
   Sketch -> Include Library -> Manage Libraries -> search for "PulseSensor Playground" and install version 1.4.11 (currently the latest version)
*/

#define USE_ARDUINO_INTERRUPTS false    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   

/*
   Code to detect pulses from the PulseSensor,
   using an interrupt service routine.

   Here is a link to the tutorial\
   https://pulsesensor.com/pages/getting-advanced

   Copyright World Famous Electronics LLC - see LICENSE
   Contributors:
     Joel Murphy, https://pulsesensor.com
     Yury Gitman, https://pulsesensor.com
     Bradford Needham, @bneedhamia, https://bluepapertech.com

   Licensed under the MIT License, a copy of which
   should have been included with this software.

   This software is not intended for medical use.
*/

/*
   Every Sketch that uses the PulseSensor Playground must
   define USE_ARDUINO_INTERRUPTS before including PulseSensorPlayground.h.
   Here, #define USE_ARDUINO_INTERRUPTS true tells the library to use
   interrupts to automatically read and process PulseSensor data.

   See ProcessEverySample.ino for an example of not using interrupts.
*/


/*
   The format of our output.

   Set this to PROCESSING_VISUALIZER if you're going to run
    the Processing Visualizer Sketch.
    See https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer

   Set this to SERIAL_PLOTTER if you're going to run
    the Arduino IDE's Serial Plotter.
*/
const int OUTPUT_TYPE = SERIAL_PLOTTER;

/*
   Pinout:
     PULSE_INPUT = Analog Input. Connected to the pulse sensor
      purple (signal) wire.
     lightPins = digital Output. PWM pin onnected to an LED (and resistor)
      that will smoothly fade with each pulse.
*/
const int PULSE_INPUT = A0;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle
const int START_BRIGHTNESS = 255;    // how bright the LED starts off
const int END_BRIGHTNESS = 0;    // how bright the LED ends at
const int FADE_AMT = 20;    // how many points to fade the LED by

int lightPins[] = {6, 5, 3, 11};
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

/*
   All the PulseSensor Playground functions.
*/
PulseSensorPlayground pulseSensor;

void setup() {
  /*
     Use 115200 baud because that's what the Processing Sketch expects to read,
     and because that speed provides about 11 bytes per millisecond.

     If we used a slower baud rate, we'd likely write bytes faster than
     they can be transmitted, which would mess up the timing
     of readSensor() calls, which would make the pulse measurement
     not work properly.
  */
  Serial.begin(115200);

  // Configure the PulseSensor manager.
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Set all the pins' pinmode so they can properly output light
  for (int i = 0; i < sizeof lightPins / sizeof lightPins[0]; i++) {
    pinMode(lightPins[i], OUTPUT);
  }

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our particular Arduino platform interrupts
       aren't supported yet.
    */
  }
}

void Blink(int pin, int pin2) {
  for (int i = START_BRIGHTNESS; i >= END_BRIGHTNESS; i = i - FADE_AMT) {
    analogWrite(pin, i);
    analogWrite(pin2, i);
    delay(30);
  }
}

void loop() {
  if (pulseSensor.sawNewSample()) {
    /*
       Every so often, send the latest Sample.
       We don't print every sample, because our baud rate
       won't support that much I/O.
    */
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

      pulseSensor.outputSample();

      /*
         At about the beginning of every heartbeat,
         report the heart rate and inter-beat-interval.
      */
      if (pulseSensor.sawStartOfBeat()) {
        Blink(5, 6);
        Blink(3, 11);
        for (int i = 0; i < sizeof lightPins / sizeof lightPins[0]; i++) {
          analogWrite(lightPins[i], END_BRIGHTNESS);
        }
      }
    }
  }
}
