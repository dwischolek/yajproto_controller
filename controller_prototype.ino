#include "sketch_settings.h"
#include "pseudo_cobs.h"
#include "yajproto.h"

#define UPDATE_INTERVAL (10)  // milliseconds

#define BAUD_RATE (115200)

static uint8_t cols[] = { 2, 3, 4, 5, 6 };
static const int colCount = sizeof(cols) / sizeof(cols[0]);

static uint8_t rows[] = { 7, 8, 9, 10, 11 };
static const int rowCount = sizeof(rows) / sizeof(rows[0]);

static int8_t debounce_count[colCount][rowCount];

#define DEBOUNCE_PUSHED_THRESHOLD (3)

static const uint8_t analogPins[] = { A0, A1 };

static const int numAnalogChannels = sizeof(analogPins) / sizeof(analogPins[0]);

#ifdef IS_PROFILING
// Debugging stuff
static unsigned short lastIterationDuration = 0;
#endif

static uint8_t messageBuffer[PSEUDO_COBS_MAX_INPUT_SIZE];

static YajProto yajProto(colCount* rowCount, numAnalogChannels);

void setup() {
  Serial.begin(BAUD_RATE);
  // Pullup all digital pins through ports.
  PORTB = 0x0F;  // pins 8..11
  PORTC = 0x00;  // pins A0..A1
  PORTD = 0xFC;  // pins 2..7

  // Portwise pull up may not work on every device. On my ATmega328P it worked,
  // but on a ATmega32u4 it did nothing, leading to hard to debug ghost presses etc.
  // On these, you can pull up all used pins one by one.
  /*
  for (int colIndex = 0; colIndex < colCount; ++colIndex) {
    pinMode(cols[colIndex], INPUT_PULLUP);
  }

  for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
    pinMode(rows[rowIndex], INPUT_PULLUP);
  }
	*/
}


void readMatrix() {
  // iterate over column pins
  for (int colIndex = 0; colIndex < colCount; ++colIndex) {
    // set column pin to OUTPUT LOW
    byte colPin = cols[colIndex];
    digitalWrite(colPin, LOW);
    pinMode(colPin, OUTPUT);

    // iterate over row pins
    for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
      byte rowPin = rows[rowIndex];
      if (digitalRead(rowPin) == LOW) {
        // Switch closed.
        if (debounce_count[colIndex][rowIndex] < DEBOUNCE_PUSHED_THRESHOLD) {
          ++debounce_count[colIndex][rowIndex];
          if (debounce_count[colIndex][rowIndex] == DEBOUNCE_PUSHED_THRESHOLD) {
            yajProto.setDigitalChannelValue((rowIndex * colCount) + colIndex, true);
          }
        }
      } else {
        if (debounce_count[colIndex][rowIndex] > 0) {
          --debounce_count[colIndex][rowIndex];
          if (debounce_count[colIndex][rowIndex] == 0) {
            yajProto.setDigitalChannelValue((rowIndex * colCount) + colIndex, false);
          }
        }
      }
    }
    // return column pin to INPUT
    pinMode(colPin, INPUT_PULLUP);
  }
}

void readAnalogPins() {
  for (int pinIndex = 0; pinIndex < numAnalogChannels; ++pinIndex) {
    yajProto.setAnalogChannelValue(pinIndex, analogRead(analogPins[pinIndex]));
  }
}

void loop() {
  unsigned long time = millis();

  readMatrix();
  readAnalogPins();

  yajProto.writeChannelDataFrameToBuffer(messageBuffer);
  uint16_t encodedByteCount = PseudoCobs::encode(messageBuffer, yajProto.getChannelDataMessageByteCount(), messageBuffer);

  if (encodedByteCount > 0)
    for (size_t i = 0; i < encodedByteCount; i++)
      Serial.write(messageBuffer[i]);

  time = millis() - time;
#ifdef IS_PROFILING
  lastIterationDuration = time > lastIterationDuration ? time : lastIterationDuration;
#endif
  if (time < UPDATE_INTERVAL)
    delay(UPDATE_INTERVAL - time);
}