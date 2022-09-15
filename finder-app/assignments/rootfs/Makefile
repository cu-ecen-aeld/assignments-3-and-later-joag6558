##############################################################################
#
#
# Rootfs Makefile
#
##############################################################################

.PHONY: help
help:
	@echo  'Rootfs Targets:'
	@echo  '    rootfs            - Create the root filesystem'
	@echo  '    firmware          - Create the firmware archive'
	@echo  '    distclean         - Remove the root filesystem'

.PHONY: all
all: rootfs

topdirs := \
    fs \
    fs/bin \
    fs/dev \
    fs/etc \
    fs/home \
    fs/lib \
    fs/lib64 \
    fs/mnt \
    fs/opt \
    fs/proc \
    fs/root \
    fs/sbin \
    fs/sys \
    fs/tmp \
    fs/usr \
    fs/usr/bin \
    fs/usr/lib \
    fs/var \
    fs/var/lib \
    fs/var/lock \
    fs/var/log \
    fs/var/run \
    fs/var/tmp

runtimelibs := \
    fs/lib/libc.so.6 \
    fs/lib/libstdc++.so

.PHONY: rootfs
rootfs: $(topdirs)
rootfs: $(runtimelibs)

$(runtimelibs): $(topdirs)

$(topdirs):
	mkdir -p $@

$(runtimelibs): install-runtimelibs

.PHONY: install-runtimelibs
install-runtimelibs:
	cp -a $(OUTDIR)/rootfs/lib/*.so*  $(OUTDIR)/rootfs/fs/lib/.
	cp -a $(OUTDIR)/rootfs/lib64/*.so*  $(OUTDIR)/rootfs/fs/lib64/.

.PHONY: firmware
firmware: rootfs
	$(MAKE) -f Makefile.ecea5305 firmware

.PHONY: clean mrproper distclean
clean mrproper distclean:
	-$(MAKE) -f Makefile.ecea5305 distclean
	-@if [ -d fs ] ; \
	  then \
	     rm -rf ./fs/ ; \
	  fi
