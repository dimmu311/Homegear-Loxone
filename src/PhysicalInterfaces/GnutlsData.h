#ifndef GNUTLSDATA_H
#define GNUTLSDATA_H


#include <vector>
#include <memory>
#include <string>

#include <gnutls/gnutls.h>

class GnutlsData
{
private:
    std::vector<uint8_t> _data;
    std::shared_ptr<gnutls_datum_t> _gnutlsData;
public:
    explicit GnutlsData(const std::vector<uint8_t>& data);
    explicit GnutlsData(const std::string& data);

    gnutls_datum_t* getData();
};


#endif //GNUTLSDATA_H
