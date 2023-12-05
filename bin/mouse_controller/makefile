MKDIR_P = mkdir -p
RM_RF = rm -rf

OUT_DIR = ../../arduino_builds/mouse_controller

BUILD = $(OUT_DIR)/mouse_controller.ino

HEADERS = ../arduino_files/simplelist.h \
          ../arduino_files/stringlist.h \
          ../arduino_files/linkedstring.h \
          ../arduino_files/simplequeue.h \
          ../arduino_files/communicator.h

HEADER_LIST := $(OUT_DIR)/headers

SOURCES = ../arduino_files/stringlist.cpp \
          ../arduino_files/linkedstring.cpp \
          ../arduino_files/communicator.cpp

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
	cat $(HEADER_LIST) >$@
	cat $^ >$@.tmp
	grep -v '#include.*' $@.tmp >> $@
	rm $@.tmp
	rm $(HEADER_LIST)

clean:
	$(RM_RF) $(OUT_DIR)
