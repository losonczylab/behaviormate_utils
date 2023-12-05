MKDIR_P = mkdir -p
RM_RF = rm -rf

OUT_DIR = ../../arduino_builds/position_controller

BUILD = $(OUT_DIR)/position_controller.ino

LICENSE = ../license_summary.txt

HEADERS = network_settings.h \
          ../arduino_files/netcomm.h

HEADER_LIST := $(OUT_DIR)/headers

SOURCES = ../arduino_files/netcomm.cpp

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
	printf "\"\n\n" >> tmp.txt
	cat tmp.txt >>$@
	cat $(HEADER_LIST) >>$@
	cat $^ >>$@.tmp
	grep -v '#include.*' $@.tmp >> $@
	rm $@.tmp
	rm $(HEADER_LIST)
	rm tmp.txt


clean:
	$(RM_RF) $(OUT_DIR)
