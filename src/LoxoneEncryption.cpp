#include "LoxoneEncryption.h"
#include <gnutls/gnutls.h>
#include <gnutls/abstract.h>
#include <gnutls/crypto.h>

#include <curl/curl.h>

namespace Loxone
{
	GCRY_THREAD_OPTION_PTHREAD_IMPL;
	
	LoxoneEncryption::LoxoneEncryption()
	{
		initGnuTls();

		int salt_len = 16;
		unsigned char* salt_buffer = new unsigned char[salt_len];
		gnutls_rnd(GNUTLS_RND_KEY, salt_buffer, salt_len);
		salt = BaseLib::HelperFunctions::getHexString(salt_buffer, salt_len);
	}

	void LoxoneEncryption::setPublicKey(std::string certificate)
	{
		certificate.erase(252, 11);
		certificate.insert(252, "PUBLIC KEY");

		certificate.erase(11, 11);
		certificate.insert(11, "PUBLIC KEY");

		_publicKey = certificate;
	}

	void LoxoneEncryption::setKey(std::string hexKey)
	{
		_key.clear();

		for (uint32_t i = 0; i < hexKey.size(); i += 2)
		{
			std::string s = hexKey.substr(i, 2);
			int value = std::stoi(s, nullptr, 16);
			_key += (char)value;
		}
	}

	int LoxoneEncryption::makeAES256()
	{
		try
		{
			int AES256_key_len = 16;
			unsigned char* AES256_key_buffer = new unsigned char[AES256_key_len];
			gnutls_rnd(GNUTLS_RND_KEY, AES256_key_buffer, AES256_key_len);
			_AES256_key = BaseLib::HelperFunctions::getHexString(AES256_key_buffer, AES256_key_len);

			int AES256_iv_len = 8;
			unsigned char* AES256_iv_buffer = new unsigned char[AES256_iv_len];
			gnutls_rnd(GNUTLS_RND_KEY, AES256_iv_buffer, AES256_iv_len);
			_AES256_iv = BaseLib::HelperFunctions::getHexString(AES256_iv_buffer, AES256_iv_len);

			return 0;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return -1;
		}
		return 0;
	}

	int LoxoneEncryption::buildSessionKey(std::string& RSA_encrypted)
	{
		int result;
		gnutls_pubkey_t publicKey;

		if (gnutls_pubkey_init(&publicKey) < 0)
		{
			GD::out.printError("gnutls_pubkey_init failed");
			return -1;
		}

		// Construct the pre-shared key in GnuTLS's 'datum' structure, whose definition is as follows:
		// typedef struct {
		//	unsigned char *data;
		//  unsigned int size;
		//	} gnutls_datum_t;
		gnutls_datum_t key;
		key.size = _publicKey.size();
		key.data = (unsigned char*)_publicKey.data();

		if (GNUTLS_E_SUCCESS != gnutls_pubkey_import(publicKey, &key, GNUTLS_X509_FMT_PEM))
		{
			GD::out.printError("Error: Failed to read public key (e).");
			gnutls_pubkey_deinit(publicKey);
			return-1;
		}

		std::string plaintextString = _AES256_key + ":" + _AES256_iv;

		gnutls_datum_t plaintext;
		plaintext.data = (unsigned char*)plaintextString.data();
		plaintext.size = plaintextString.size();

		gnutls_datum_t ciphertext;

		result = gnutls_pubkey_encrypt_data(publicKey, 0, &plaintext, &ciphertext);
		if (result != GNUTLS_E_SUCCESS || ciphertext.size == 0)
		{
			GD::out.printError("Error: Failed to encrypt data.");
			gnutls_pubkey_deinit(publicKey);
			if (ciphertext.data) gnutls_free(ciphertext.data);
			return-1;
		}

		std::stringstream RSA_encryptedStringStream;
		for (uint32_t i = 0; i < ciphertext.size; i++)
		{
			RSA_encryptedStringStream << ciphertext.data[i];
		}

		std::string x(reinterpret_cast<const char *>(ciphertext.data), ciphertext.size);
		std::string RSA_encrypted2 = "";
		BaseLib::Base64::encode(x, RSA_encrypted2);

		BaseLib::Base64::encode(RSA_encryptedStringStream.str(), RSA_encrypted);

		return 0;
	}

	void LoxoneEncryption::deInitGnuTls()
	{
		// {{{ DeInit gcrypt and GnuTLS
		gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
		gcry_control(GCRYCTL_TERM_SECMEM);
		gcry_control(GCRYCTL_RESUME_SECMEM_WARN);

		gnutls_global_deinit();
		// }}}
	}

	void LoxoneEncryption::initGnuTls()
	{
		// {{{ Init gcrypt and GnuTLS
		gcry_error_t gcryResult;
		if ((gcryResult = gcry_control(GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread)) != GPG_ERR_NO_ERROR)
		{
			GD::out.printCritical("Critical: Could not enable thread support for gcrypt.");
			exit(2);
		}

		if (!gcry_check_version(GCRYPT_VERSION))
		{
			GD::out.printCritical("Critical: Wrong gcrypt version.");
			exit(2);
		}
		gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
		int secureMemorySize = 16384;
		if ((gcryResult = gcry_control(GCRYCTL_INIT_SECMEM, (int)secureMemorySize, 0)) != GPG_ERR_NO_ERROR)
		{
			GD::out.printCritical("Critical: Could not allocate secure memory. Error code is: " + std::to_string((int32_t)gcryResult));
			exit(2);
		}
		gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
		gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

		gnutls_global_init();
		// }}}
	}

	std::string LoxoneEncryption::encryptCommand(std::string command)
	{
		try
		{
			std::string toEncrypt = "salt/" + salt + "/" + command + "\0";
            std::vector<char> cstr(toEncrypt.begin(), toEncrypt.end());

            while(cstr.size()%4 > 0)
            {
                cstr.push_back(NULL);
            }
			gnutls_cipher_hd_t handle;
			gnutls_datum_t key;
			key.data = (unsigned char*)_AES256_key.data();
			key.size = _AES256_key.size();

			gnutls_datum_t iv;
			iv.data = (unsigned char*)_AES256_iv.data();
			iv.size = _AES256_iv.size();

			gnutls_cipher_init(&handle, GNUTLS_CIPHER_AES_256_CBC, &key, &iv);

			uint32_t toEncryptSize = toEncrypt.size();
			unsigned char encrypted[cstr.size()];

			uint32_t blocksize = gnutls_cipher_get_block_size(GNUTLS_CIPHER_AES_256_CBC);

            uint32_t padLength = ((toEncryptSize/ blocksize) + 1) * blocksize - toEncryptSize;
			
			gnutls_cipher_encrypt2(handle, cstr.data(), cstr.size(), &encrypted, cstr.size());
			
			std::string encryptedString;
			for (uint32_t i = 0; i < toEncryptSize + padLength; i++)
			{
				encryptedString += encrypted[i];
			}
			
			std::string Base64EncryptedString;
			BaseLib::Base64::encode(encryptedString, Base64EncryptedString);
            auto b = BaseLib::Http::encodeURL(Base64EncryptedString);


			std::stringstream stream;
			CURL* curl = curl_easy_init();
			if (curl) {

				char* output = curl_easy_escape(curl, Base64EncryptedString.c_str(), Base64EncryptedString.size()) ;
				if (output) {

					stream << output;

					curl_free(output);
				}
			}
			return stream.str();



			//gnutls_cipher_deinit(handle);
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return"";
		}
		return"";
	}
	
	std::string LoxoneEncryption::hashPassword(std::string user, std::string password)
	{
		try
		{
			std::string hashedString;
			{
				int hashedLen = gnutls_hash_get_len(GNUTLS_DIG_SHA1);
				unsigned char hashed[hashedLen];
				std::string ptext = password + ":" + _salt;

				gnutls_hash_fast(GNUTLS_DIG_SHA1, ptext.c_str(), ptext.size(), &hashed);

				hashedString = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
			}
			
			{
				int hashedLen = gnutls_hmac_get_len(GNUTLS_MAC_SHA1);
				unsigned char hashed[hashedLen];
				
				std::string ptext = user + ":" + hashedString;
				gnutls_hmac_fast(GNUTLS_MAC_SHA1, _key.c_str(), _key.size(), ptext.c_str(), ptext.size(), &hashed);

				hashedString = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
				hashedString = BaseLib::HelperFunctions::toLower(hashedString);
			}
			return hashedString;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return"";
		}
		return"";
	}
}