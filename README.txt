

		win-ssh-agent, win-ssh-askpass (for the cygwin openssh)


1. Introduction

	With the win-ssh-agent, we can use the ssh-agent (available in
	the cygwin openssh) in the more smart way.

	Normally, we need to start all relevant programs, which might
	need to use the ssh, as child processes of the shell
	(e.g. bash) in which you eval'ed the ssh-agent.  Because, the
	programs must be able to refer to environment variables that
	set by the ssh-agent.

	The win-ssh-agent enables all programs to refer to the
	environment variables of the ssh-agent, i.e. the
	SSH_AUTH_SOCK.  Now, we no longer need to start programs as
	child processes of the shell.


2. Install

	The win-ssh-agent.exe and the win-ssh-askpass.exe should be in
	the same directory (e.g. /usr/local/bin/) .

		For 32-bit Cygwin, you can use the attached
		win-ssh-agent.exe and the attached
		win-ssh-askpass.exe.

		For 64-bit Cygwin, you need to compile sources.
		Follow the section "6. Compile", then do the above
		instruction.

	Install the cygwin openssh by the cygwin setup.exe.
	I confirmed that the openssh-6.2p2-1 worked properly.

	The system wide PATH environment variable must have /bin of the
	cygwin (e.g. PATH=C:\cygwin\bin;... or PATH=C:\cygwin64\bin;...)


3. Run and Terminate

	To run, double-click the win-ssh-agent.exe .

	It asks the pass-phrases.  After you fulfill them, the
	win-ssh-agent stays in the notification area of the taskbar.

	To terminate it, right-click the icon in the notification
	area, then choose "Exit".


4. Behaviors

	After the win-ssh-agent started, it will make the ssh-agent
	automatically start.  And then the below environment variables
	will be set up.

		SSH_AGENT_PID	the process ID for ssh-agent.exe
		SSH_AUTH_SOCK	the socket-path for ssh-agent.exe
		SSH_ASKPASS	the path of win-ssh-askpass.exe
		DISPLAY		:0

	(The terms of set up the DISPLAY environment variable are that
	the DISPLAY have not been set up and also then --no-DISPLAY
	option was not set up at the command line of the
	win-ssh-agent.)

	Once this set up would be done, all application which will be
	started through the Explorer will be able to refer to these
	environment variables, because they are set up into the
	Explorer.

	The win-ssh-agent stays in the notification area of the
	taskbar and deletes each environment variable at the end of
	its life.

	
5. Options of the win-ssh-agent.exe

	--no-ssh-agent

		It stops to start the ssh-agent.  Without this option,
		the ssh-agent is automatically started.

	--no-DISPLAY

		It stops that the win-ssh-agent sets up automatically
		the DISPLAY environment variable.

	-i, --identity FILENAME

		In case using more than one identity file, the
		identity file will be specified by this option.  This
		option can be specified more more than once.

	-I, --default-identity -

		It shows dialogue which asks the pass-phrase at the time of
		start.  This option can be specified only once.

		The same identity files are considerd to be specified
		as the ssh-add with no-options uses.
		(i.e. ~/.ssh/id_rsa, ~/.ssh/id_dsa, ~/.ssh/id_ecdsa,
		~/.ssh/id_ed25519, ~/.ssh/identity)

		In the case of default, -I - is set up automatically.

		This option invalidates -I FILENAME.

	-I, --default-identity FILENAME

		This option works almost as same as -i.  But, it shows
		a dialogue which asks the pass-phrase at the time of
		start.  This option can be specified multiple times.

		This option invalidates -I -.

	--no-default-identity

		It stops that -I - is set in the case of default.

		This option invalidates -I -, and -I FILENAME.

	-e, --exec PROGRAM [OPTION ...]

		After the win-ssh-agent started, it executes the
		PROGRAM.  If the -I option is specified, it is
		executed after closing the pass-phrase dialogue.  In
		the case the pass-phrase dialogue was cancelled, it
		will not be executed.

	-a BIND_ADDRESS

		Bind the agent to the UNIX-domain socket bind_address.  The
		default is $TMPDIR/ssh-XXXXXXXXXX/agent.<ppid>.

	-t LIFE

		Set a default value for the maximum lifetime of identities
		added to the agent.  The lifetime may be specified in seconds
		or in a time format specified in sshd_config(5).  A lifetime
		specified for an identity with ssh-add(1) overrides this
		value.  Without this option the default maximum lifetime is
		forever.


6. Compile


	The ssh-agent.exe and ssh-add.exe must be found in the path of
	the PATH.

	If you want to re-compile, please do make.

	$ make clean && make && make install

	NOTE:
		If win-ssh-askpass.exe cannot run under Windows XP,
		please try the following commandline:

		$ make install-xp


7. Support 

	https://github.com/ganaware/win-ssh-agent


8. Copyright

	Copyright (c) 2001-2006, 2011, 2013, TAGA Nayuta <nayuta@ganaware.jp>
	All rights reserved.

	Redistribution and use in source and binary forms, with or
	without modification, are permitted provided that the
	following conditions are met:

	* Redistributions of source code must retain the above
          copyright notice, this list of conditions and the following
          disclaimer.

	* Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials
          provided with the distribution.

	* Neither the name of the GANAware nor the names of its
          contributors may be used to endorse or promote products
          derived from this software without specific prior written
          permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
	OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
	STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


9. History

	2016/04/12 1.10
		* -I FILENAME can be specified multiple times.
		* some fix

	2016/03/10 1.09
		* Fixed for Cygwin 64-bit.

	2013/08/08 1.08
		* "make install-xp" is added to build a
		  win-ssh-askpass.exe without visual-style dialogbox.
		  (Thanks to Patrick Mézard)
		* Fixed for Cygwin 64-bit.

	2011/11/02 1.07
		* This project is renamed to win-ssh-agent from win-ssh-askpass.
		* --hide-console is removed.  If you use it, it is ignored.
		* Now, win-ssh-agent.exe does not show a console window
		  when it is executed by your double-click.
		* Workaround for openssh 5.9p1-1.
		* README fix
		* some fix

	2011/10/14 1.06
		* Fix sources to build on Cygwin 1.7 .
		* Now we use visual-style dialogbox.
		* We use more Unicode API.

	2006/04/02 1.05
		* Now, win-ssh-agent set up an environment variable
                  DISPLAY to :0 instead of localhost:0.
		* Thanks to Ralf Hauser and Naoko, README written in
		  English is available.
		* -a and -t are added.
		* some bug fix for cygwin 1.5.19.

	2002/09/22 1.04
		* I separated the win-ssh-agent and the win-ssh-askpass.
		* I made a change, so that "-I-" will be specified in default.
		* --hide-console is added.
		* sample.bat was cancelled.

	2002/02/27 1.03
		* Now, -e will not be executed when you cancelled the
                  pass-phrase dialogue in the case -I and -e were
                  simultaneously specified. (Patched by Sumio TOYAMA)
		* sample.bat was added.

	2001/12/09 1.02
		* More than one identity files can be specified.
		* The pass-phrase dialogue may open at the beginning.
		* A program can be executed after the start of the
                  win-ssh-askpass.

	2001/11/10 1.01
		* The link-warning were vanished when make.

	2001/11/04 1.00
		* I made it.
