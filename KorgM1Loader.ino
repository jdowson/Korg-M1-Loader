/*
  Korg M1 SysEx File Loader
  
  The circuit:
  - digital in 1 connected to MIDI jack pin 5
  - MIDI jack pin 2 connected to ground
  - MIDI jack pin 4 connected to +5V through 220 ohm resistor
  - Attach a MIDI cable to the jack, then to a MIDI synth, and play music.
*/

#include <MIDI.h>

#include "SysExData.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

static const uint8_t C_MAJOR[] = { 60, 62, 64, 65, 67, 69, 71, 72 };

#define LED_PIN                  13
#define DUMP_DATA                1
#define DUMP_DATA_BYTES_PER_LINE 32
#define BLOCK_SIZE               64
#define BLOCK_DELAY              10
#define DELAY_AFTER_F0           400

void setup() {
  
  Serial.begin(9600); 
  MIDI.begin();

  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  while (!Serial) {}

  for (int8_t i = 10; i >= 0; i--) {
    Serial.print("[SYSEX] About to install ");
    Serial.println(i);
    delay(1000);
  }

  writeSysEx();
}

void loop() {
// Test MIDI Interface - Send C Major to Ch.1
/*    if (Serial.read() >= 0)                    // If we have received a message
    {
      for (uint8_t i = 0; i < 8; i ++) {
        digitalWrite(ledPin, HIGH);
        MIDI.sendNoteOn(C_MAJOR[i], 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)
        delay(1000);                // Wait for a second
        MIDI.sendNoteOff(C_MAJOR[i], 0, 1);     // Stop the note
        digitalWrite(ledPin, LOW);        
      }
    }
*/
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}

void writeSysExChunk(uint8_t *pChunk, uint16_t iLength, uint16_t &count) {

  uint8_t d[1];
  char    hex[7];

  for (uint16_t i = 0; i < iLength; i++) {
    d[0] = pgm_read_byte(pChunk + i); 
    
    MIDI.sendSysEx(1, d, true);
    
    if (DELAY_AFTER_F0 && d[0] == 0xF0) delay(DELAY_AFTER_F0);
    count++;
    if (!(count % BLOCK_SIZE)) delay(BLOCK_DELAY);

#ifdef DUMP_DATA
    sprintf(hex, "%02X ", d[0]);
    Serial.print(hex);
    if (!(count % DUMP_DATA_BYTES_PER_LINE)) {
      Serial.println();
      sprintf(hex, "%04X: ", count);
      Serial.print(hex);
    }
#endif  
  }  
}

void writeSysEx() {
  
  Serial.println("[SYSEX] Writing SysEx File");
  Serial.println();
  Serial.print("0000: ");
  
  digitalWrite(LED_PIN, HIGH);
  
  delay(1000);
  uint16_t l = 0;

  writeSysExChunk(sysex_file,  sizeof(sysex_file),  l);
  writeSysExChunk(sysex_file2, sizeof(sysex_file2), l);

  digitalWrite(LED_PIN, LOW);
  Serial.println();
  Serial.println();
  Serial.print("[SYSEX] SysEx File Sent - ");
  Serial.print(l);
  Serial.println(" bytes");
}
