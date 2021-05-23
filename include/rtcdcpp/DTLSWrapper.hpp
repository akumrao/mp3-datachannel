

#pragma once

/**
 * Wrapper around OpenSSL DTLS.
 */

#include "ChunkQueue.hpp"
#include "PeerConnection.hpp"
#include "Logging.hpp"

#include <openssl/ssl.h>

#include <thread>

namespace rtcdcpp {

class DTLSWrapper {
 public:
  DTLSWrapper(PeerConnection *peer_connection);
  virtual ~DTLSWrapper();

  const RTCCertificate *certificate() { return certificate_; }

  bool Initialize();
  void Start();
  void Stop();

  void EncryptData(ChunkPtr chunk);
  void DecryptData(ChunkPtr chunk);

  void SetEncryptedCallback(std::function<void(ChunkPtr chunk)>);
  void SetDecryptedCallback(std::function<void(ChunkPtr chunk)>);

 private:
  PeerConnection *peer_connection;
  const RTCCertificate *certificate_;

  std::atomic<bool> should_stop;

  ChunkQueue encrypt_queue;
  ChunkQueue decrypt_queue;

  std::thread encrypt_thread;
  std::thread decrypt_thread;

  void RunEncrypt();
  void RunDecrypt();

  // SSL Context
  std::mutex ssl_mutex;
  SSL_CTX *ctx;
  SSL *ssl;
  BIO *in_bio, *out_bio;

  bool handshake_complete;

  std::function<void(ChunkPtr chunk)> decrypted_callback;
  std::function<void(ChunkPtr chunk)> encrypted_callback;

  std::shared_ptr<Logger> logger = GetLogger("rtcdcpp.DTLS");
};
}
