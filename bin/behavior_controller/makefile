MKDIR_P = mkdir -p
RM_RF = rm -rf

OUT_DIR = ../../arduino_builds/behavior_controller

BUILD = $(OUT_DIR)/behavior_controller.ino

LICENSE = ../license_summary.txt

HEADERS = network_settings.h \
          ../arduino_files/op_new.h \
          ../arduino_files/staticsimplelist.h \
          ../arduino_files/simplearrayqueue.h \
          ../arduino_files/netcomm_due.h \
          ../arduino_files/communicator.h \
          ../arduino_files/mprvalues.h \
          ../arduino_files/valve.h \
          ../arduino_files/dueTone.h \
          ../arduino_files/tone.h \
          ../arduino_files/inverted_valve.h \
          ../arduino_files/valvemanager.h \
          ../arduino_files/sensor.h \
          ../arduino_files/rfidreader.h \
          ../arduino_files/piezoport.h \
          ../arduino_files/lickport.h \
          ../arduino_files/irport.h \
          ../arduino_files/cap1188.h \
          ../arduino_files/cap1188_mapping.h \
          ../arduino_files/sensormanager.h \
          ../arduino_files/context.h \
          ../arduino_files/constantcontext.h \
          ../arduino_files/operantcontext.h \
          ../arduino_files/pulsedcontext.h \
          ../arduino_files/contextmanager.h \

HEADER_LIST := $(OUT_DIR)/headers

SOURCES = ../arduino_files/op_new.cpp \
          ../arduino_files/piezoport.cpp \
          ../arduino_files/lickport.cpp \
          ../arduino_files/irport.cpp\
          ../arduino_files/cap1188.cpp \
          ../arduino_files/cap1188_mapping.cpp \
          ../arduino_files/rfidreader.cpp \
          ../arduino_files/netcomm_due.cpp \
          ../arduino_files/communicator.cpp \
          ../arduino_files/dueTone.cpp \
          ../arduino_files/valve.cpp \
          ../arduino_files/tone.cpp \
          ../arduino_files/inverted_valve.cpp \
          ../arduino_files/valvemanager.cpp \
          ../arduino_files/context.cpp \
          ../arduino_files/operantcontext.cpp \
          ../arduino_files/pulsedcontext.cpp \
          ../arduino_files/contextmanager.cpp \
          ../arduino_files/sensor.cpp \
          ../arduino_files/sensormanager.cpp

MAIN = *.ino

space = $(noop) $(noop)

all: $(OUT_DIR) $(HEADER_LIST) $(BUILD)

$(OUT_DIR):
	$(MKDIR_P) $(OUT_DIR)

$(HEADER_LIST): $(MAIN) $(HEADERS) $(SOURCE)
	grep -h '#include <.*>' $^ > $@
	sed -n 'G; s/\n/&&/; /^\([ -~]*\n\).*\n\1/d; s/\n//; h; P' $@ >$@.tmp
	mv $@.tmp $@

$(BUILD): $(HEADERS) $(SOURCES) $(MAIN)
	cat $(LICENSE) >$@
	printf "\n\n" >>$@
	cat $(LICENSE)
	{ echo "#define GIT_REVISION \""; git rev-parse HEAD | cat; } | tr -d "\n" >tmp.txt
	printf "\"\n" >>tmp.txt
	cat tmp.txt >>$@
	{ echo "#define VERSION \""; git describe --tag | cat; } | tr -d "\n" >tmp.txt
	printf "\"\n\n" >>tmp.txt
	cat tmp.txt >>$@
	cat $(HEADER_LIST) >>$@
	cat $^ >>$@.tmp
	grep -v '#include.*' $@.tmp >> $@
	rm $@.tmp
	rm $(HEADER_LIST)
	rm tmp.txt

clean:
	$(RM_RF) $(OUT_DIR)
