#ifndef LOXONE_MINISERVER_JALOUSIE_H
#define LOXONE_MINISERVER_JALOUSIE_H

#include "../LoxoneControl.h"
namespace Loxone
{
    enum class animation
    {
        Blinds = 0,
        Shutters = 1,
        CurtainBothSides = 2,
        NotSupported = 3,
        CurtainLeft = 4,
        CurtainRight = 5,
        Awning = 6,
    };
    class Jalousie : public LoxoneControl
    {
        /*
        "15da980c-018b-755c-ffff14b9c0f9d460":{
            "name":"Automatikjalousie",
            "type":"Jalousie",
            "uuidAction":"15da980c-018b-755c-ffff14b9c0f9d460",
            "room":"140032b9-0146-2eb7-ffff14b9c0f9d460",
            "cat":"140032b9-00f0-2d72-ffff14b9c0f9d460",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":false,
            "details":{
                "isAutomatic":true,
                "animation":0
            },
            "states":{
                "infoText":"15da980c-018b-752b-ffff567213859ac9",
                "up":"15da980c-018b-7554-ffff567213859ac9",
                "down":"15da980c-018b-7555-ffff567213859ac9",
                "position":"15da980c-018b-7556-ffff567213859ac9",
                "shadePosition":"15da980c-018b-7557-ffff567213859ac9",
                "safetyActive":"15da980c-018b-7559-ffff567213859ac9",
                "autoAllowed":"15da980c-018b-7535-ffff567213859ac9",
                "autoActive":"15da980c-018b-7558-ffff567213859ac9",
                "locked":"15da980c-018b-755a-ffff567213859ac9",
                "autoInfoText":"15da980c-018b-752c-ffff567213859ac9",
                "autoState":"15da980c-018b-752d-ffff567213859ac9"
            }
        }
         */
    public:
        Jalousie(PVariable control, std::string room, std::string cat);
        Jalousie(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
        uint32_t getExtraData(std::list<extraData> &extraData);
    protected:
        bool _isFavorite;

        bool _isAutomatic;
        animation _animation;
    };
}

#endif //LOXONE_MINISERVER_JALOUSIE_H
