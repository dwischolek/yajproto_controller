#pragma once

#define PSEUDO_COBS_MAX_INPUT_SIZE_EXCEEDED (-1)
#define PSEUDO_COBS_MAX_FRAME_BYTES 256
#define PSEUDO_COBS_OVERHEAD_BYTES 2
#define PSEUDO_COBS_MARKER_SIZE 1
#define PSEUDO_COBS_MAX_INPUT_SIZE (PSEUDO_COBS_MAX_FRAME_BYTES - PSEUDO_COBS_OVERHEAD_BYTES)

class PseudoCobs {
public:

  static int16_t encode(const uint8_t* buffer,
                        uint8_t size,
                        uint8_t* encodedBuffer) {
    if (size > PSEUDO_COBS_MAX_INPUT_SIZE)
      return PSEUDO_COBS_MAX_INPUT_SIZE_EXCEEDED;

    encodedBuffer[size + PSEUDO_COBS_MARKER_SIZE] = 0;

    uint8_t lastZeroByte = size;
    for (int16_t i = size - 1; i >= 0; i--) {
      if (buffer[i] == 0) {
        encodedBuffer[i + PSEUDO_COBS_MARKER_SIZE] = lastZeroByte - i;
        lastZeroByte = i;
      } else {
        encodedBuffer[i + PSEUDO_COBS_MARKER_SIZE] = buffer[i];
      }
    }
    encodedBuffer[0] = lastZeroByte + 1;

    return size + PSEUDO_COBS_OVERHEAD_BYTES;
  }
};