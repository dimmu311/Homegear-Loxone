#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "../LoxoneControl.h"

namespace Loxone
{
class Pushbutton : public LoxoneControl
    {
    //TODO, maybe decode the response after changing a value, see this example
    /*
    {"LL": { "control": "jdev/sys/enc/chXRdF2D5fda2WcpW2mSOy8kgcKqO7FyEpQgEEeh8QFLQmpHehPhHJZZhFu2HcvDgChZkjk0OlDyMg/WINNJVPNDp0FNrydYilIowb21UDD99ryJVDuBkgbaVjzZCsfm", "value": "1", "Code": "200"}}
    responseCode 200 control jdev/sys/enc/chXRdF2D5fda2WcpW2mSOy8kgcKqO7FyEpQgEEeh8QFLQmpHehPhHJZZhFu2HcvDgChZkjk0OlDyMg/WINNJVPNDp0FNrydYilIowb21UDD99ryJVDuBkgbaVjzZCsfm
    decrypted Command = jdev/sps/io/14003f84-007d-36b6-ffff14b9c0f9d460/off
     */
    public:
        Pushbutton(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat);
        Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
        std::string _defaultIcon;
    };
}

#endif //PUSHBUTTON_H
