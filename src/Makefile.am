AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++14 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear

libdir = $(localstatedir)/lib/homegear/modules
lib_LTLIBRARIES = mod_loxone_miniserver.la
mod_loxone_miniserver_la_SOURCES = Loxone.cpp Factory.cpp LoxonePacket.cpp GD.cpp LoxonePeer.cpp PhysicalInterfaces/Miniserver.cpp LoxoneCentral.cpp Interfaces.cpp PhysicalInterfaces/LoxoneEncryption.cpp LoxoneControl.cpp LoxoneLoxApp3.cpp PhysicalInterfaces/GnutlsData.cpp controls/createInstance.cpp controls/Alarm.cpp controls/Dimmer.cpp controls/LightControllerV2.cpp controls/MediaClient.cpp controls/Pushbutton.cpp controls/Slider.cpp
mod_loxone_miniserver_la_LDFLAGS =-module -avoid-version -shared
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/mod_loxone_miniserver.la
