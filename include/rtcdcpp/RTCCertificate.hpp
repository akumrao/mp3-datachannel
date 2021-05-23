

#pragma once

/**
 * Wrapper around OpenSSL Certs.
 */

#include <openssl/x509.h>
#include <memory>
#include <cstring>
#include <string>

namespace rtcdcpp {

#define SHA256_FINGERPRINT_SIZE (95 + 1)

class RTCCertificate {
 public:
  static RTCCertificate GenerateCertificate(std::string common_name, int days);

  RTCCertificate(std::string cert_pem, std::string pkey_pem);

  const std::string &fingerprint() const { return fingerprint_; }

 protected:
  friend class DTLSWrapper;

  X509 *x509() const { return x509_.get(); }
  EVP_PKEY *evp_pkey() const { return evp_pkey_.get(); }

 private:
  RTCCertificate(std::shared_ptr<X509> x509, std::shared_ptr<EVP_PKEY> evp_pkey);

  std::shared_ptr<X509> x509_;
  std::shared_ptr<EVP_PKEY> evp_pkey_;
  std::string fingerprint_;
};
}
