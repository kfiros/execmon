# Main Makefile
KM_DIR=kmod
UM_DIR=app
SUBDIRS = $(KM_DIR) $(UM_DIR)
SUBDIRS_CLEAN = $(addsuffix .clean, $(SUBDIRS))

.PHONY: clean $(SUBDIRS)

all: $(SUBDIRS)
clean: $(SUBDIRS_CLEAN)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBDIRS_CLEAN): %.clean:
	$(MAKE) -C $* clean

