SUFFIXES = .bmp

ICONS = \
	colon_bmp.bmp	\
	dash_bmp.bmp	\
	eight_bmp.bmp	\
	eject_bmp.bmp	\
	five_bmp.bmp	\
	forward_bmp.bmp	\
	four_bmp.bmp	\
	intro_bmp.bmp	\
	introd_bmp.bmp	\
	mute_bmp.bmp	\
	next_bmp.bmp	\
	nine_bmp.bmp	\
	nomute_bmp.bmp	\
	one_bmp.bmp	\
	pause_bmp.bmp	\
	play_bmp.bmp	\
	prefs_bmp.bmp	\
	prev_bmp.bmp	\
	rand_bmp.bmp	\
	randd_bmp.bmp	\
	repeatd_bmp.bmp	\
	repeatn_bmp.bmp	\
	repeatt_bmp.bmp	\
	reverse_bmp.bmp	\
	seven_bmp.bmp	\
	six_bmp.bmp	\
	stop_bmp.bmp	\
	three_bmp.bmp	\
	two_bmp.bmp	\
	zero_bmp.bmp

all: ../icons.h ../icons.cpp

../icons.h: $(ICONS)
	$(RESWRAP) -i -o ../icons.h $^

../icons.cpp: $(ICONS)
	$(RESWRAP) -e -0 ../icons.cpp $^

CLEANFILES = ../icons.h ../icons.cpp

EXTRA_DIST = $(ICONS)

