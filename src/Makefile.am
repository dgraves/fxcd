INCLUDES = -I$(top_srcdir)/include

SUFFIXES = .bmp

bin_PROGRAMS = fxcd

ICONS = \
	$(top_srcdir)/icons/colon_bmp.bmp	\
	$(top_srcdir)/icons/dash_bmp.bmp	\
	$(top_srcdir)/icons/eight_bmp.bmp	\
	$(top_srcdir)/icons/eject_bmp.bmp	\
	$(top_srcdir)/icons/five_bmp.bmp	\
	$(top_srcdir)/icons/forward_bmp.bmp	\
	$(top_srcdir)/icons/four_bmp.bmp	\
	$(top_srcdir)/icons/intro_bmp.bmp	\
	$(top_srcdir)/icons/introd_bmp.bmp	\
	$(top_srcdir)/icons/mute_bmp.bmp	\
	$(top_srcdir)/icons/next_bmp.bmp	\
	$(top_srcdir)/icons/nine_bmp.bmp	\
	$(top_srcdir)/icons/nomute_bmp.bmp	\
	$(top_srcdir)/icons/one_bmp.bmp	\
	$(top_srcdir)/icons/pause_bmp.bmp	\
	$(top_srcdir)/icons/play_bmp.bmp	\
	$(top_srcdir)/icons/prefs_bmp.bmp	\
	$(top_srcdir)/icons/$(top_srcdir)/icons/prev_bmp.bmp	\
	$(top_srcdir)/icons/rand_bmp.bmp	\
	$(top_srcdir)/icons/randd_bmp.bmp	\
	$(top_srcdir)/icons/repeatd_bmp.bmp	\
	$(top_srcdir)/icons/repeatn_bmp.bmp	\
	$(top_srcdir)/icons/repeatt_bmp.bmp	\
	$(top_srcdir)/icons/reverse_bmp.bmp	\
	$(top_srcdir)/icons/seven_bmp.bmp	\
	$(top_srcdir)/icons/six_bmp.bmp	\
	$(top_srcdir)/icons/stop_bmp.bmp	\
	$(top_srcdir)/icons/three_bmp.bmp	\
	$(top_srcdir)/icons/two_bmp.bmp	\
	$(top_srcdir)/icons/zero_bmp.bmp

fxcd_SOURCES = \
		icons.cpp	        \
		CDBMPIcon.cpp	        \
		CDCanvas.cpp	        \
		CDListBox.cpp	\
		CDPlayer.cpp	        \
		CDPreferences.cpp	\
		CDSeekButton.cpp	\
		CDWindow.cpp	        \
		cd.cpp

#fxcd_SOURCES = \
#		icons.cpp	        \
#		CDBMPIcon.cpp	        \
#		CDChoiceDialog.cpp	\
#		CDInfo.cpp	        \
#		CDPipe.cpp              \
#		CDPlayer.cpp	        \
#		CDPreferences.cpp	\
#		CDSeekButton.cpp	\
#		CDServerDialog.cpp	\
#		CDWindow.cpp	        \
#		cd.cpp

CDBMPIcon.cpp: $(top_srcdir)/include/icons.h icons.cpp

$(top_srcdir)/include/icons.h: $(ICONS)
	$(RESWRAP) -i -o $(top_srcdir)/include/icons.h $^

icons.cpp: $(ICONS)
	$(RESWRAP) -e -o icons.cpp $^

CLEANFILES = $(top_srcdir)/include/icons.h icons.cpp

EXTRA_DIST = $(ICONS) \
             $(top_srcdir)/include/icons.h
