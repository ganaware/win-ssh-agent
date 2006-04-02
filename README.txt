

		    win-ssh-agent, win-ssh-askpass


1. Introduction

	When using the ssh-agent with X, you can start the ssh-agent in
	the .xsession or the .xinitrc.  Then all your programs are able
	to refer to environment variables the SSH_AGENT_PID and the
	SSH_AUTH_SOCK.  It must be very useful.

	In the cygwin, however, it may be difficult to set up the
	environment variables that can be referred to by all your
	programs.  In such case, you need to start all relevant
	programs, which might need to use the ssh, as child processes
	of the shell (e.g. bash) in which you eval'ed the ssh-agent.

	The win-ssh-agent enables all programs to refer to the
	environment variables of the ssh-agent, i.e. the SSH_AGENT_PID
	and the SSH_AUTH_SOCK.

	The win-ssh-askpass can fulfill the same function of the
	ssh-askpass for X.

	
2. Behaviors

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

	The win-ssh-agent stays in the task-tray and deletes each
	environment variable at the end of its life.

	
	If the DISPLAY and the SSH_ASKPASS are correctly set up, the
	ssh-add will start the win-ssh-askpass.  In this case the
	pass-phrase will be required like the case of ssh-askpass for
	X.


	Because the win-ssh-askpass uses the tool of the openssh
	inside, the PATH environment variable must have /bin of the
	cygwin.  You need to use the openssh 3.4p1-5 or the later.  It
	is not known whether it can work properly with the previous
	version.

	
3. Options of the win-ssh-agent

	--no-ssh-agent

		It stops to start the ssh-agent.  Without this option,
		the ssh-agent is automatically started.

	--no-DISPLAY

		It stops that the win-ssh-agent sets up automatically
		the DISPLAY environment variable.  (see section 2)

	--hide-console

		It hides the console-window belonging to
		win-ssh-agent.  It will appear again when the
		win-ssh-agent was closed.

	-i, --identity FILENAME

		In case using more than one identity file, the
		identity file will be specified by this option.  This
		option can be specified more more than once.

	-I, --default-identity FILENAME

		This option works almost as same as -i.  But, it shows
		a dialogue which asks the pass-phrase at the time of
		start.  This option can be specified only once.

		In the case of default, -I- is set up automatically.
		If - is specified as a FILENAME, the same identity
		files are considerd to be specified as the ssh-add
		with no-options uses.  (i.e. ~/.ssh/id_rsa,
		~/.ssh/id_dsa, ~/.ssh/identity)

	--no-default-identity

		It stops that -I- is set in the case of default.

	-e, --exec PROGRAM [OPTION ...]

		After the win-ssh-agent started, it executes the
		PROGRAM.  If the -I option is specified, it is
		executed after closing the pass-phrase dialogue.  In
		the case the pass-phrase dialogue was cancelled, it
		will not be executed.

	-a BIND_ADDRESS

		Bind the agent to the unix-domain socket bind_address.  The
		default is /tmp/ssh-XXXXXXXX/agent.<ppid>.

	-t LIFE

		Set a default value for the maximum lifetime of identities
		added to the agent.  The lifetime may be specified in seconds
		or in a time format specified in sshd(8).  A lifetime
		specified for an identity with ssh-add(1) overrides this
		value.  Without this option the default maximum lifetime is
		forever.


4. Compile and Install

	The win-ssh-agent.exe and the win-ssh-askpass.exe should be in
	the same directory.

	The ssh-agent.exe and ssh-add.exe must be found in the path of
	the PATH.

	If you want to re-compile, please do make.


5. Support 

	http://www.ganaware.jp/S/win-ssh-askpass/


6. Copyright

	Copyright (c) 2001-2006, TAGA Nayuta <nayuta@ganaware.jp>
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


7. History

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
