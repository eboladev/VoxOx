#Customisable stuff here
LINUX32_COMPILER = i686-pc-linux-gnu-gcc
LINUX64_COMPILER = x86_64-pc-linux-gnu-gcc
WIN32_COMPILER = /usr/bin/i586-mingw32-gcc
WIN32_WINDRES = i586-mingw32-windres
#LINUX_ARM_COMPILER = arm-pc-linux-gnu-gcc
LINUX_ARM_COMPILER = arm-none-linux-gnueabi-gcc
LINUX_PPC_COMPILER = powerpc-unknown-linux-gnu-gcc
FREEBSD60_COMPILER = i686-pc-freebsd6.0-gcc
MACPORT_COMPILER = i686-apple-darwin9-gcc-4.0.1

LIBPURPLE_CFLAGS = -I/usr/include/libpurple -I/usr/local/include/libpurple -DPURPLE_PLUGINS -DENABLE_NLS -DHAVE_ZLIB
GLIB_CFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include -I/usr/include/json-glib-1.0 -ljson-glib-1.0
WIN32_DEV_DIR = /root/pidgin/win32-dev
WIN32_PIDGIN_DIR = /root/pidgin/pidgin-2.3.0_win32
WIN32_CFLAGS = -I${WIN32_DEV_DIR}/gtk_2_0/include/glib-2.0 -I${WIN32_PIDGIN_DIR}/libpurple/win32 -I${WIN32_DEV_DIR}/gtk_2_0/include -I${WIN32_DEV_DIR}/gtk_2_0/include/glib-2.0 -I${WIN32_DEV_DIR}/gtk_2_0/lib/glib-2.0/include -I/usr/include/json-glib-1.0 -Wno-format
WIN32_LIBS = -L${WIN32_DEV_DIR}/gtk_2_0/lib -L${WIN32_PIDGIN_DIR}/libpurple -lglib-2.0 -lgobject-2.0 -lintl -lpurple -lws2_32 -L. -ljson-glib-1.0 -lzlib1
MACPORT_CFLAGS = -I/opt/local/include/libpurple -DPURPLE_PLUGINS -DENABLE_NLS -DHAVE_ZLIB -I/opt/local/include/glib-2.0 -I/opt/local/lib/glib-2.0/include -I/opt/local/include -I/opt/local/include/json-glib-1.0 -arch i386 -arch ppc -dynamiclib -L/opt/local/lib -ljson-glib-1.0 -lpurple -lglib-2.0 -lgobject-2.0 -lintl -lz -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4

DEB_PACKAGE_DIR = ./debdir

FACEBOOK_SOURCES = \
	libfacebook.h \
	libfacebook.c \
	fb_blist.h \
	fb_blist.c \
	fb_connection.h \
	fb_connection.c \
	fb_conversation.h \
	fb_conversation.c \
	fb_info.h \
	fb_info.c \
	fb_managefriends.h \
	fb_managefriends.c \
	fb_messages.h \
	fb_messages.c \
	fb_notifications.h \
	fb_notifications.c \
	fb_search.h \
	fb_search.c \
	fb_util.h \
	fb_util.c \
	fb_friendlist.h \
	fb_friendlist.c
	fb_json.h \
	fb_json.c

#Standard stuff here
.PHONY:	all clean install sourcepackage

all:	libfacebook.so libfacebook.dll libfacebook64.so libfacebookarm.so libfacebookppc.so installers sourcepackage

install:
	cp libfacebook.so /usr/lib/purple-2/
	cp libfacebook64.so /usr/lib64/purple-2/
	cp libfacebookarm.so /usr/lib/pidgin/
	cp libfacebookppc.so /usr/lib/purple-2/
	cp facebook16.png /usr/share/pixmaps/pidgin/protocols/16/facebook.png
	cp facebook22.png /usr/share/pixmaps/pidgin/protocols/22/facebook.png
	cp facebook48.png /usr/share/pixmaps/pidgin/protocols/48/facebook.png

installers:	pidgin-facebookchat.exe pidgin-facebookchat.deb pidgin-facebookchat.tar.bz2

clean:
	rm -f libfacebook.so libfacebook.dll libfacebook64.so libfacebookarm.so libfacebookppc.so pidgin-facebookchat.exe pidgin-facebookchat.deb pidgin-facebookchat.tar.bz2 pidgin-facebookchat-source.tar.bz2
	rm -rf pidgin-facebookchat

libfacebook.so:	${FACEBOOK_SOURCES}
	${LINUX32_COMPILER} ${LIBPURPLE_CFLAGS} -Wall ${GLIB_CFLAGS} -I. -g -O2 -pipe ${FACEBOOK_SOURCES} -o libfacebook.so -shared -fPIC -DPIC

libfacebookarm.so:	${FACEBOOK_SOURCES}
	${LINUX_ARM_COMPILER} ${LIBPURPLE_CFLAGS} -Wall ${GLIB_CFLAGS} -I. -g -O2 -pipe ${FACEBOOK_SOURCES} -o libfacebookarm.so -shared -fPIC -DPIC

libfacebook64.so:	${FACEBOOK_SOURCES}
	${LINUX64_COMPILER} ${LIBPURPLE_CFLAGS} -Wall ${GLIB_CFLAGS} -I. -g -m64 -O2 -pipe ${FACEBOOK_SOURCES} -o libfacebook64.so -shared -fPIC -DPIC

libfacebookppc.so:	${FACEBOOK_SOURCES}
	${LINUX_PPC_COMPILER} ${LIBPURPLE_CFLAGS} -Wall ${GLIB_CFLAGS} -I. -g -O2 -pipe ${FACEBOOK_SOURCES} -o libfacebookppc.so -shared -fPIC -DPIC

libfacebookmacport.so: ${FACEBOOK_SOURCES}
	${MACPORT_COMPILER} ${MACPORT_CFLAGS} -Wall -I. -g -O2 -pipe ${FACEBOOK_SOURCES} -o libfacebookmacport.so -shared

pidgin-facebookchat.res:	pidgin-facebookchat.rc
	${WIN32_WINDRES} $< -O coff -o $@

libfacebook.dll:	${FACEBOOK_SOURCES} pidgin-facebookchat.res
	${WIN32_COMPILER} ${LIBPURPLE_CFLAGS} -Wall -I. -g -O2 -pipe ${FACEBOOK_SOURCES} pidgin-facebookchat.res -o $@ -shared -mno-cygwin ${WIN32_CFLAGS} ${WIN32_LIBS} -Wl,--strip-all
	upx libfacebook.dll

libfacebook-debug.dll:	${FACEBOOK_SOURCES} pidgin-facebookchat.res
	${WIN32_COMPILER} ${LIBPURPLE_CFLAGS} -Wall -I. -g -O2 -pipe ${FACEBOOK_SOURCES} pidgin-facebookchat.res -o $@ -shared -mno-cygwin ${WIN32_CFLAGS} ${WIN32_LIBS}

libfacebookbsd60.so:	${FACEBOOK_SOURCES}
	${FREEBSD60_COMPILER} ${LIBPURPLE_CFLAGS} -Wall ${GLIB_CFLAGS} -I. -g -O2 -pipe ${FACEBOOK_SOURCES} -o libfacebook.so -shared -fPIC -DPIC


pidgin-facebookchat.exe:	libfacebook.dll
	echo "Dont forget to update version number"
	makensis facebook.nsi > /dev/null
	
pidgin-facebookchat.deb:	libfacebook.so libfacebookarm.so libfacebook64.so libfacebookppc.so
	echo "Dont forget to update version number"
	cp libfacebook.so ${DEB_PACKAGE_DIR}/usr/lib/purple-2/
	cp libfacebookppc.so ${DEB_PACKAGE_DIR}/usr/lib/purple-2/
	cp libfacebook64.so ${DEB_PACKAGE_DIR}/usr/lib64/purple-2/
	cp libfacebookarm.so ${DEB_PACKAGE_DIR}/usr/lib/pidgin/
	cp facebook16.png ${DEB_PACKAGE_DIR}/usr/share/pixmaps/pidgin/protocols/16/facebook.png
	cp facebook22.png ${DEB_PACKAGE_DIR}/usr/share/pixmaps/pidgin/protocols/22/facebook.png
	cp facebook48.png ${DEB_PACKAGE_DIR}/usr/share/pixmaps/pidgin/protocols/48/facebook.png
	chown -R root:root ${DEB_PACKAGE_DIR}
	chmod -R 755 ${DEB_PACKAGE_DIR}
	dpkg-deb --build ${DEB_PACKAGE_DIR} $@ > /dev/null

pidgin-facebookchat.tar.bz2:	pidgin-facebookchat.deb
	tar --bzip2 --directory ${DEB_PACKAGE_DIR} -cf $@ usr/

sourcepackage:	${FACEBOOK_SOURCES} Makefile facebook16.png facebook22.png facebook48.png COPYING facebook.nsi
	tar -cf tmp.tar $^
	mkdir pidgin-facebookchat
	mv tmp.tar pidgin-facebookchat
	tar xvf pidgin-facebookchat/tmp.tar -C pidgin-facebookchat
	rm pidgin-facebookchat/tmp.tar
	tar --bzip2 -cf pidgin-facebookchat-source.tar.bz2 pidgin-facebookchat
	rm -rf pidgin-facebookchat
