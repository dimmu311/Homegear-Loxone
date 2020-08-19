#ifndef LOXONEENCRYPTION_H_
#define LOXONEENCRYPTION_H_

#include "GnutlsData.h"
#include <string>
#include "../GD.h"
#include <gnutls/gnutls.h>
#include <gnutls/abstract.h>
#include <gnutls/crypto.h>

namespace Loxone
{
	class LoxoneEncryption
	{
	public:
		LoxoneEncryption(const std::string user, const std::string password, BaseLib::SharedObjects* bl);
        ~LoxoneEncryption();

		uint32_t buildSessionKey(std::string& rsaEncrypted);

		uint32_t encryptCommand(const std::string command, std::string& encryptedCommand);
        uint32_t decryptCommand(const std::string encryptedCommand, std::string& command);
        uint32_t hashPassword(std::string& hashedPassword);
        uint32_t hashToken(std::string& hashedToken);

		void setPublicKey(const std::string certificate);
		void setKey(const std::string hexKey);
		void setSalt(const std::string salt);
		uint32_t setHashAlgorithm(const std::string algorithm);
        uint32_t setToken(const BaseLib::PVariable value);
        uint32_t setToken(const std::string jwt);
        uint32_t getToken(std::string& token, std::time_t& lifeTime);

	private:
        BaseLib::SharedObjects* _bl;

        uint64_t _tokenValidUntil;
	    std::shared_ptr<GnutlsData> _publicKey;
        std::shared_ptr<GnutlsData> _myAes256key;
        std::shared_ptr<GnutlsData> _myAes256iv;
        std::shared_ptr<GnutlsData> _myAes256key_iv;

        std::string _user;
        std::string _password;

        std::string _loxKey;
        std::string _loxSalt;
        std::string _loxToken;

        gnutls_digest_algorithm_t _digestAlgorithm;
        gnutls_mac_algorithm_t _macAlgorithm;

        gnutls_cipher_hd_t _Aes256Handle;
        std::string _mySalt;
        uint32_t _mySaltUsageCounter;

        std::unique_ptr<BaseLib::Rpc::JsonDecoder> _jsonDecoder;

        std::string getRandomHexString(uint32_t len);
        std::string getNewSalt();
        std::string getSalt();
        void removeSalt(std::string& command);
        uint32_t getNewAes256();

        void deInitGnuTls();
        void initGnuTls();
    };
}
#endif