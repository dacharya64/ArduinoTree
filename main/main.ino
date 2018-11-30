/*
   This is the main file for DANM 219 final project
   It uses an input heartbeat sensor to control audio output and LED light strands.

   NOTE: To use, please make sure PulseSensor Playground is installed.
   Sketch -> Include Library -> Manage Libraries -> search for "PulseSensor Playground" and install version 1.4.11 (currently the latest version)
*/

#define USE_ARDUINO_INTERRUPTS false    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   

/* ----------------------------------------------------------------------
 *  
 *  HARDWARE SETUP
 *  --------------
 *  
 *  Needed: 
 *  2 Arduinos 
 *    -1 with Sound Shield [SOUND ARDUINO]
 *    -Other to control lights [LIGHTS ARDUINO]
 *  Heartbeat sensor
 *  
 *  
 *  Wiring 
 *  ------
 *  Plug in heartbeat sensor on the Sound Arduino, putting red wire into 5V and black wire into ground
 *  
 *  Put purple wire of heartbeat sensor into breadboard, run 2 leads from that input into A0 of both Arduinos
 *  
 *  Link Ground of Lights Arduino to breadboard ground, then to ground of Sound Arduino
 *  
 *  // Audio
 *  Make sure that Sound Shield arduino has SD card with sound files on it, and that output (speaker) is connected to headphone jack
 *  Make sure that volume (round knob on right side of sound shield) is turned up
 *  The headphone jack can be finicky, so you might have to mess with it a bit to get the sound outputting properly
 *  
 *  // Lights
 *  Light output should be running from the Lights Arduino into the breadboard
 *  Select PWM pins you want to use (any should work) and run leads from those pins to the breadboard
 *  Place the positive end of the LEDs you want to use in line with the wire from the Lights Arduino
 *  Place a resistor between the negative end of the LED and the breadboard ground
 *  
 *  
 *  Update Code
 *  -----------
 *  Update the code below to include the pins you are using for the lights: 
 *  
 *  1) update lightPins[] array with all pin values used
 *  2) Update Blink() calls to include the pins you want to blink. 
 *     First Blink() call will blink all those pins at the same time, then a delay, then the next Blink() call pins will flash
 *  
 *  
 *  Uploading Code 
 *  --------------
 *  Load "Audio" Arduino code onto Sound Arduino
 *  Load this file ("Main") onto Lights Arduino
 *  
 *  Make sure both Arduinos are recieving power.
 *  
 * 
 *  User Instructions
 *  -----------------
 *  Place your finger lightly on the heartbeat sensor and wait 5 seconds
 *  You should now hear audio coming from the speaker, and the LEDs should blink in time with your heartbeat
 *  
 *  
 *  ----------------------------------------------------------------------
 */



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
const int PULSE_INPUT = A0;  // The input for the PulseSensor data 
const int THRESHOLD = 550;   // Threshold of heartbeat sensor. Adjust this number to avoid noise when idle
const int START_BRIGHTNESS = 255;    // how bright the LED starts off
const int END_BRIGHTNESS = 0;    // how bright the LED ends at
const int FADE_AMT = 15;    // how many points to fade the LED by
const int DELAY = 20;

long previousMillis = 0;

int lightPins[] = {6, 5, 10, 11};
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

unsigned long time_now = 0;

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
    //delay(20);
    time_now = millis();
    while(millis() < time_now + DELAY){
        //wait approx. [period] ms
    }
  }
}

void loop() { 
//  
//  if (pulseSensor.sawNewSample()) {
//    /*
//       Every so often, send the latest Sample.
//       We don't print every sample, because our baud rate
//       won't support that much I/O.
//
//       Use this to operate in conjunction with Sound Shield
//    */
//    if (--samplesUntilReport == (byte) 0) {
//      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
//
//      pulseSensor.outputSample();
//
//      /*
//         At about the beginning of every heartbeat,
//         report the heart rate and inter-beat-interval.
//      */
//      if (pulseSensor.sawStartOfBeat()) {
//        Blink(5, 6);
//        Blink(10, 11);
//        //for (int i = 0; i < sizeof lightPins / sizeof lightPins[0]; i++) {
//        //  analogWrite(lightPins[i], END_BRIGHTNESS);
//        //}
//      }
//    }
//  }

   /* THIS OPERATES IN PLACE OF THE HEARTBEAT SENSOR CODE
   * Use only if trying to operate independent of heartbeat sensor data
   */

   Blink(5, 6);
   Blink(10, 11);
   delay(650);
}

//
//void Blink(int pin, int pin2) {
//  brightness = START_BRIGHTNESS;
//  // previousMillis = currentMillis;
//  currentMillis = millis();
//  if(currentMillis - previousMillis > 20) {
//    analogWrite(pin, brightness);
//    analogWrite(pin2, brightness);
//    brightness = brightness - FADE_AMT;
//    previousMillis = currentMillis;   
//  }
//}
