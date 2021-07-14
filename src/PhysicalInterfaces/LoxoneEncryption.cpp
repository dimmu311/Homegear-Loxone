#include "LoxoneEncryption.h"
#include <ctime>
#include "../Loxone.h"

namespace Loxone
{
	GCRY_THREAD_OPTION_PTHREAD_IMPL;
	
	LoxoneEncryption::LoxoneEncryption(std::shared_ptr<BaseLib::Systems::PhysicalInterfaceSettings> settings)
	{
        _out.init(GD::bl);
        _out.setPrefix(GD::out.getPrefix() + "Encryption: ");

        if(!settings){
            _out.printCritical("Critical: Error initializing. Settings pointer is empty.");
            return;
        }

        if(settings->user.empty()){
            _out.printCritical("Critical: Error initializing. User is not set in loxone.conf.");
            return;
        }
	    _user = settings->user;

        if(settings->password.empty()){
            _out.printCritical("Critical: Error initializing. Password is not set in loxone.conf.");
            return;
        }
	    _password = settings->password;

        if(settings->passwordS21.empty()){
            _out.printCritical("Critical: Error initializing. Visu Password is not set in loxone.conf.");
            return;
        }
	    _visuPassword = settings->passwordS21;

        auto token = GD::family->getFamilySetting("token");
        if(token) setToken(token->stringValue);

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
        std::vector<uint8_t> buffer;
        buffer.resize(len);
        gnutls_rnd(GNUTLS_RND_KEY, buffer.data(), buffer.size());
        return BaseLib::HelperFunctions::getHexString(buffer);
    }

	std::string LoxoneEncryption::getNewSalt()
    {
	    return getRandomHexString(16);
    }

	std::string LoxoneEncryption::getSalt()
    {
        std::string salt = _mySalt;
        if(_mySaltUsageCounter >= 10){
            _mySalt = getNewSalt();
            salt = "nextSalt/" + salt + "/" + _mySalt + "/";
            _mySaltUsageCounter = 0;
            return salt;
        }
        _mySaltUsageCounter++;
        return "salt/" + salt + "/";
    }

    void LoxoneEncryption::removeSalt(std::string& command)
    {
	    //check if need to remove nextSalt?
        if (command.compare(0, 9, "nextSalt/") == 0)
        {
            //remove "nextSalt/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/" at the beginning
            command.erase(0,42);
        }
        //remove "salt/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/" at the beginning
        command.erase(0,38);
    }

    uint32_t LoxoneEncryption::getNewAes256()
    {
        try
        {
            std::string key = getRandomHexString(16);
            std::string iv = getRandomHexString(8);

            _myAes256key = std::make_shared<GnutlsData>(key);
            _myAes256iv = std::make_shared<GnutlsData>(iv);

            if (gnutls_cipher_init(&_Aes256Handle, GNUTLS_CIPHER_AES_256_CBC, _myAes256key->getData(), _myAes256iv->getData())<0)
            {
                _out.printError("gnutls_cipher_init failed");
                return -1;
            }
            _myAes256key_iv = std::make_shared<GnutlsData>(key + ":"+ iv);
            return 0;
        }
        catch (const std::exception& ex)
        {
           _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }

	void LoxoneEncryption::setPublicKey(const std::string certificate)
	{
	    std::string publicKey = std::regex_replace(certificate, std::regex("CERTIFICATE"), "PUBLIC KEY");
        _out.printDebug("Public Key after regex: " + publicKey, 6);
        _publicKey = std::make_shared<GnutlsData>(publicKey);
	}

	void LoxoneEncryption::setKey(std::string hexKey)
	{
        std::vector<uint8_t> x = BaseLib::HelperFunctions::hexToBin(hexKey);
        _loxKey = std::string(x.begin(), x.end());
	}

    void LoxoneEncryption::setSalt(const std::string salt)
    {
	    _loxSalt = salt;
    }

    uint32_t LoxoneEncryption::setHashAlgorithm(const std::string algorithm)
    {
	    if(algorithm == "SHA1"){
            _digestAlgorithm = GNUTLS_DIG_SHA1;
            _macAlgorithm = GNUTLS_MAC_SHA1;
            return 0;
        }
	    else if(algorithm == "SHA256"){
            _digestAlgorithm = GNUTLS_DIG_SHA256;
            _macAlgorithm = GNUTLS_MAC_SHA256;
            return 0;
	    }
	    else{
            _out.printError("given Hash Algorithm not support.");
            return -1;
        }
    }

    void LoxoneEncryption::setVisuKey(std::string hexKey)
    {
        std::vector<uint8_t> x = BaseLib::HelperFunctions::hexToBin(hexKey);
        _loxVisuKey = std::string(x.begin(), x.end());
    }

    void LoxoneEncryption::setVisuSalt(const std::string salt)
    {
        _loxVisuSalt = salt;
    }

    uint32_t LoxoneEncryption::setVisuHashAlgorithm(std::string algorithm)
    {
        if(algorithm == "SHA1"){
            _visuDigestAlgorithm = GNUTLS_DIG_SHA1;
            _visuMacAlgorithm = GNUTLS_MAC_SHA1;
            return 0;
        }
        else if(algorithm == "SHA256"){
            _visuDigestAlgorithm = GNUTLS_DIG_SHA256;
            _visuMacAlgorithm = GNUTLS_MAC_SHA256;
            return 0;
        }
        else{
            _out.printError("given Hash Algorithm not support.");
            return -1;
        }
    }

    void LoxoneEncryption::setHashedVisuPassword(const std::string hashedPassword)
    {
	    _hashedVisuPassword = hashedPassword;
    }
    std::string LoxoneEncryption::getHashedVisuPassword()
    {
	    return _hashedVisuPassword;
    }

	uint32_t LoxoneEncryption::buildSessionKey(std::string& rsaEncrypted)
	{
		gnutls_pubkey_t publicKey;
		if (gnutls_pubkey_init(&publicKey) < 0){
			_out.printError("gnutls_pubkey_init failed");
			return -1;
		}

		if (GNUTLS_E_SUCCESS != gnutls_pubkey_import(publicKey, _publicKey->getData(), GNUTLS_X509_FMT_PEM)){
			_out.printError("Error: Failed to read public key (e).");
			gnutls_pubkey_deinit(publicKey);
			return-1;
		}

		gnutls_datum_t ciphertext;
		int result = gnutls_pubkey_encrypt_data(publicKey, 0, _myAes256key_iv->getData(), &ciphertext);
		if (result != GNUTLS_E_SUCCESS || ciphertext.size == 0){
			_out.printError("Error: Failed to encrypt data.");
			gnutls_pubkey_deinit(publicKey);
			if (ciphertext.data) gnutls_free(ciphertext.data);
			return-1;
		}

        std::string ciphertextString(reinterpret_cast<const char *>(ciphertext.data), ciphertext.size);
        BaseLib::Base64::encode(ciphertextString, rsaEncrypted);

        gnutls_pubkey_deinit(publicKey);
        if (ciphertext.data) gnutls_free(ciphertext.data);

		return 0;
	}
	uint32_t LoxoneEncryption::encryptCommand(const std::string command, std::string& encryptedCommand)
	{
		try{
            uint32_t blocksize = gnutls_cipher_get_block_size(GNUTLS_CIPHER_AES_256_CBC);
			std::string plaintextString = getSalt() + command + "\0";

			std::vector<char> plaintext(plaintextString.begin(), plaintextString.end());
            while(plaintext.size()%blocksize > 0){
                plaintext.push_back('\0');
            }

            unsigned char encrypted[plaintext.size()];
            gnutls_cipher_set_iv(_Aes256Handle, _myAes256iv->getData()->data, _myAes256iv->getData()->size);
            if(gnutls_cipher_encrypt2(_Aes256Handle, plaintext.data(), plaintext.size(), encrypted, plaintext.size())< 0){
                _out.printError("gnutls_cipher_encrypt2 failed");
                return -1;
            }

            std::string encryptedString(reinterpret_cast<const char *>(encrypted), plaintext.size());
			std::string Base64EncryptedString;
			BaseLib::Base64::encode(encryptedString, Base64EncryptedString);
            encryptedCommand = "jdev/sys/enc/" + BaseLib::Http::encodeURL(Base64EncryptedString);
            return 0;
		}
		catch (const std::exception& ex){
			_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return -1;
		}
	}

    uint32_t LoxoneEncryption::decryptCommand(const std::string encryptedCommand, std::string& command)
    {
        try{
            std::string Base64DecryptedString;
            BaseLib::Base64::decode(encryptedCommand, Base64DecryptedString);

            unsigned char decrypted[encryptedCommand.size()];
            gnutls_cipher_set_iv(_Aes256Handle, _myAes256iv->getData()->data, _myAes256iv->getData()->size);
            if(gnutls_cipher_decrypt2(_Aes256Handle, Base64DecryptedString.data(), Base64DecryptedString.size(),decrypted,Base64DecryptedString.size()) <0){
                _out.printError("gnutls_cipher_decrypt2 failed");
                return -1;
            }
            command = std::string(reinterpret_cast<const char *>(decrypted), Base64DecryptedString.size());
            command.erase(std::find(command.begin(), command.end(), '\0'), command.end());
            removeSalt(command);
            return 0;
        }
        catch (const std::exception& ex){
            _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }

	uint32_t LoxoneEncryption::hashPassword(std::string& hashedPassword)
	{
	    try{
            {
                int hashedLen = gnutls_hash_get_len(_digestAlgorithm);
                unsigned char hashed[hashedLen];
                std::string ptxt = _password + ":" + _loxSalt;
                if(gnutls_hash_fast(_digestAlgorithm, ptxt.c_str(), ptxt.size(), &hashed)<0){
                    _out.printError("GNUTLS_DIG_xxx failed");
                    return -1;
                }
                hashedPassword = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
            }
            {
                int hashedLen = gnutls_hmac_get_len(_macAlgorithm);
                unsigned char hashed[hashedLen];
                std::string ptxt = _user + ":" + hashedPassword;
                if(gnutls_hmac_fast(_macAlgorithm, _loxKey.c_str(), _loxKey.size(), ptxt.c_str(), ptxt.size(), &hashed)<0){
                    _out.printError("GNUTLS_MAC_xxx failed");
                    return -1;
                }

                hashedPassword = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
                hashedPassword = BaseLib::HelperFunctions::toLower(hashedPassword);
            }
            return 0;
        }
        catch (const std::exception &ex) {
            _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }

    uint32_t LoxoneEncryption::hashVisuPassword(std::string& hashedPassword)
    {
        try{
            {
                int hashedLen = gnutls_hash_get_len(_visuDigestAlgorithm);
                unsigned char hashed[hashedLen];
                std::string ptxt = _visuPassword + ":" + _loxVisuSalt;
                if(gnutls_hash_fast(_visuDigestAlgorithm, ptxt.c_str(), ptxt.size(), &hashed)<0){
                    _out.printError("GNUTLS_DIG_xxx failed");
                    return -1;
                }
                hashedPassword = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
            }
            {
                int hashedLen = gnutls_hmac_get_len(_visuMacAlgorithm);
                unsigned char hashed[hashedLen];
                std::string ptxt = hashedPassword;
                if(gnutls_hmac_fast(_visuMacAlgorithm, _loxVisuKey.c_str(), _loxVisuKey.size(), ptxt.c_str(), ptxt.size(), &hashed)<0){
                    _out.printError("GNUTLS_MAC_xxx failed");
                    return -1;
                }

                hashedPassword = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
                hashedPassword = BaseLib::HelperFunctions::toLower(hashedPassword);
            }
            return 0;
        }
        catch (const std::exception &ex) {
            _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }

    uint32_t LoxoneEncryption::hashToken(std::string &hashedToken)
    {
	    try
        {
            int hashedLen = gnutls_hmac_get_len(_macAlgorithm);
            unsigned char hashed[hashedLen];
            if(gnutls_hmac_fast(_macAlgorithm, _loxKey.c_str(), _loxKey.size(), _loxToken.c_str(), _loxToken.size(), &hashed)<0){
                _out.printError("GNUTLS_MAC_xxx failed");
                return -1;
            }
            hashedToken = BaseLib::HelperFunctions::getHexString(hashed, hashedLen);
            hashedToken = BaseLib::HelperFunctions::toLower(hashedToken);
            return 0;
        }
        catch (const std::exception &ex)
        {
            _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }

    uint32_t LoxoneEncryption::setToken(const std::string token)
    {
        _loxToken = token;
        _tokenValidUntil = 0;
        return 0;
    }
    uint32_t LoxoneEncryption::setToken(const BaseLib::PVariable value)
    {
        try{
            _loxToken = value->structValue->at("token")->stringValue;
            _tokenValidUntil = 0;

            _out.printInfo("Info: Save Token.");
            GD::family->setFamilySetting("token", _loxToken);

            std::stringstream ss(value->structValue->at("token")->stringValue);
            std::string part;
            uint32_t count = 0;
            while (std::getline(ss, part, '.')){
                count++;
                if(count == 1 || count == 2){
                    std::string jsonString;
                    BaseLib::Base64::decode(part, jsonString);
                    _out.printDebug("Parse Token Part " + std::to_string(count) + " : "+ jsonString);

                    _jsonDecoder.reset(new BaseLib::Rpc::JsonDecoder(GD::bl));
                    PVariable json = _jsonDecoder->decode(jsonString);

                    if(count == 1){

                    }
                    else{
                        if(json->structValue->find("iat") == json->structValue->end()) continue;
                        if(json->structValue->find("exp") == json->structValue->end()) continue;

                        _out.printDebug("This Token was generated at: "+ convertJwtTime(json->structValue->at("iat")->floatValue) + " and is valid until: " + convertJwtTime(json->structValue->at("exp")->floatValue));
                        //todo: this information is given in the JWT, but i don't see any use for it until now.
                        /*
                        uint32_t tokenRights = json->structValue->at("tokenRights")->integerValue;
                        std::string uuid = json->structValue->at("uuid")->stringValue;
                        std::string user = json->structValue->at("user")->stringValue;
                        */
                    }
                }
                else if (count == 3){
                    _out.printDebug("Parse Token Part: " + part);
                }
                else{
                    _out.printError("Decryption of JSON Web Token failed");
                    return  -1;
                }
            }
            return 0;
        }
        catch (const std::exception &ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            return -1;
        }
    }
    std::string LoxoneEncryption::convertJwtTime(uint64_t timestamp)
    {
        time_t time = timestamp;
        return ctime(&time);
    }
    uint32_t LoxoneEncryption::getToken(std::string& token, std::time_t& lifeTime)
    {
	    if(_loxToken.size() == 0) return -1;
	    if(_tokenValidUntil == 0) return -1;
	    token = _loxToken;
	    lifeTime = _tokenValidUntil;
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
        if ((gcryResult = gcry_control(GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread)) != GPG_ERR_NO_ERROR){
            _out.printCritical("Critical: Could not enable thread support for gcrypt.");
            exit(2);
        }

        if (!gcry_check_version(GCRYPT_VERSION)){
            _out.printCritical("Critical: Wrong gcrypt version.");
            exit(2);
        }
        gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
        int secureMemorySize = 16384;
        if ((gcryResult = gcry_control(GCRYCTL_INIT_SECMEM, (int)secureMemorySize, 0)) != GPG_ERR_NO_ERROR){
            _out.printCritical("Critical: Could not allocate secure memory. Error code is: " + std::to_string((int32_t)gcryResult));
            exit(2);
        }
        gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
        gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

        gnutls_global_init();
        // }}}
    }
}
