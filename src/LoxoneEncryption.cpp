#include "LoxoneEncryption.h"

namespace Loxone
{
	GCRY_THREAD_OPTION_PTHREAD_IMPL;
	
	LoxoneEncryption::LoxoneEncryption(const std::string user, const std::string password)
	{
	    _user = user;
	    _password = password;

        _publicKey = new gnutls_datum_t;
        _myAes256key = new gnutls_datum_t;
        _myAes256iv = new gnutls_datum_t;
        _myAes256key_iv = new gnutls_datum_t;

	    initGnuTls();

		_mySaltUsageCounter = 0;
		_mySalt = getNewSalt();
		getNewAes256();
	}

	LoxoneEncryption::~LoxoneEncryption()
    {
        gnutls_cipher_deinit(_Aes256Handle);
        deInitGnuTls();
    }

	std::string LoxoneEncryption::getRandomHexString(uint32_t len)
    {
        unsigned char* buffer = new unsigned char[len];
        gnutls_rnd(GNUTLS_RND_KEY, buffer, len);
        return BaseLib::HelperFunctions::getHexString(buffer, len);
    }

	std::string LoxoneEncryption::getNewSalt()
    {
	    return getRandomHexString(16);
    }

	std::string LoxoneEncryption::getSalt()
    {
        std::string salt = _mySalt;
        if(_mySaltUsageCounter >= 10)
        {
            _mySalt = getNewSalt();
            salt = "nextSalt/" + salt + "/" +_mySalt + "/";
            _mySaltUsageCounter = 0;
            return salt;
        }
        _mySaltUsageCounter++;
        return "salt/"+salt+"/";
    }

    uint32_t LoxoneEncryption::getNewAes256()
    {
        try
        {
            key = getRandomHexString(16);
            iv = getRandomHexString(8);

            //memcpy(_myAes256key->data, (unsigned char*)key.data(),key.size());
            _myAes256key->data = (unsigned char*)key.data();
            _myAes256key->size = key.size();

            //memcpy(_myAes256iv->data, (unsigned char*)iv.data(),iv.size());
            _myAes256iv->data = (unsigned char*)iv.data();
            _myAes256iv->size = iv.size();

            if (gnutls_cipher_init(&_Aes256Handle, GNUTLS_CIPHER_AES_256_CBC, _myAes256key, _myAes256iv)<0)
            {
                GD::out.printError("gnutls_cipher_init failed");
                return -1;
            }

            aesKey_Iv.insert(aesKey_Iv.begin(), key.begin(), key.end());
            aesKey_Iv.push_back(':');
            aesKey_Iv.insert(aesKey_Iv.end(), iv.begin(), iv.end());

            //memcpy(_myAes256key_iv->data, (unsigned char*)aesKey_Iv.data(),aesKey_Iv.size());
            _myAes256key_iv->data=(unsigned char*)aesKey_Iv.data();
            _myAes256key_iv->size = aesKey_Iv.size();

            return 0;
        }
        catch (const std::exception& ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }

	void LoxoneEncryption::setPublicKey(const std::string certificate)
	{
	    std::string publicKey = certificate;
        publicKey.erase(252, 11);
        publicKey.insert(252, "PUBLIC KEY");

        publicKey.erase(11, 11);
        publicKey.insert(11, "PUBLIC KEY");

        memcpy(_publicKey->data, (unsigned char*)publicKey.data(),publicKey.size());
		_publicKey->size = publicKey.size();
	}

	void LoxoneEncryption::setKey(std::string hexKey)
	{
        _loxKey.clear();

        for (uint32_t i = 0; i < hexKey.size(); i += 2) {
            std::string s = hexKey.substr(i, 2);
            int value = std::stoi(s, nullptr, 16);
            _loxKey += (char) value;
        }
    }

    void LoxoneEncryption::setSalt(const std::string salt)
    {
	    _loxSalt = salt;
    }

	uint32_t LoxoneEncryption::buildSessionKey(std::string& rsaEncrypted)
	{
		gnutls_pubkey_t publicKey;
		if (gnutls_pubkey_init(&publicKey) < 0)
		{
			GD::out.printError("gnutls_pubkey_init failed");
			return -1;
		}

		if (GNUTLS_E_SUCCESS != gnutls_pubkey_import(publicKey, _publicKey, GNUTLS_X509_FMT_PEM))
		{
			GD::out.printError("Error: Failed to read public key (e).");
			gnutls_pubkey_deinit(publicKey);
			return-1;
		}

		gnutls_datum_t ciphertext;
		int result = gnutls_pubkey_encrypt_data(publicKey, 0, _myAes256key_iv, &ciphertext);
		if (result != GNUTLS_E_SUCCESS || ciphertext.size == 0)
		{
			GD::out.printError("Error: Failed to encrypt data.");
			gnutls_pubkey_deinit(publicKey);
			if (ciphertext.data) gnutls_free(ciphertext.data);
			return-1;
		}

        std::string ciphertextString(reinterpret_cast<const char *>(ciphertext.data), ciphertext.size);
        BaseLib::Base64::encode(ciphertextString, rsaEncrypted);

		return 0;
	}
	uint32_t LoxoneEncryption::encryptCommand(const std::string command, std::string& encryptedCommand)
	{
		try
		{
            uint32_t blocksize = gnutls_cipher_get_block_size(GNUTLS_CIPHER_AES_256_CBC);
			std::string plaintextString = getSalt() + command + "\0";

			std::vector<char> plaintext(plaintextString.begin(), plaintextString.end());
            while(plaintext.size()%blocksize > 0)
            {
                plaintext.push_back('\0');
            }

            gnutls_cipher_set_iv(_Aes256Handle, _myAes256iv->data, _myAes256iv->size);
            unsigned char encrypted[plaintext.size()];
            if(gnutls_cipher_encrypt2(_Aes256Handle, plaintext.data(), plaintext.size(), encrypted, plaintext.size())< 0)
            {
                GD::out.printError("gnutls_cipher_encrypt2 failed");
                return -1;
            }

            std::string encryptedString(reinterpret_cast<const char *>(encrypted), plaintext.size());
			std::string Base64EncryptedString;
			BaseLib::Base64::encode(encryptedString, Base64EncryptedString);
            encryptedCommand = BaseLib::Http::encodeURL(Base64EncryptedString);
            return 0;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return -1;
		}
	}
	
	uint32_t LoxoneEncryption::hashPassword(std::string& hashedPassword)
	{
        try
        {
            {
                int hashedLen = gnutls_hash_get_len(GNUTLS_DIG_SHA1);
                unsigned char hashed[hashedLen];
                std::string ptext = _password + ":" + _loxSalt;
                if(gnutls_hash_fast(GNUTLS_DIG_SHA1, ptext.c_str(), ptext.size(), &hashed)<0)
                {
                    GD::out.printError("GNUTLS_DIG_SHA1 failed");
                    return -1;
                }
                hashedPassword = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
            }
            {
                int hashedLen = gnutls_hmac_get_len(GNUTLS_MAC_SHA1);
                unsigned char hashed[hashedLen];
                std::string ptext = _user + ":" + hashedPassword;
                if(gnutls_hmac_fast(GNUTLS_MAC_SHA1, _loxKey.c_str(), _loxKey.size(), ptext.c_str(), ptext.size(), &hashed)<0)
                {
                    GD::out.printError("GNUTLS_MAC_SHA1 failed");
                    return -1;
                }

                hashedPassword = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
                hashedPassword = BaseLib::HelperFunctions::toLower(hashedPassword);
            }
            return 0;
        }
        catch (const std::exception &ex) {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
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
}