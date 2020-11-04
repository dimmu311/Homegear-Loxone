#ifndef HOMEGEAR_LOXONE_MINISERVER_COLORPICKERV2_H
#define HOMEGEAR_LOXONE_MINISERVER_COLORPICKERV2_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class ColorPickerV2 : public LoxoneControl
    {
        /*
         * 15d454ca-01d7-0977-ffff14b9c0f9d460/AI2":{
         *  "name":"RGB",
         *  "type":"ColorPickerV2",
         *  "uuidAction":"15d454ca-01d7-0977-ffff14b9c0f9d460/AI2",
         *  "defaultRating":0,
         *  "isFavorite":false,
         *  "isSecured":false,
         *  "details":{"
         *      pickerType":"Rgb"
         *   },
         *   "states":{
         *      "color":"15d454ca-01d7-0961-ffff6a40bac9d8ea",
         *      "sequence":"15cd799d-00ba-2f3e-ffff3ce43fa9aec5",
         *      "sequenceColorIdx":"15cd799d-00ba-2f3f-ffff3ce43fa9aec5"
         *   }
         * }
         * LoxoneControl::LoxoneValueStatesPacket at sequenceColorIdx of peer 17 and value is -1.000000
         * LoxoneControl::LoxoneTextStatesPacket at color of peer 17 and value is hsv(0,0,0)
         * LoxoneControl::LoxoneTextStatesPacket at sequence of peer 17 and value is
         */
    public:
        ColorPickerV2(PVariable control, std::string room, std::string cat);
        ColorPickerV2(std::shared_ptr<BaseLib::Database::DataTable> rows);

        bool processPacket(PLoxoneTextStatesPacket loxonePacket);
        //bool processPacket(PLoxoneValueStatesPacket loxonePacket);

        bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
    protected:
    };
}
#endif //HOMEGEAR_LOXONE_MINISERVER_COLORPICKERV2_H
