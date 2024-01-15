#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2; //xy=563.3333129882812,477.3333435058594
AudioSynthWaveform       waveform3; //xy=563.3333129882812,518.3333435058594
AudioSynthWaveform       waveform4; //xy=563.3333129882812,560.3333435058594
AudioSynthWaveform       waveform1;      //xy=564.3333129882812,437.3333435058594
AudioInputUSB            from_host;           //xy=645.3333129882812,259.3333435058594
AudioInputI2SQuad        analog_in;      //xy=649.3333129882812,119.3332748413086
AudioAmplifier           amp1;           //xy=997.3333129882812,227.33334350585938
AudioAmplifier           amp2;           //xy=999.3333129882812,274.33331298828125
AudioMixer4              input_mix_1; //xy=1010.3333129882812,623.3333435058594
AudioMixer4              input_mix_0;         //xy=1011.3333129882812,418.3333435058594
AudioOutputUSB           to_host;           //xy=1445.3333740234375,495.3333435058594
AudioOutputI2SQuad       analog_out;      //xy=1553.3333740234375,192.33331298828125
AudioConnection          patchCord1(waveform2, 0, input_mix_0, 1);
AudioConnection          patchCord2(waveform2, 0, input_mix_1, 1);
AudioConnection          patchCord3(waveform3, 0, input_mix_0, 2);
AudioConnection          patchCord4(waveform3, 0, input_mix_1, 2);
AudioConnection          patchCord5(waveform4, 0, input_mix_0, 3);
AudioConnection          patchCord6(waveform4, 0, input_mix_1, 3);
AudioConnection          patchCord7(waveform1, 0, input_mix_0, 0);
AudioConnection          patchCord8(waveform1, 0, input_mix_1, 0);
AudioConnection          patchCord9(from_host, 0, amp1, 0);
AudioConnection          patchCord10(from_host, 1, amp2, 0);
AudioConnection          patchCord11(analog_in, 0, analog_out, 0);
AudioConnection          patchCord12(analog_in, 1, analog_out, 1);
AudioConnection          patchCord13(amp1, 0, analog_out, 2);
AudioConnection          patchCord14(amp2, 0, analog_out, 3);
AudioConnection          patchCord15(input_mix_1, 0, to_host, 1);
AudioConnection          patchCord16(input_mix_0, 0, to_host, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=846.3333282470703,869.3333282470703
AudioControlSGTL5000     sgtl5000_2;     //xy=846.3333129882812,915.3333129882812
// GUItool: end automatically generated code


#include <Metro.h>
#include <array>

constexpr float hw_output_volume = 0.8f;
constexpr auto midi_state_send_interval = 1000;

// MIDI channel numbers are 1-indexed here
constexpr auto outbound_control_channel = 16; 
constexpr auto inbound_control_channel = 16; 

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   14  // Teensy 4 ignores this, uses pin 13


// My gains.
std::array<byte, 12> channelGainState = {0, 0, 0, 0,    0, 0, 0, 0,    6, 6, 6, 6};

byte whinePassFilterEnabled[2] = {127, 127};

/**
 * Set the gain on both headphone channels equally.
*/
void set_hp_gain(float g) {
  amp1.gain(g);
  amp2.gain(g);
}

void sync_ccs() {
  for (size_t i = 0; i < channelGainState.size(); i++) {
    usbMIDI.sendControlChange(16 + i, channelGainState[i], outbound_control_channel);
  }

  for (int i = 0; i < 2; i++) {
    usbMIDI.sendControlChange(2 + i, whinePassFilterEnabled[i], outbound_control_channel);
  }
}

// void apply_channel_gains() {
//   for (int i = 0; i < 8; i++) {
//     if (i < 4) {
//       input_mix_0.gain()
//     }
//   }
// }

void update_mixer_control(byte control, byte value) {
  if (control == 1) { // sync button
    if (value > 50) {
      sync_ccs();
    }

    return;
  }

  if (control == 2 || control == 3) { // enable/disable whiney filter thingie
    if (control == 2) {
      if (value > 50) {
        sgtl5000_1.adcHighPassFilterEnable();
      }
      else {
        sgtl5000_1.adcHighPassFilterDisable();
      }
    }

    if (control == 3) {
      if (value > 50) {
        sgtl5000_2.adcHighPassFilterEnable();
      }
      else {
        sgtl5000_2.adcHighPassFilterDisable();
      }
    }

    whinePassFilterEnabled[control - 2] = value;
    return;
  }

  if (control >= 16 && control <= 23) { // mixer gains
    const float fgain = value / 127.f;

    switch (control) {
      case 16:
      case 17:
      case 18:
      case 19:
        input_mix_0.gain(control - 16, fgain);
        channelGainState[control - 16] = value;
        break;
      case 20:
      case 21:
      case 22:
      case 23:
        input_mix_1.gain(control - 20, fgain);
        channelGainState[control - 16] = value; // not a typo, it's in the same array.
        break;
    }

    return;
  }

  if (control >= 24 && control <= 27) { // input gain levels
    const byte level = (value > 15) ? 15 : value;
    channelGainState[control - 16] = level;

    if (control == 24 || control == 25) {
      sgtl5000_1.lineInLevel(channelGainState[8], channelGainState[9]);
    }
    else if (control == 26 || control == 27) {
      sgtl5000_2.lineInLevel(channelGainState[10], channelGainState[11]);
    }
  }
}

void myControlChange(byte channel, byte control, byte value) {
  if (channel == inbound_control_channel) update_mixer_control(control, value);
}

void setup() {
  //Serial.begin(9600);
  AudioMemory(64);
  
  // Set up sound chips.
  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  //sgtl5000_1.lineInLevel(8);
  sgtl5000_1.volume(hw_output_volume);

  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
  //sgtl5000_2.lineInLevel(8);
  sgtl5000_2.volume(hw_output_volume);

  // configure SD card to use the audio-board slot, which is somehow better according to docs.
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  waveform1.begin(0.5f, 440.f, WAVEFORM_SINE);
  waveform2.begin(0.5f, 440.f, WAVEFORM_SAWTOOTH);
  waveform3.begin(0.5f, 440.f, WAVEFORM_SQUARE);
  waveform4.begin(0.5f, 440.f, WAVEFORM_TRIANGLE);

  // set up the default mix
  input_mix_0.gain(0, 0.0f); // cc 16
  input_mix_0.gain(1, 0.0f); // cc 17
  input_mix_0.gain(2, 0.0f); // cc 18
  input_mix_0.gain(3, 0.0f); // cc 19


  input_mix_1.gain(0, 0.0f); // cc 20
  input_mix_1.gain(1, 0.0f); // cc 21
  input_mix_1.gain(2, 0.0f); // cc 22
  input_mix_1.gain(3, 0.0f); // cc 23

  set_hp_gain(0.f);

  usbMIDI.setHandleControlChange(myControlChange);
}

void loop() {
  set_hp_gain(from_host.volume()); // update from host-side USB volume setting (like the volume slider in the control bar)

  usbMIDI.read(); // call MIDI input callbacks
}
