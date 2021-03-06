

#pragma once

#include "ChunkQueue.hpp"
#include "DataChannel.hpp"
#include "RTCCertificate.hpp"
#include "Logging.hpp"
#include <atomic>
#include <map>

namespace rtcdcpp {

class NiceWrapper;
class DTLSWrapper;
class SCTPWrapper;

struct RTCIceServer {
  std::string hostname;
  int port;
};

std::ostream &operator<<(std::ostream &os, const RTCIceServer &ice_server);

struct RTCConfiguration {
  std::vector<RTCIceServer> ice_servers;
  std::pair<unsigned, unsigned> ice_port_range;
  std::string ice_ufrag;
  std::string ice_pwd;
  std::vector<RTCCertificate> certificates;
};

class PeerConnection {
 public:
  struct IceCandidate {
    IceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex)
        : candidate(candidate), sdpMid(sdpMid), sdpMLineIndex(sdpMLineIndex) {}
    std::string candidate;
    std::string sdpMid;
    int sdpMLineIndex;
  };

  using IceCandidateCallbackPtr = std::function<void(IceCandidate)>;
  using DataChannelCallbackPtr = std::function<void(std::shared_ptr<DataChannel> channel)>;

  PeerConnection(const RTCConfiguration &config, IceCandidateCallbackPtr icCB, DataChannelCallbackPtr dcCB);

  virtual ~PeerConnection();

  const RTCConfiguration &config() { return config_; }

  /**
   *
   * Parse Offer SDP
   */
  void ParseOffer(std::string offer_sdp);

  /**
   * Generate Answer SDP
   */
  std::string GenerateAnswer();

  /**
  * Handle remote ICE Candidate.
  * Supports trickle ice candidates.
  */
  bool SetRemoteIceCandidate(std::string candidate_sdp);

  /**
  * Handle remote ICE Candidates.
  * TODO: Handle trickle ice candidates.
  */
  bool SetRemoteIceCandidates(std::vector<std::string> candidate_sdps);

  /**
   * Create a new data channel with the given label.
   * Only callable once RTCConnectedCallback has been called.
   * TODO: Handle creating data channels before generating SDP, so that the
   *       data channel is created as part of the connection process.
   */
  //    std::shared_ptr<DataChannel> CreateDataChannel(std::string label);

  /**
   * Notify when remote party creates a DataChannel.
   * XXX: This is *not* a callback saying that a call to CreateDataChannel
   *      has succeeded. This is a call saying the remote party wants to
   *      create a new data channel.
   */
  //	void SetDataChannelCreatedCallback(DataChannelCallbackPtr cb);

  // TODO: Error callbacks

  void SendStrMsg(std::string msg, uint16_t sid);
  void SendBinaryMsg(const uint8_t *data, int len, uint16_t sid);

  /* Internal Callback Handlers */
  void OnLocalIceCandidate(std::string &ice_candidate);
  void OnIceReady();
  void OnDTLSHandshakeDone();
  void OnSCTPMsgReceived(ChunkPtr chunk, uint16_t sid, uint32_t ppid);

 private:
  RTCConfiguration config_;
  const IceCandidateCallbackPtr ice_candidate_cb;
  const DataChannelCallbackPtr new_channel_cb;

  std::string mid;

  enum Role { Client, Server } role = Client;

  std::atomic<bool> iceReady{false};
  std::unique_ptr<NiceWrapper> nice;
  std::unique_ptr<DTLSWrapper> dtls;
  std::unique_ptr<SCTPWrapper> sctp;

  std::map<uint16_t, std::shared_ptr<DataChannel>> data_channels;
  std::shared_ptr<DataChannel> GetChannel(uint16_t sid);

  /**
  * Constructor helper
  * Initialize the RTC connection.
  * Allocates all internal structures and configs, and starts ICE gathering.
  */
  bool Initialize();

  // DataChannel message parsing
  void HandleNewDataChannel(ChunkPtr chunk, uint16_t sid);
  void HandleStringMessage(ChunkPtr chunk, uint16_t sid);
  void HandleBinaryMessage(ChunkPtr chunk, uint16_t sid);

  std::shared_ptr<Logger> logger = GetLogger("rtcdcpp.PeerConnection");

};
}
