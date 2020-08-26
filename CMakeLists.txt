cmake_minimum_required(VERSION 3.8)
project(loxone_miniserver)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
		src/PhysicalInterfaces/GnutlsData.cpp
		src/PhysicalInterfaces/GnutlsData.h
        src/PhysicalInterfaces/Miniserver.cpp
        src/PhysicalInterfaces/Miniserver.h
		src/PhysicalInterfaces/LoxoneEncryption.cpp
		src/PhysicalInterfaces/LoxoneEncryption.h
		src/PhysicalInterfaces/
		src/Factory.cpp
        src/Factory.h
        src/GD.cpp
        src/GD.h
        src/Interfaces.cpp
        src/Interfaces.h
        src/Loxone.cpp
        src/Loxone.h
        src/LoxoneCentral.cpp
        src/LoxoneCentral.h
		src/LoxoneControl.cpp
		src/LoxoneControl.h
		src/LoxoneLoxApp3.cpp
		src/LoxoneLoxApp3.h
		src/LoxonePacket.cpp
        src/LoxonePacket.h
        src/LoxonePeer.cpp
        src/LoxonePeer.h
		config.h
        src/controls/Pushbutton.cpp
		src/controls/Pushbutton.h
		src/controls/createInstance.cpp
		src/controls/createInstance.h
		src/controls/Slider.cpp
		src/controls/Slider.h
		src/controls/Dimmer.cpp
		src/controls/Dimmer.h
		src/controls/LightControllerV2.cpp
		src/controls/LightControllerV2.h
		src/controls/Alarm.cpp
		src/controls/Alarm.h
		src/controls/MediaClient.cpp
		src/controls/MediaClient.h
		src/controls/Jalousie.cpp
		src/controls/Jalousie.h src/controls/IntelligentRoomControllerV2.cpp src/controls/IntelligentRoomControllerV2.h src/controls/AalEmergency.cpp src/controls/AalEmergency.h src/controls/AalSmartAlarm.h src/controls/AalSmartAlarm.cpp)

add_custom_target(homegear COMMAND ../../makeAll.sh SOURCES ${SOURCE_FILES})
set(CMAKE_SHARED_LIBRARY_PREFIX "mod_")

add_library(loxone_miniserver SHARED ${SOURCE_FILES})
install(TARGETS loxone_miniserver DESTINATION /var/lib/homegear/modules)