all:	win-ssh-askpass

CXXFLAGS = -Wall -O2
RC	= windres

%.res:	%.rc
	$(RC) -i $< -O coff -o $@

win-ssh-askpass:	main.o resource.res
	$(LINK.cpp) -mwindows -e _mainCRTStartup -o $@ $^

resource.res:	resource.rc resource.h

distrib:
	tar cvzf ../win-ssh-askpass-1.03.tgz	\
		Makefile			\
		README.txt			\
		main.cpp			\
		resource.h			\
		resource.rc			\
		win-ssh-askpass.ico		\
		sample.bat

clean:
	-$(RM) win-ssh-askpass.exe
	-$(RM) *.o *.aps *.coff *.~ *.res
