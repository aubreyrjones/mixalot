// Quad channel output test
// Play two WAV files on two audio shields.
//
// TODO: add info about required hardware connections here....
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=1140,729
AudioSynthWaveform       waveform1;      //xy=1143,679
AudioSynthWaveformSine   sine1;          //xy=1144,622
AudioSynthWaveform       waveform3;      //xy=1151,792
AudioOutputUSB           usb1;           //xy=1628,905
AudioOutputI2SQuad       i2s_quad1;      //xy=1650,663
AudioConnection          patchCord1(waveform2, 0, i2s_quad1, 2);
AudioConnection          patchCord2(waveform1, 0, i2s_quad1, 1);
AudioConnection          patchCord3(waveform1, 0, usb1, 1);
AudioConnection          patchCord4(sine1, 0, i2s_quad1, 0);
AudioConnection          patchCord5(sine1, 0, usb1, 0);
AudioConnection          patchCord6(waveform3, 0, i2s_quad1, 3);
AudioControlSGTL5000     sgtl5000_2;     //xy=1729,1531
AudioControlSGTL5000     sgtl5000_1;     //xy=1730,1466
// GUItool: end automatically generated code


// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   14  // Teensy 4 ignores this, uses pin 13

// Use these with the Teensy 3.5 & 3.6 & 4.1 SD card
//#define SDCARD_CS_PIN    BUILTIN_SDCARD
//#define SDCARD_MOSI_PIN  11  // not actually used
//#define SDCARD_SCK_PIN   13  // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13

void setup() {
  Serial.begin(9600);
  AudioMemory(10);
  
  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.volume(0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  sine1.frequency(440.f);
  sine1.amplitude(1.0f);

  waveform1.begin(0.5f, 440.f, WAVEFORM_SAWTOOTH);
  waveform2.begin(0.5f, 440.f, WAVEFORM_SQUARE);
  waveform3.begin(0.5f, 440.f, WAVEFORM_TRIANGLE);
}

void loop() {
  //sine_hires1.frequency(float freq)
}
