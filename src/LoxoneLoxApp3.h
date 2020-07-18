#ifndef LOXONELOXAPP3_H_
#define LOXONELOXAPP3_H_

#include <homegear-base/BaseLib.h>
#include "GD.h"
#include "LoxoneControl.h"

namespace Loxone
{
class LoxoneLoxApp3
{
public:
	LoxoneLoxApp3();
	int32_t parseStructFile(BaseLib::PVariable structFile);

	std::unordered_map<std::string, std::shared_ptr<LoxoneControl>> getControls() { return _controls; }
	std::string getlastModified() { return _lastModified; }

protected:
	BaseLib::PVariable _structFile;

	std::unique_ptr<BaseLib::Rpc::JsonEncoder> _jsonEncoder;
	std::unique_ptr<BaseLib::Rpc::JsonDecoder> _jsonDecoder;

	std::unordered_map<std::string, std::string> _cats;
	std::unordered_map<std::string, std::string> _rooms;
	std::unordered_map<std::string, std::shared_ptr<LoxoneControl>> _controls;

	std::string _lastModified;

	void loadlastModified();
	void loadCats();
	void loadRooms();
	void loadControls();
};
}
#endif
