all:	win-ssh-agent win-ssh-askpass

CXXFLAGS = -Wall -O2
RC	= windres
PREFIX	?= /usr/local/bin

%.res:	%.rc
	$(RC) -i $< -O coff -o $@

win-ssh-agent:	agent.o agent.res misc.o
	$(LINK.cpp) -e _mainCRTStartup -o $@ $^

win-ssh-askpass:	askpass.o askpass.res misc.o
	$(LINK.cpp) -mwindows -e _mainCRTStartup -o $@ $^

agent.res:	agent.rc agentrc.h agent.ico
askpass.res:	askpass.rc askpassrc.h askpass.ico

install:	win-ssh-agent win-ssh-askpass
	install -D -s win-ssh-agent $(PREFIX)/win-ssh-agent
	install -D -s win-ssh-askpass $(PREFIX)/win-ssh-askpass

distrib:	all
	strip win-ssh-agent.exe
	strip win-ssh-askpass.exe
	tar cvzf ../win-ssh-askpass-1.05.tgz	\
		Makefile			\
		README.txt			\
		README-ja.txt			\
		agent.cpp			\
		agent.ico			\
		agent.rc			\
		agentrc.h			\
		askpass.cpp			\
		askpass.ico			\
		askpass.rc			\
		askpassrc.h			\
		misc.cpp			\
		misc.h				\
		win-ssh-agent.exe		\
		win-ssh-askpass.exe

clean:
	-$(RM) win-ssh-askpass.exe win-ssh-agent.exe
	-$(RM) *.o *.aps *.coff *.~ *.res
