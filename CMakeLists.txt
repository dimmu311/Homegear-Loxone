cmake_minimum_required(VERSION 3.8)
project(homegear_loxone_miniserver)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/PhysicalInterfaces/Miniserver.cpp
        src/PhysicalInterfaces/Miniserver.h
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
		src/LoxoneEncryption.cpp
		src/LoxoneEncryption.h
		src/LoxoneLoxApp3.cpp
		src/LoxoneLoxApp3.h
		src/LoxonePacket.cpp
        src/LoxonePacket.h
        src/LoxonePeer.cpp
        src/LoxonePeer.h
)
add_custom_target(homegear COMMAND ../../makeAll.sh SOURCES ${SOURCE_FILES})

add_library(homegear_loxone_miniserver ${SOURCE_FILES})
