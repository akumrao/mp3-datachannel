

#pragma once

/**
 * Wrapper around libnice and NiceAgent.
 */

#include "ChunkQueue.hpp"
#include "PeerConnection.hpp"
#include "Logging.hpp"

#include <thread>

extern "C" {
#include <nice/agent.h>
}

namespace rtcdcpp {

/**
 * Nice Wrapper broh.
 */
class NiceWrapper {
 public:
  // TODO: Remove reference to handler
  // TODO: Add callback for candidates
  NiceWrapper(PeerConnection *peer_connection);
  virtual ~NiceWrapper();

  // Setup libnice
  bool Initialize();

  // Start sending packets XXX: recv just happens once candidates are set
  void StartSendLoop();

  // Shutdown nice and stop the send thread
  void Stop();

  // Parse the remote SDP
  void ParseRemoteSDP(std::string remote_sdp);

  // void SetRemoteCredentials(std::string username, std::string password);

  // Generate the local SDP
  std::string GenerateLocalSDP();

  // Add a single remote ice candidate (supports trickling)
  bool SetRemoteIceCandidate(std::string candidate_sdp);

  // Set the remote ice candidates
  bool SetRemoteIceCandidates(std::vector<std::string> candidate_sdps);

  // Callback to call when we receive local ice candidates
  // void SetLocalCandidatesCallback(std::vector<std::string> candidate_sdps);

  // Callback to call when we receive remote data
  void SetDataReceivedCallback(std::function<void(ChunkPtr)>);

  // Send data over the nice channel
  void SendData(ChunkPtr chunk);

 private:
  PeerConnection *peer_connection;
  int packets_sent;

  std::unique_ptr<NiceAgent, void (*)(gpointer)> agent;
  std::unique_ptr<GMainLoop, void (*)(GMainLoop *)> loop;
  uint32_t stream_id;
  std::mutex send_lock;

  bool gathering_done;
  bool negotiation_done;

  ChunkQueue send_queue;

  std::function<void(ChunkPtr)> data_received_callback;

  // Send data thread
  void SendLoop();
  std::thread send_thread;
  std::thread g_main_loop_thread;
  std::atomic<bool> should_stop;

  // Callback methods
  void OnStateChange(uint32_t stream_id, uint32_t component_id, uint32_t state);
  void OnGatheringDone();
  void OnCandidate(std::string candidate);
  void OnSelectedPair();
  void OnDataReceived(const uint8_t *buf, int len);
  void OnIceReady();
  void LogMessage(const gchar *message);

  // Helper functions
  friend void candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer user_data);
  friend void component_state_changed(NiceAgent *agent, guint stream_id, guint component_id, guint state, gpointer user_data);
  friend void new_local_candidate(NiceAgent *agent, NiceCandidate *candidate, gpointer user_data);
  friend void new_selected_pair(NiceAgent *agent, guint stream_id, guint component_id, NiceCandidate *lcandidate, NiceCandidate *rcandidate,
                                gpointer user_data);
  friend void data_received(NiceAgent *agent, guint stream_id, guint component_id, guint len, gchar *buf, gpointer user_data);
  friend void nice_log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);

  std::shared_ptr<Logger> logger = GetLogger("rtcdcpp.Nice");
};
}
