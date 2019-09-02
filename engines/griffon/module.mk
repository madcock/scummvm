MODULE := engines/griffon

MODULE_OBJS := \
	combat.o \
	config.o \
	console.o \
	cutscenes.o \
	detection.o \
	dialogs.o \
	draw.o \
	engine.o \
	griffon.o \
	resources.o \
	saveload.o \
	sound.o

MODULE_DIRS += \
	engines/griffon

# This module can be built as a plugin
ifeq ($(ENABLE_GRIFFON), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
