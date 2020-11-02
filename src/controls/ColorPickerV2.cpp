#include "ColorPickerV2.h"
#include "../helper/KelvinRgb.h"

namespace Loxone
{
    ColorPickerV2::ColorPickerV2(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x105)
    {
    }
    ColorPickerV2::ColorPickerV2(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x105)
    {
    }
    bool ColorPickerV2::processPacket(PLoxoneTextStatesPacket loxonePacket) {
        try
        {
            if(!LoxoneControl::processPacket(loxonePacket)) return false;
            auto value = loxonePacket->getText();

            if(value.compare(0,3, "hsv") == 0){

                value = value.substr(4,value.size()-1-4);

                std::vector<uint32_t> values;
                values.reserve(3);
                {//extract hue
                    auto end = value.find(',');
                    auto part = value.substr(0, end);
                    values[0] = std::stod(part);
                    value.erase(0,end+1);

                    _json->structValue->at("state")->structValue->operator[]("hue") = PVariable(new Variable(values[0]));
                }
                {//extract saturation
                    auto end = value.find(',');
                    auto part = value.substr(0, end);
                    values[1] = std::stod(part);
                    value.erase(0,end+1);

                    _json->structValue->at("state")->structValue->operator[]("saturation") = PVariable(new Variable(values[1]));
                }
                {//extract value
                    values[2] = std::stod(value);

                    _json->structValue->at("state")->structValue->operator[]("value") = PVariable(new Variable(values[2]));
                }

                Color::HSV hsv = Color::HSV(values[0], values[1], values[2]);
                Color::RGB rgb = hsv.toRGB();

                _json->structValue->at("state")->structValue->operator[]("red") = PVariable(new Variable(rgb.getRed()));
                _json->structValue->at("state")->structValue->operator[]("green") = PVariable(new Variable(rgb.getGreen()));
                _json->structValue->at("state")->structValue->operator[]("blue") = PVariable(new Variable(rgb.getBlue()));

                loxonePacket->setJsonString(_json);
                return true;
            }
            else if(value.compare(0,4, "temp") == 0) {
                value = value.substr(5,value.size()-1-5);

                std::vector<uint32_t> values;
                values.reserve(2);
                {//extract brightness
                    auto end = value.find(',');
                    auto part = value.substr(0, end);
                    values[0] = std::stod(part);
                    value.erase(0,end+1);
                    _json->structValue->at("state")->structValue->operator[]("value") = PVariable(new Variable(values[0]));
                }
                {//extract kelvin
                    values[1] = std::stod(value);
                    _json->structValue->at("state")->structValue->operator[]("kelvin") = PVariable(new Variable(values[1]));
                }
                auto kelvinRgb = KelvinRgb(values[1], values[0]);

                _json->structValue->at("state")->structValue->operator[]("red") = PVariable(new Variable(kelvinRgb.getRed()));
                _json->structValue->at("state")->structValue->operator[]("green") = PVariable(new Variable(kelvinRgb.getGreen()));
                _json->structValue->at("state")->structValue->operator[]("blue") = PVariable(new Variable(kelvinRgb.getBlue()));

                loxonePacket->setJsonString(_json);
                return true;
            }
        }
        catch (const std::exception& ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }

    bool ColorPickerV2::setValue(PPacket frame, PVariable parameters, std::string &command, bool &isSecured)
    {
        try
        {
            if(LoxoneControl::setValue(frame, parameters, command, isSecured)) return true;
            if(parameters->type != VariableType::tArray) return false;
            if(frame->function1 != "special") return false;

            if(frame->function2 == "setTemperature")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tInteger) return false;
                if (parameters->arrayValue->at(1)->type != VariableType::tInteger) return false;
                command += "temp(" + std::to_string(parameters->arrayValue->at(1)->integerValue) + "," + std::to_string(parameters->arrayValue->at(0)->integerValue) + ")";
                return true;
            }
            if(frame->function2 == "setHsv")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tInteger) return false;
                if (parameters->arrayValue->at(1)->type != VariableType::tInteger) return false;
                if (parameters->arrayValue->at(2)->type != VariableType::tInteger) return false;
                command += "hsv(" + std::to_string(parameters->arrayValue->at(0)->integerValue) + "," + std::to_string(parameters->arrayValue->at(1)->integerValue)+ "," + std::to_string(parameters->arrayValue->at(2)->integerValue) + ")";
                return true;
            }
            if(frame->function2 == "setRgb")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tInteger) return false;
                if (parameters->arrayValue->at(1)->type != VariableType::tInteger) return false;
                if (parameters->arrayValue->at(2)->type != VariableType::tInteger) return false;
                if (parameters->arrayValue->at(3)->type != VariableType::tInteger) return false;

                Color::NormalizedRGB rgb = Color::NormalizedRGB((uint8_t)parameters->arrayValue->at(0)->integerValue, (uint8_t)parameters->arrayValue->at(1)->integerValue, (uint8_t)parameters->arrayValue->at(2)->integerValue);
                Color::HSV hsv = rgb.toHSV();
                command += "hsv(" + std::to_string(hsv.getHue()) + "," + std::to_string(hsv.getSaturation()) + "," + std::to_string(parameters->arrayValue->at(3)->integerValue) + ")";
                return true;
            }
        }
        catch (const std::exception& ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }
}