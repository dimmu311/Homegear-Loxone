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
        src/controls/ColorPickerV2.cpp
		src/controls/ColorPickerV2.h
		src/helper/KelvinRgb.cpp
		src/helper/KelvinRgb.h
		src/controls/Central.cpp
		src/controls/Central.h)

add_custom_target(homegear COMMAND ../../makeAll.sh SOURCES ${SOURCE_FILES})
set(CMAKE_SHARED_LIBRARY_PREFIX "mod_")

add_library(loxone SHARED ${SOURCE_FILES})
install(TARGETS loxone DESTINATION /var/lib/homegear/modules)