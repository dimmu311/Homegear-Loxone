cmake_minimum_required(VERSION 3.8)
project(homegear_loxone)

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
        src/controls/ColorPicker.cpp
        src/controls/ColorPicker.h
		src/helper/KelvinRgb.cpp
		src/helper/KelvinRgb.h
		src/controls/Central.cpp
		src/controls/Central.h
		src/controls/Daytimer.cpp
		src/controls/Daytimer.h
		src/controls/Weather.cpp
		src/controls/Weather.h
		src/controls/NFCCodeTouch.cpp
		src/controls/NFCCodeTouch.h)

# These are the corresponding output paths
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
set (LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

add_custom_target(homegear COMMAND ../../makeAll.sh SOURCES ${SOURCE_FILES})
set(CMAKE_SHARED_LIBRARY_PREFIX "mod_")

add_library(loxone SHARED ${SOURCE_FILES})
install(TARGETS loxone DESTINATION /var/lib/homegear/modules)