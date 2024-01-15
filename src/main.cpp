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
AudioSynthWaveform       waveform3; //xy=382.33331298828125,372.3333740234375
AudioSynthWaveform       waveform1;      //xy=383.33331298828125,293.3333435058594
AudioSynthWaveform       waveform2; //xy=383.33331298828125,333.3333740234375
AudioSynthWaveform       waveform4; //xy=384.33331298828125,414.3333740234375
AudioInputI2SQuad        analog_in;      //xy=388.33331298828125,139.33328247070312
AudioOutputI2SQuad       analog_out;      //xy=1235.3333740234375,338.33331298828125
AudioConnection          patchCord1(waveform3, 0, analog_out, 2);
AudioConnection          patchCord2(waveform1, 0, analog_out, 0);
AudioConnection          patchCord3(waveform2, 0, analog_out, 1);
AudioConnection          patchCord4(waveform4, 0, analog_out, 3);
AudioControlSGTL5000     sgtl5000_1;     //xy=110.33332824707031,58.333335876464844
AudioControlSGTL5000     sgtl5000_2;     //xy=110.33331298828125,104.33332061767578
// GUItool: end automatically generated code


#include <Metro.h>
#include <array>

constexpr float hw_output_volume = 0.5f;
constexpr auto midi_state_send_interval = 1000;

// MIDI channel numbers are 1-indexed here
constexpr auto outbound_control_channel = 16; 
constexpr auto inbound_control_channel = 16; 

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   14  // Teensy 4 ignores this, uses pin 13

// All MIDI controls.
std::array<byte, 100> controlState;

void myControlChange(byte channel, byte control, byte value) {
  if (channel == inbound_control_channel) return;
}

void setup() {
  Serial.begin(9600);
  AudioMemory(32);
  
  // Set up sound chips.
  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(hw_output_volume);

  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_2.volume(hw_output_volume);

  // configure SD card to use the audio-board slot, which is somehow better according to docs.
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  waveform1.begin(1, 440, WAVEFORM_SINE);
  waveform2.begin(1, 440, WAVEFORM_SAWTOOTH);
  waveform3.begin(1, 440, WAVEFORM_SQUARE);
  waveform4.begin(1, 440, WAVEFORM_TRIANGLE);

  usbMIDI.setHandleControlChange(myControlChange);
}

void loop() {
  usbMIDI.read(); // call MIDI input callbacks
}
