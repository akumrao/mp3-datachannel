
/**
 * DataChannel.
 */

#include <iostream>

#include "rtcdcpp/DataChannel.hpp"
#include "rtcdcpp/PeerConnection.hpp"

namespace rtcdcpp {

DataChannel::DataChannel(PeerConnection *pc, uint16_t stream_id, uint8_t chan_type, std::string label, std::string protocol)
    : pc(pc), stream_id(stream_id), chan_type(chan_type), label(label), protocol(protocol) {
  // XXX: Default-noop callbacks
  open_cb = []() { ; };  // XXX: I love and hate that this is valid c++
  str_msg_cb = [](std::string x) { ; };
  bin_msg_cb = [](ChunkPtr data) { ; };
  closed_cb = []() { ; };
  error_cb = [](std::string x) { ; };
}

DataChannel::~DataChannel() { ; }

uint16_t DataChannel::GetStreamID() { return this->stream_id; }

uint8_t DataChannel::GetChannelType() { return this->chan_type; }

std::string DataChannel::GetLabel() { return this->label; }

std::string DataChannel::GetProtocol() { return this->protocol; }

/**
 * TODO: Close the DataChannel.
 */
void Close() { ; }

bool DataChannel::SendString(std::string msg) {
  std::cerr << "DC: Sending string: " << msg << std::endl;
  this->pc->SendStrMsg(msg, this->stream_id);
  return true;
}

// TODO Take a shared_ptr to datachunk
bool DataChannel::SendBinary(const uint8_t *msg, int len) {
  std::cerr << "DC: Sending binary of len - " << len << std::endl;
  this->pc->SendBinaryMsg(msg, len, this->stream_id);
  std::cerr << "DC: Binary sent" << std::endl;
  return true;
}

void DataChannel::SetOnOpen(std::function<void()> open_cb) { this->open_cb = open_cb; }

void DataChannel::SetOnStringMsgCallback(std::function<void(std::string msg)> str_msg_cb) { this->str_msg_cb = str_msg_cb; }

void DataChannel::SetOnBinaryMsgCallback(std::function<void(ChunkPtr)> bin_msg_cb) { this->bin_msg_cb = bin_msg_cb; }

void DataChannel::SetOnClosedCallback(std::function<void()> closed_cb) { this->closed_cb = closed_cb; }

void DataChannel::SetOnErrorCallback(std::function<void(std::string description)> error_cb) { this->error_cb = error_cb; }

void DataChannel::OnOpen() {
  if (this->open_cb) {
    this->open_cb();
  }
}

void DataChannel::OnStringMsg(std::string msg) {
  if (this->str_msg_cb) {
    this->str_msg_cb(msg);
  }
}

void DataChannel::OnBinaryMsg(ChunkPtr msg) {
  if (this->bin_msg_cb) {
    this->bin_msg_cb(msg);
  }
}

void DataChannel::OnClosed() {
  if (this->closed_cb) {
    this->closed_cb();
  }
}

void DataChannel::OnError(std::string description) {
  if (this->error_cb) {
    this->error_cb(description);
  }
}
}
