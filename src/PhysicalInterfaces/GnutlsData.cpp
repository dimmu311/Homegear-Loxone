#include "GnutlsData.h"

GnutlsData::GnutlsData(const std::vector<uint8_t>& data)
{
    _data = data;
    _gnutlsData = std::make_shared<gnutls_datum_t>();
    _gnutlsData->data = _data.data();
    _gnutlsData->size = _data.size();
}
GnutlsData::GnutlsData(const std::string& data)
{
    _data.insert(_data.begin(), data.begin(), data.end());
    _gnutlsData = std::make_shared<gnutls_datum_t>();
    _gnutlsData->data = _data.data();
    _gnutlsData->size = _data.size();
}

gnutls_datum_t* GnutlsData::getData() {
    return _gnutlsData.get();
}
