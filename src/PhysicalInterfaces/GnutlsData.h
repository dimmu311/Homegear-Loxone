#ifndef HOMEGEAR_LOXONE_MINISERVER_GNUTLSDATA_H
#define HOMEGEAR_LOXONE_MINISERVER_GNUTLSDATA_H


#include <vector>
#include <memory>
#include <cstdint>

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


#endif //HOMEGEAR_LOXONE_MINISERVER_GNUTLSDATA_H
