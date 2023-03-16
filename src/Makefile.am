AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++17 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear

libdir = $(localstatedir)/lib/homegear/modules
lib_LTLIBRARIES = mod_loxone.la
mod_loxone_la_SOURCES = Loxone.cpp Factory.cpp LoxonePacket.cpp GD.cpp LoxonePeer.cpp PhysicalInterfaces/Miniserver.cpp LoxoneCentral.cpp Interfaces.cpp PhysicalInterfaces/LoxoneEncryption.cpp LoxoneControl.cpp LoxoneLoxApp3.cpp PhysicalInterfaces/GnutlsData.cpp controls/createInstance.cpp controls/Pushbutton.cpp controls/Slider.cpp controls/ColorPicker.cpp helper/KelvinRgb.cpp controls/Central.cpp controls/Daytimer.cpp controls/Weather.cpp controls/NFCCodeTouch.cpp
mod_loxone_la_LDFLAGS =-module -avoid-version -shared
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/mod_loxone.la
