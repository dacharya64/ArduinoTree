/*
 * This is the main file for DANM 219 final project 
 * It uses an input heartbeat sensor to control audio output and LED light strands. 
 * 
 * NOTE: To use, please make sure PulseSensor Playground is installed. 
 * Sketch -> Include Library -> Manage Libraries -> search for "PulseSensor Playground" and install version 1.4.11 (currently the latest version)
 */
 
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
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
     PULSE_BLINK = digital Output. Connected to an LED (and 220 ohm resistor)
      that will flash on each detected pulse.
     PULSE_FADE = digital Output. PWM pin onnected to an LED (and resistor)
      that will smoothly fade with each pulse.
      NOTE: PULSE_FADE must be a pin that supports PWM. Do not use
      pin 9 or 10, because those pins' PWM interferes with the sample timer.
*/
const int PULSE_INPUT = A0;
const int PULSE_BLINK = 5;    // Pin 13 is the on-board LED
const int PULSE_FADE = 6;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle

int brightness = 255;    // how bright the LED is
int fadeAmount = 50;    // how many points to fade the LED by



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
  //pulseSensor.fadeOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  pinMode(PULSE_BLINK, OUTPUT);
 

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our particular Arduino platform interrupts
       aren't supported yet.

       If your Sketch hangs here, try PulseSensor_BPM_Alternative.ino,
       which doesn't use interrupts.
    */
   
  }
}

void loop() {
  /*
     Wait a bit.
     We don't output every sample, because our baud rate
     won't support that much I/O.
  */

  delay(20);

  // write the latest sample to Serial.
 //pulseSensor.outputSample();

  /*
     If a beat has happened since we last checked,
     write the per-beat information to Serial.
   */
  if (pulseSensor.sawStartOfBeat()) {
        Serial.println("beat");
        brightness = 255;
        analogWrite(PULSE_BLINK, brightness);
        brightness = 240;
        analogWrite(PULSE_BLINK, brightness);
        delay(30);
        brightness = 220;
        analogWrite(PULSE_BLINK, brightness);
        delay(30);
        brightness = 200;
        analogWrite(PULSE_BLINK, brightness);
        brightness = 150;
        analogWrite(PULSE_BLINK, brightness);
        delay(30);
        brightness = 100;
        analogWrite(PULSE_BLINK, brightness);
        delay(30);
        brightness = 50;
        analogWrite(PULSE_BLINK, brightness);
        delay(30);
        brightness = 0;
        analogWrite(PULSE_BLINK, brightness);
       
      
        // reverse the direction of the fading at the ends of the fade:
        
        // wait for 30 milliseconds to see the dimming effect
        //delay(30);
        
   pulseSensor.outputBeat();

  }
}
