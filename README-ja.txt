

		win-ssh-agent, win-ssh-askpass (for the cygwin openssh)


1. �͂��߂�

	win-ssh-agent ���g�p����ƁAcygwin �� openssh �� ssh-agent ����
	��X�}�[�g�ɗ��p�ł���悤�ɂȂ�܂��B

	�ʏ� ssh-agent �𗘗p���邽�߂ɂ́Assh-agent ���N�������V�F��
	(��: bash) ���炻�̑��̃v���O�������N������K�v������܂��B����
	�́Assh-agent ���ݒ肷����ϐ� SSH_AUTH_SOCK �𑼂̃\�t�g�E�F
	�A����Q�Ƃł��Ȃ���΂Ȃ�Ȃ����߂ł��B

	win-ssh-agent �́Assh-agent ���ݒ肷����ϐ���S�Ẵv���O��
	�����Q�Ƃł���悤�ɂ��܂��̂ŁA����̃V�F�����瑼�̃v���O����
	���N������K�v���Ȃ��Ȃ�܂��B


2. �C���X�g�[��

	win-ssh-agent.exe �� win-ssh-askpass.exe �𓯂��f�B���N�g���ɒu
	���Ă��������B(��: /usr/local/bin/ �Ȃ�)

	cygwin �� openssh ���K�v�ł��B����m�F�� openssh 5.8p1-1 �ōs��
	�܂����B
	# 5.9p1-1 �� openssh ���g�̃o�O�̂��ߓ��삵�܂���

	Windows �̊��ϐ� PATH �� cygwin �� /bin ��ǉ����Ă��������B
	(��: PATH=C:\cygwin\bin;...)


3. ���s�E�I��

	win-ssh-agent.exe ���_�u���N���b�N���ċN�����Ă��������B
	
	����ƁA�p�X�t���[�Y�𕷂���܂��B���͌�Awin-ssh-agent �̓^�X
	�N�o�[�̒[�̒ʒm�̈�ɏ풓���܂��B
	
	�I������ɂ́A�ʒm�̈���̃A�C�R�����E�N���b�N���� Exit ��I��
	�ł��������B


4. ������

	win-ssh-agent �͋N�����ꂽ��A�����ŏ���� ssh-agent ���N������
	���B�����āA�ȉ��̊��ϐ��̐ݒ�����܂��B

		SSH_AGENT_PID	ssh-agent.exe �̃v���Z�X ID
		SSH_AUTH_SOCK	ssh-agent.exe �̔F�ؗp�\�P�b�g�p�X
		SSH_ASKPASS	win-ssh-askpass.exe �̃p�X
		DISPLAY		:0

	(��DISPLAY ���ϐ����ݒ肳�������́A���� DISPLAY ���ϐ���
	�ݒ肳��Ă��炸�A���� win-ssh-agent ���N�����鎞��
	--no-DISPLAY �I�v�V�������w�肵�Ȃ������ꍇ�݂̂ł�)

	�����̐ݒ�� Explorer �ɑ΂��čs���A�Ȍ� Explorer �o�R�ŋN
	������S�ẴA�v���P�[�V�����͂����̊��ϐ����p�����܂��B

	win-ssh-agent �̓^�X�N�o�[�̒[�̒ʒm�̈�ɏ풓���A�I�����ɂ���
	��̊��ϐ����폜���܂��B


5. win-ssh-agent.exe �̃I�v�V����

	--no-ssh-agent

		�ʏ�́Awin-ssh-agent �͋N�����ꂽ��A�����ŏ���� 
		ssh-agent ���N�����܂����A�N�����Ȃ��悤�ɂ��܂��B

	--no-DISPLAY

		DISPLAY ���ϐ��� win-ssh-agent ������ɐݒ肵�Ȃ�
		�悤�ɂ��܂��B

	-i, --identity FILENAME

		������ identity �t�@�C�����g�p����ꍇ�A���̃I�v�V����
		�� identity �t�@�C�����w�肵�܂��B���̃I�v�V�����͕���
		�w��\�ł��B

	-I, --default-identity FILENAME

		-i �Ɠ����ł����Awin-ssh-agent �N�����Ƀp�X�t���[�Y
		��₢���킹��_�C�A���O��\�����܂��B���̃I�v�V������
		������w��ł��܂���B

		�f�t�H���g�ł� -I - ���ÖقɎw�肳��Ă��܂��BFILENAME 
		�Ƃ��� - ���w�肷��ƁAssh-add �������Ȃ��Ŏ��s������
		���Ɠ����t�@�C�� (~/.ssh/id_rsa, ~/.ssh/id_dsa,
		~/.ssh/id_ecdsa, ~/.ssh/identity) ���w�肵�����ƂɂȂ�
		�܂��B

	--no-default-identity

		�f�t�H���g�� -I - ���w�肳��Ȃ��悤�ɂ��܂��B

	-e, --exec PROGRAM [OPTION ...]

		���̃I�v�V�����ȍ~�ɏ������v���O������ win-ssh-agent 
		�N����Ɏ��s���܂��B-I ���w�肳��Ă����ꍇ�́A�_�C�A
		���O�����Ă�����s���܂����A�p�X�t���[�Y���͂��L����
		�Z�����ꂽ�ꍇ�͎��s���܂���B

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


6. �R���p�C��

	$ make && make install


7. �T�|�[�g

	https://github.com/ganaware/win-ssh-askpass


8. Copyright

	Copyright (c) 2001-2006, 2011 TAGA Nayuta <nayuta@ganaware.jp>
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


9. ����

	2011/10/?? 1.07
		* --hide-console �I�v�V�������폜�B�w�肵�Ă���������܂��B
		* win-ssh-agent.exe ���_�u���N���b�N���ċN�������Ƃ��ɁA
		  �R���\�[���E�B���h�E���J���Ȃ��悤�ɕύX�B
		* openssh 5.9p1-1 �� ssh-agent �͒ʐM�p�� unix domain socket
		  �̍쐬�Ɏ��s����悤�Ȃ̂ŁA���삵�܂���B
		  ���̃o�[�W�������g�p���Ă��������B(5.8p1-1 �œ���m�F�ς�)
		* README �̏C��
		* �ׂ��ȏC��

	2011/10/14 1.06
		* Cygwin 1.7 ���Ńr���h�ł��Ȃ��Ȃ��Ă����̂ŏC�����܂���
		* Visual�X�^�C���̃_�C�A���O�{�b�N�X���g�p����悤�ɂȂ�܂���
		* Unicode API ��ϋɓI�Ɏg���悤�ɂ��܂���

	2006/04/02 1.05
		* DISPLAY ���ϐ��ɐݒ肷��l�� :0 �ɂ����B(�ȑO�� 
                  localhost:0 ������)
		* �p�� README �̓Y�t�B(Thanks to Ralf Hauser and Naoko)
		* -a �� -t �I�v�V�����̒ǉ��B
		* cygwin 1.5.19 �ɑΉ��B

	2002/09/22 1.04
		* win-ssh-agent �� win-ssh-askpass �𕪗��B
		* -I - ���f�t�H���g�Ŏw�肳���悤�ɕύX�B
		* --hide-console ��ǉ��B
		* sample.bat ��p�~�B

	2002/02/27 1.03
		* -I �� -e �𓯎��Ɏw�肵���ꍇ�A�p�X�t���[�Y��₢��
		  �킹��_�C�A���O���L�����Z������� -e �͎��s����Ȃ�
		  �悤�ɂ����B(Patched by �O�R����)
		* sample.bat ��t���B

	2001/12/09 1.02
		* ������ identity �t�@�C�����g�p�\�ɂ����B
		* win-ssh-askpass ���s���Ƀp�X�t���[�Y�₢���킹�_�C�A
		  ���O���J���悤�ɂł���悤�ɂ����B
		* win-ssh-askpass ���s��ɋN������A�v���P�[�V�������w
		  ��ł���悤�ɂ����B

	2001/11/10 1.01
		* make �������Ƀ����N���[�j���O���o�Ȃ��悤�ɂ����B
		
	2001/11/04 1.00
		* ������B
