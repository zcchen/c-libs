BUILD_DIR   = build
INSTALL_DIR = install

pwd         = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

MAKEFLAGS  += --no-print-directory

CMAKEFLAGS  = -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR)

.PHONY:     all
.PHONY:     _all
.PHONY:     cmaking
.PHONY:     cleanall
.PHONY:     Makefile

_all: cmaking
	make -C $(BUILD_DIR) all

cmaking: $(BUILD_DIR)
	cmake $(CMAKEFLAGS) -B $< $(pwd)

%: ${BUILD_DIR}
	make -C $(BUILD_DIR) $@

$(BUILD_DIR):
	mkdir -p $@

cleanall:
	-rm -rf $(BUILD_DIR)

