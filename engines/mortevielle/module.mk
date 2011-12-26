MODULE := engines/mortevielle
 
MODULE_OBJS := \
	actions.o \
	alert.o \
	boite.o \
	disk.o \
	droite.o \
	keyboard.o \
	level15.o \
	menu.o \
	mor.o \
	mor2.o \
	mort.o \
	mouse.o \
	outtext.o \
	ovd1.o \
	parole.o \
	parole2.o \
	prog.o \
	ques.o \
	sprint.o \
	taffich.o \
	var_mor.o
 
# This module can be built as a plugin
ifeq ($(ENABLE_MORTEVIELLE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif
 
# Include common rules 
include $(srcdir)/rules.mk
