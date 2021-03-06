

/**
 * Simple wrapper around OpenSSL Certs.
 */

#include "rtcdcpp/RTCCertificate.hpp"

#include <openssl/pem.h>

namespace rtcdcpp {

using namespace std;

static std::shared_ptr<X509> GenerateX509(std::shared_ptr<EVP_PKEY> evp_pkey, const std::string &common_name, int days) {
  std::shared_ptr<X509> null_result;

  std::shared_ptr<X509> x509(X509_new(), X509_free);
  std::shared_ptr<BIGNUM> serial_number(BN_new(), BN_free);
  std::shared_ptr<X509_NAME> name(X509_NAME_new(), X509_NAME_free);

  if (!x509 || !serial_number || !name) {
    return null_result;
  }

  if (!X509_set_pubkey(x509.get(), evp_pkey.get())) {
    return null_result;
  }

  if (!BN_pseudo_rand(serial_number.get(), 64, 0, 0)) {
    return null_result;
  }

  ASN1_INTEGER *asn1_serial_number = X509_get_serialNumber(x509.get());
  if (!asn1_serial_number) {
    return null_result;
  }

  if (!BN_to_ASN1_INTEGER(serial_number.get(), asn1_serial_number)) {
    return null_result;
  }

  if (!X509_set_version(x509.get(), 0L)) {
    return null_result;
  }

  if (!X509_NAME_add_entry_by_NID(name.get(), NID_commonName, MBSTRING_UTF8, (unsigned char *)common_name.c_str(), -1, -1, 0)) {
    return null_result;
  }

  if (!X509_set_subject_name(x509.get(), name.get()) || !X509_set_issuer_name(x509.get(), name.get())) {
    return null_result;
  }

  if (!X509_gmtime_adj(X509_get_notBefore(x509.get()), 0) || !X509_gmtime_adj(X509_get_notAfter(x509.get()), days * 24 * 3600)) {
    return null_result;
  }

  if (!X509_sign(x509.get(), evp_pkey.get(), EVP_sha1())) {
    return null_result;
  }

  return x509;
}

static std::string GenerateFingerprint(std::shared_ptr<X509> x509) {
  unsigned int len;
  unsigned char buf[4096] = {0};
  if (!X509_digest(x509.get(), EVP_sha256(), buf, &len)) {
    throw std::runtime_error("GenerateFingerprint(): X509_digest error");
  }

  if (len > SHA256_FINGERPRINT_SIZE) {
    throw std::runtime_error("GenerateFingerprint(): fingerprint size too large for buffer!");
  }

  int offset = 0;
  char fp[SHA256_FINGERPRINT_SIZE];
  memset(fp, 0, SHA256_FINGERPRINT_SIZE);
  for (unsigned int i = 0; i < len; ++i) {
    snprintf(fp + offset, 4, "%02X:", buf[i]);
    offset += 3;
  }
  fp[offset - 1] = '\0';
  return std::string(fp);
}

RTCCertificate RTCCertificate::GenerateCertificate(std::string common_name, int days) {
  std::shared_ptr<EVP_PKEY> pkey(EVP_PKEY_new(), EVP_PKEY_free);
  RSA *rsa = RSA_new();

  std::shared_ptr<BIGNUM> exponent(BN_new(), BN_free);

  if (!pkey || !rsa || !exponent) {
    throw std::runtime_error("GenerateCertificate: !pkey || !rsa || !exponent");
  }

  if (!BN_set_word(exponent.get(), RSA_F4) 
      || !RSA_generate_key_ex(rsa, 2048, exponent.get(), NULL)
      || !EVP_PKEY_assign_RSA(pkey.get(), rsa)
  ){
    throw std::runtime_error("GenerateCertificate: Error generating key");
  }
  auto cert = GenerateX509(pkey, common_name, days);

  if (!cert) {
    throw std::runtime_error("GenerateCertificate: Error in GenerateX509");
  }
  return RTCCertificate(cert, pkey);
}

RTCCertificate::RTCCertificate(std::string cert_pem, std::string pkey_pem) {
  /* x509 */
  BIO *bio = BIO_new(BIO_s_mem());
  BIO_write(bio, cert_pem.c_str(), (int)cert_pem.length());

  x509_ = std::shared_ptr<X509>(PEM_read_bio_X509(bio, nullptr, 0, 0), X509_free);
  BIO_free(bio);
  if (!x509_) {
    throw std::invalid_argument("Could not read cert_pem");
  }

  /* evp_pkey */
  bio = BIO_new(BIO_s_mem());
  BIO_write(bio, pkey_pem.c_str(), (int)pkey_pem.length());

  evp_pkey_ = std::shared_ptr<EVP_PKEY>(PEM_read_bio_PrivateKey(bio, nullptr, 0, 0), EVP_PKEY_free);
  BIO_free(bio);

  if (!evp_pkey_) {
    throw std::invalid_argument("Could not read pkey_pem");
  }

  fingerprint_ = GenerateFingerprint(x509_);
}

RTCCertificate::RTCCertificate(std::shared_ptr<X509> x509, std::shared_ptr<EVP_PKEY> evp_pkey)
    : x509_(x509), evp_pkey_(evp_pkey), fingerprint_(GenerateFingerprint(x509_)) {}
}
