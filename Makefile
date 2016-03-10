MAJOR_VERSION = 1
MINOR_VERSION = 9

DISTRIB = win-ssh-agent-$(shell printf %d.%02d $(MAJOR_VERSION) $(MINOR_VERSION)).tgz

all:	win-ssh-agent win-ssh-askpass

CXXFLAGS = -Wall -O2 -g
RC	= windres
PREFIX	?= /usr/local/bin
ifdef WITHOUT_MANIFEST
RCFLAGS	= -DWITHOUT_MANIFEST
endif

%.res:	%.rc
	$(RC) $(RCFLAGS) -i $< -O coff -o $@

agent.o:	agent.cpp   agentrc.h   misc.h
askpass.o:	askpass.cpp askpassrc.h misc.h
misc.o:	misc.cpp                misc.h

win-ssh-agent:	agent.o agent.res misc.o
	$(LINK.cpp) -g -mwindows -o $@ $^

win-ssh-askpass:	askpass.o askpass.res misc.o
	$(LINK.cpp) -g -mconsole -o $@ $^

agent.res:		agent.rc   agentrc.h   agent.ico   agent.manifest
askpass.res:	askpass.rc askpassrc.h askpass.ico askpass.manifest

install:	win-ssh-agent win-ssh-askpass
	install -D -s win-ssh-agent $(PREFIX)/win-ssh-agent
	install -D -s win-ssh-askpass $(PREFIX)/win-ssh-askpass

install-xp:
	$(MAKE) $(MAKEFLAGS) WITHOUT_MANIFEST=1 clean install

distrib:	all
	strip win-ssh-agent.exe
	strip win-ssh-askpass.exe
	./check_dist_ver.rb $(MAJOR_VERSION) $(MINOR_VERSION)
	tar cvzfC $(DISTRIB)	..					\
		win-ssh-agent/.gitignore				\
		win-ssh-agent/Makefile					\
		win-ssh-agent/README-ja.txt				\
		win-ssh-agent/README.txt				\
		win-ssh-agent/agent.cpp					\
		win-ssh-agent/agent.ico					\
		win-ssh-agent/agent.manifest			\
		win-ssh-agent/agent.rc					\
		win-ssh-agent/agentrc.h					\
		win-ssh-agent/askpass.cpp				\
		win-ssh-agent/askpass.ico				\
		win-ssh-agent/askpass.manifest			\
		win-ssh-agent/askpass.rc				\
		win-ssh-agent/askpassrc.h				\
		win-ssh-agent/check_dist_ver.rb			\
		win-ssh-agent/misc.cpp					\
		win-ssh-agent/misc.h					\
		win-ssh-agent/win-ssh-agent.exe			\
		win-ssh-agent/win-ssh-askpass.exe

clean:
	-$(RM) win-ssh-askpass.exe win-ssh-agent.exe
	-$(RM) *.o *.aps *.coff *.~ *.res
