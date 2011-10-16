all:	win-ssh-agent win-ssh-askpass

CXXFLAGS = -Wall -O2
RC	= windres
PREFIX	?= /usr/local/bin

%.res:	%.rc
	$(RC) -i $< -O coff -o $@

agent.o:	agent.cpp   agentrc.h   misc.h
askpass.o:	askpass.cpp askpassrc.h misc.h
misc.o:	misc.cpp                misc.h

win-ssh-agent:	agent.o agent.res misc.o
	$(LINK.cpp) -mwindows -e _mainCRTStartup -o $@ $^

win-ssh-askpass:	askpass.o askpass.res misc.o
	$(LINK.cpp) -mconsole -e _mainCRTStartup -o $@ $^

agent.res:		agent.rc   agentrc.h   agent.ico   agent.manifest
askpass.res:	askpass.rc askpassrc.h askpass.ico askpass.manifest

install:	win-ssh-agent win-ssh-askpass
	install -D -s win-ssh-agent $(PREFIX)/win-ssh-agent
	install -D -s win-ssh-askpass $(PREFIX)/win-ssh-askpass

distrib:	all
	strip win-ssh-agent.exe
	strip win-ssh-askpass.exe
	tar cvzfC win-ssh-askpass-1.06.tgz	..	\
		win-ssh-askpass/.gitignore			\
		win-ssh-askpass/Makefile			\
		win-ssh-askpass/README-ja.txt		\
		win-ssh-askpass/README.txt			\
		win-ssh-askpass/agent.cpp			\
		win-ssh-askpass/agent.ico			\
		win-ssh-askpass/agent.manifest		\
		win-ssh-askpass/agent.rc			\
		win-ssh-askpass/agentrc.h			\
		win-ssh-askpass/askpass.cpp		\
		win-ssh-askpass/askpass.ico		\
		win-ssh-askpass/askpass.manifest	\
		win-ssh-askpass/askpass.rc			\
		win-ssh-askpass/askpassrc.h		\
		win-ssh-askpass/misc.cpp			\
		win-ssh-askpass/misc.h				\
		win-ssh-askpass/win-ssh-agent.exe	\
		win-ssh-askpass/win-ssh-askpass.exe

clean:
	-$(RM) win-ssh-askpass.exe win-ssh-agent.exe
	-$(RM) *.o *.aps *.coff *.~ *.res
