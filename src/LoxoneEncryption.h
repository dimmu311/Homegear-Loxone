#ifndef LOXONEENCRYPTION_H_
#define LOXONEENCRYPTION_H_

#include <string>
#include "GD.h"

namespace Loxone
{
	class LoxoneEncryption
	{
	public:
		LoxoneEncryption();


		int buildSessionKey(std::string& RSA_encrypted);
		void deInitGnuTls();
		void initGnuTls();

		std::string encryptCommand(std::string command);
		std::string hashPassword(std::string user, std::string password);

		void setPublicKey(std::string certificate);
		void setKey(std::string hexKey);
		
		std::string _key;
		std::string _salt;

	private:
		std::string _publicKey;
		std::string _myAes256key_iv;
        std::string _myAes256key;
        std::string _myAes256iv;
        std::string _mySalt;
        uint32_t _mySaltUsageCounter;

        std::string getRandomHexString(uint32_t len);
        std::string getNewSalt();
        std::string getSalt();
        std::string getNewAes256();
    };
}
#endif