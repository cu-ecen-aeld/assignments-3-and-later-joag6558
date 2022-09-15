##############################################################################
#
# Busybox Makefile
#
##############################################################################

BUSYBOX_DEFCONFIG = busybox-1.33.1-arm64_defconfig


CROSS_COMPILER_PREFIX = aarch64-none-linux-gnu

CROSS_COMPILE = $(CROSS_COMPILER_PREFIX)-
export CROSS_COMPILE

CC=$(CROSS_COMPILER_PREFIX)-gcc
CXX=$(CROSS_COMPILER_PREFIX)-g++
AR=$(CROSS_COMPILER_PREFIX)-ar
AS=$(CROSS_COMPILER_PREFIX)-as
LD=$(CROSS_COMPILER_PREFIX)-ld
NM=$(CROSS_COMPILER_PREFIX)-nm
OBJDUMP=$(CROSS_COMPILER_PREFIX)-objdump
RANLIB=$(CROSS_COMPILER_PREFIX)-ranlib
STRIP=$(CROSS_COMPILER_PREFIX)-strip

CC := $(CC) 

export CC CXX AR AS LD NM OBJDUMP RANLIB STRIP LDFLAGS


.PHONY: help
help:
	@echo 'Targets are:'
	@echo '    clean'
	@echo '    distclean'
	@echo '    all'
	@echo '    config'
	@echo '    install'

.PHONY: all
all: dobuild

.PHONY: dobuild
dobuild: build/.config
	$(MAKE) -C build -j4  ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE)

build/.config: $(BUSYBOX_DEFCONFIG)
	mkdir -p ./build/
	cp $(BUSYBOX_DEFCONFIG) build/.config
	$(MAKE) -C $(OUTDIR)/busybox ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) O=../sysapps/build oldconfig

.PHONY: config
config: build/.config

.PHONY: install
install: #dobuild
	$(MAKE) -C ./build CONFIG_PREFIX=$(OUTDIR)/rootfs/fs  ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) install

.PHONY: clean
clean:
	-@if [ -f build/Makefile ] ; \
	  then \
	    $(MAKE) -C build clean ; \
	  fi

.PHONY: mrproper distclean
mrproper distclean:
	-@if [ -d build ] ; \
	  then \
	     rm -rf build ; \
	  fi
	-@if [ -d src ] ; \
	  then \
	     rm -rf src ; \
	  fi

