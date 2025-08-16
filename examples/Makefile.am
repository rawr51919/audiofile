AM_CPPFLAGS = -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	adddcoffset \
	power \
	@TEST_BIN@

EXTRA_PROGRAMS = alsaplay irixread irixtestloop linuxtest osxplay

LIBAUDIOFILE = $(top_builddir)/libaudiofile/libaudiofile.la

# ALSA playback for Linux and PortAudio playback elsewhere
alsaplay_SOURCES = alsaplay.cpp
alsaplay_LDADD = $(LIBAUDIOFILE)

# If ALSA is available, remove PortAudio and add ALSA
if HAVE_ALSA
  alsaplay_LDADD := $(filter-out -lportaudio,$(alsaplay_LDADD))
  alsaplay_LDADD += -lasound
endif

# If PortAudio is available, remove ALSA and add PortAudio
if HAVE_PORTAUDIO
  alsaplay_LDADD := $(filter-out -lasound,$(alsaplay_LDADD))
  alsaplay_LDADD += -lportaudio
endif

irixread_SOURCES = irixread.c sgi.c sgi.h
irixread_LDADD = $(LIBAUDIOFILE) -laudio

irixtestloop_SOURCES = irixtestloop.c sgi.c sgi.h
irixtestloop_LDADD = $(LIBAUDIOFILE) -laudio

linuxtest_SOURCES = linuxtest.c

osxplay_SOURCES = osxplay.c
osxplay_LDFLAGS = -framework AudioUnit -framework CoreServices

power_SOURCES = power.c
power_LDADD = $(LIBAUDIOFILE) -lm

LDADD = $(LIBAUDIOFILE)

DEPENDENCIES = $(LIBAUDIOFILE)
