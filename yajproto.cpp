#include "Arduino.h"
#include "yajproto.h"

YajProto::YajProto(uint8_t numDigitalChannels, uint8_t numAnalogChannels) {
  numDigitalChannels_ = numDigitalChannels;
  numDigitalChannelDataBytes_ = (7 + numDigitalChannels_) / 8;
  numAnalogChannels_ = numAnalogChannels;
  channelDataMessageByteCount_ = 7 + numDigitalChannelDataBytes_ + (2 * numAnalogChannels_);
  headerCheckSum =
    0xFFFF - channelDataMessageByteCount_ - YAJ_PROTO_VERSION - CHANNEL_DATA_FRAME_COMMAND_ID - numDigitalChannels_ - numAnalogChannels_;

  digitalChannels_ = (uint8_t*)malloc(numDigitalChannelDataBytes_ * sizeof(uint8_t));
  for (size_t i = 0; i < numDigitalChannelDataBytes_; i++)
    digitalChannels_[i] = 0;

  analogChannels_ = (uint16_t*)malloc(numAnalogChannels_ * sizeof(uint16_t));
  for (size_t i = 0; i < numAnalogChannels_; i++)
    analogChannels_[i] = 0;
}

uint8_t YajProto::getNumDigitalChannels() {
  return numDigitalChannels_;
}

uint8_t YajProto::getNumAnalogChannels() {
  return numAnalogChannels_;
}

uint8_t YajProto::getChannelDataMessageByteCount() {
  return channelDataMessageByteCount_;
}

bool YajProto::setDigitalChannelValue(uint8_t channelIdx, bool value) {
  if (channelIdx >= numDigitalChannels_)
    return false;

  if (value)
    bitSet(digitalChannels_[channelIdx / 8], channelIdx % 8);
  else
    bitClear(digitalChannels_[channelIdx / 8], channelIdx % 8);

  return true;
}

bool YajProto::getDigitalChannelValue(uint8_t channelIdx) {
  if (channelIdx >= numDigitalChannels_)
    return false;

  return (digitalChannels_[channelIdx / 8] & (1 << channelIdx % 8)) != 0;
}

bool YajProto::setAnalogChannelValue(uint8_t channelIdx, uint16_t value) {
  if (channelIdx >= numAnalogChannels_)
    return false;

  analogChannels_[channelIdx] = value;

  return true;
}

uint16_t YajProto::getAnalogChannelValue(uint8_t channelIdx) {
  if (channelIdx >= numAnalogChannels_)
    return 0;

  return analogChannels_[channelIdx];
}

uint8_t YajProto::writeChannelDataFrameToBuffer(uint8_t* messageBuffer) {
  messageBuffer[0] = channelDataMessageByteCount_;
  messageBuffer[1] = YAJ_PROTO_VERSION;
  messageBuffer[2] = CHANNEL_DATA_FRAME_COMMAND_ID;
  messageBuffer[3] = numDigitalChannels_;
  messageBuffer[4] = numAnalogChannels_;
  uint16_t checkSum = headerCheckSum;
  for (size_t i = 0; i < numDigitalChannelDataBytes_; i++) {
    messageBuffer[5 + i] = digitalChannels_[i];
    checkSum -= digitalChannels_[i];
  }

  for (size_t i = 0; i < numAnalogChannels_; i++) {
    uint8_t lowerByte = analogChannels_[i] & 0xff;
    uint8_t upperByte = analogChannels_[i] >> 8;

    messageBuffer[5 + numDigitalChannelDataBytes_ + (2 * i)] = lowerByte;
    checkSum -= lowerByte;
    messageBuffer[5 + numDigitalChannelDataBytes_ + (2 * i) + 1] = upperByte;
    checkSum -= upperByte;
  }

  messageBuffer[5 + numDigitalChannelDataBytes_ + (2 * numAnalogChannels_)] = checkSum & 0xFF;
  messageBuffer[5 + numDigitalChannelDataBytes_ + (2 * numAnalogChannels_) + 1] = checkSum >> 8;

  return channelDataMessageByteCount_;
}