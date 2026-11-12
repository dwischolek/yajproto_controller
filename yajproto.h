#pragma once

class YajProto {
public:
  static const uint8_t YAJ_PROTO_VERSION = 0x01;
  static const uint8_t CHANNEL_DATA_FRAME_COMMAND_ID = 0x01;

  YajProto(uint8_t numDigitalChannels, uint8_t numAnalogChannels);

  uint8_t getNumDigitalChannels();
  uint8_t getNumAnalogChannels();
  uint8_t getChannelDataMessageByteCount();

  bool setDigitalChannelValue(uint8_t channelIdx, bool value);
  bool getDigitalChannelValue(uint8_t channelIdx);

  bool setAnalogChannelValue(uint8_t channelIdx, uint16_t value);
  uint16_t getAnalogChannelValue(uint8_t channelIdx);

  uint8_t writeChannelDataFrameToBuffer(uint8_t *messageBuffer);


private:
  uint8_t channelDataMessageByteCount_;
  uint8_t numDigitalChannels_;
  uint8_t numAnalogChannels_;
  uint8_t *digitalChannels_;
  uint16_t *analogChannels_;
  uint16_t headerCheckSum;

  uint8_t numDigitalChannelDataBytes_;
};