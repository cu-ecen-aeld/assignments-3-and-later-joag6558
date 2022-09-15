##############################################################################
#
#
# Linux Kernel Makefile
#
##############################################################################

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

ECEA5305_DEFCONFIG = linux-5.1.10-ecea5305_defconfig

.PHONY: help
help:
	@echo  'Linux Kernel Targets:'
	@echo  '    menuconfig        - Configures Kernel for arm64'
	@echo  '    all               - Build Kernel for arm64'
	@echo  '    install           - Install kernel for arm64'

.PHONY: all
all: dobuild

.PHONY: dobuild
dobuild: build/.config
	$(MAKE) -C build -j4 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) all
	$(MAKE) -C build -j4 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) dtbs
	$(MAKE) -C build -j4 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) modules

build/.config: 
	mkdir -p ./build/
	cp $(ECEA5305_DEFCONFIG) build/.config
	$(MAKE) -C $(OUTDIR)/linux-stable ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) O=../kernel/build oldconfig	

.PHONY: install
install: #dobuild
	cp -a $(OUTDIR)/kernel/build/arch/arm64/boot/Image $(OUTDIR)
	cp -a $(OUTDIR)/kernel/build/arch/arm64/boot/Image $(OUTDIR)/linux-stable/arch/arm64/boot/Image

	$(MAKE) -C build $(BUILD_VERBOSE_OPTIONS) ARCH=arm64 \
	    INSTALL_MOD_PATH=$(OUTDIR)/rootfs/fs modules_install

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
