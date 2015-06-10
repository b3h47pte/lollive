#include "Authentication.h"

#include "hmac.h"
#include "sha.h"
#include "filters.h"
#include "secblock.h"
#include "hex.h"
#include "base64.h"
#include "files.h"

Authentication* Authentication::singleton = new Authentication();

std::string Authentication::CalculateHMAC(const std::string& message, const std::string& key) const {
  CryptoPP::SecByteBlock hkey((const byte*)key.data(), key.size());
  CryptoPP::HMAC<CryptoPP::SHA256> hmac;
  hmac.SetKey(hkey, hkey.size());

  std::string output;
  CryptoPP::StringSource ss(message, true, new CryptoPP::HashFilter(hmac, new CryptoPP::HexEncoder(new CryptoPP::StringSink(output))));
  return output;
}

Authentication::Authentication() {
}

Authentication::~Authentication() {
}
