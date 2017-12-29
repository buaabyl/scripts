all:objs libusb-1.0.lib

objs:
	mkdir $@

libusb-1.0.lib:
	cl /nologo /I . /I ../libusb \
		/D _WIN32 \
		/Foobjs/ \
		/MT /c \
		../libusb/*.c  \
		../libusb/os/poll_windows.c \
		../libusb/os/threads_windows.c \
		../libusb/os/windows_usb.c 
	lib /nologo objs/*.obj \
		/out:libusb-1.0.lib


