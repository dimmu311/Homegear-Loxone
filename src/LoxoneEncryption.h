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

		int makeAES256();
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
		std::string _AES256_key;
		std::string _AES256_iv;

		std::string salt;

		
	};
}
#endif