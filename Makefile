all:	win-ssh-agent win-ssh-askpass

CXXFLAGS = -Wall -O2
RC	= windres

%.res:	%.rc
	$(RC) -i $< -O coff -o $@

win-ssh-agent:	agent.o agent.res misc.o
	$(LINK.cpp) -e _mainCRTStartup -o $@ $^

win-ssh-askpass:	askpass.o askpass.res misc.o
	$(LINK.cpp) -mwindows -e _mainCRTStartup -o $@ $^

agent.res:	agent.rc agentrc.h agent.ico
askpass.res:	askpass.rc askpassrc.h askpass.ico

distrib:
	tar cvzf ../win-ssh-askpass-1.04.tgz	\
		Makefile 			\
		README.txt 			\
		agent.cpp 			\
		agent.ico 			\
		agent.rc 			\
		agentrc.h 			\
		askpass.cpp 			\
		askpass.ico 			\
		askpass.rc 			\
		askpassrc.h 			\
		misc.cpp 			\
		misc.h 

clean:
	-$(RM) win-ssh-askpass.exe win-ssh-agent.exe
	-$(RM) *.o *.aps *.coff *.~ *.res
