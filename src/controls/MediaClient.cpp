#include "MediaClient.h"

namespace Loxone
{
    MediaClient::MediaClient(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x201){}
    MediaClient::MediaClient(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x201){}
    uint32_t MediaClient::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        {
            return 0;
        }
    }
}