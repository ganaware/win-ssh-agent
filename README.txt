

			   win-ssh-askpass


1. �͂��߂�

	X ���g�p����ꍇ�A.xsession �� .xinitrc �� ssh-agent ���N����
	�Ă����΁A���ϐ� SSH_AGENT_PID �� SSH_AUTH_SOCK ��S�Ẵv��
	�O�������Q�Ƃł���悤�ɂȂ�A���ɕ֗��ł��B

	�Ƃ��낪 Windows �� cygwin �� openssh ���g�p���Ă��鎞�ɓ��l��
	����������悤�Ƃ��Ă��Ȃ��Ȃ����܂������܂���Bssh ���g�p����
	�\���̂���S�Ẵv���O������ ssh-agent ���g�p���� bash �o�R
	�ŋN������Ȃǂ̕��@���Ƃ�˂΂Ȃ�܂���B

	win-ssh-askpass �́A

	(a) ssh-agent �̊��ϐ� SSH_AGENT_PID �� SSH_AUTH_SOCK ��S��
	    �̃v���O�������Q�Ƃł���悤�ɂ���B
	(b) X �p�� ssh-askpass �Ɠ��l�̋@�\��񋟂���B

	�Ƃ������Ƃ����܂��B


2. �g����

	(a) �X�^�[�g�A�b�v�Ȃǂ� win-ssh-askpass.exe ���N������ꍇ�B

		win-ssh-askpass.exe �͋N�����ꂽ��A�����ŏ���� 
		ssh-agent.exe ���N�����܂��B�����āA�ȉ��̊��ϐ��̐�
		������܂��B

		  SSH_AGENT_PID	ssh-agent.exe �̃v���Z�X ID�B
		  SSH_AUTH_SOCK	ssh-agent.exe �̔F�ؗp�\�P�b�g�p�X�B
		  SSH_ASKPASS	win-ssh-askpass.exe �̃p�X�B
		  DISPLAY	localhost:0

		(��DISPLAY ���ϐ����ݒ肳�������́A���� DISPLAY ��
		���ϐ����ݒ肳��Ă��炸�A���� win-ssh-askpass.exe ��
		�N�����鎞�� --no-DISPLAY �I�v�V�������w�肵�Ȃ�������
		���݂̂ł�)

		�����̐ݒ�� Explorer �ɑ΂��čs���A�Ȍ� Explorer 
		�o�R�ŋN������S�ẴA�v���P�[�V�����͂����̊��ϐ�
		���p�����܂��B

		win-ssh-askpass.exe �̓^�X�N�g���C�ɏ풓���A�I�����ɂ�
		���̊��ϐ����폜���܂��B

		������������������������������������������������������
		�ŋ߂� cygwin �� openssh ���g�p�����ꍇ�A�X�^�[�g�A�b
		�v�Ȃǂ� win-ssh-askpass.exe �ւ̃V���[�g�J�b�g���쐬
		���āA������N�����Ă����܂������Ȃ��悤�ł��B����� 
		sample.bat �̂悤�ȃo�b�`�t�@�C���ւ̃V���[�g�J�b�g��
		�쐬���ċN������Ƃ��܂������܂��B�|�C���g�� start /b 
		�ł��B
		������������������������������������������������������

	(b) ssh-add.exe �o�R�ŋN�������ꍇ�B

		���̏ꍇ�A���ϐ� DISPLAY �� SSH_ASKPASS ���K�؂ɐݒ�
		����Ă���� ssh-add.exe �� win-ssh-askpass.exe ���N��
		���܂��B���̏ꍇ X �p�� ssh-askpass �Ɠ��l�ɁA�p�X�t���[
		�Y����͂���悤�ɑ�����܂��B

	openssh �̃c�[��������Ŏg�p���邽�߁Acygwin �� /bin �փp�X��
	�Ƃ����Ă���K�v������܂��B


3. �I�v�V����

	--no-ssh-agent

		�ʏ�́Awin-ssh-askpass �͋N�����ꂽ��A�����ŏ���� 
		ssh-agent ���N�����܂����A�N�����Ȃ��悤�ɂ��܂��B

	--no-DISPLAY

		DISPLAY ���ϐ��� win-ssh-askpass ������ɐݒ肵�Ȃ�
		�悤�ɂ��܂��B(2. (a) �Q��)

	-i, --identity FILENAME

		������ identity �t�@�C�����g�p����ꍇ�A���̃I�v�V����
		�� identity �t�@�C�����w�肵�܂��B���̃I�v�V�����͕���
		�w��\�ł��B

	-I, --default-identity FILENAME

		-i �Ɠ����ł����Awin-ssh-askpass �N�����Ƀp�X�t���[�Y
		��₢���킹��_�C�A���O��\�����܂��B���̃I�v�V������
		������w��ł��܂���B

	-e, --exec PROGRAM [OPTION ...]

		���̃I�v�V�����ȍ~�ɏ������v���O������ win-ssh-askpass 
		�N����Ɏ��s���܂��B-I ���w�肳��Ă����ꍇ�́A�_�C�A
		���O�����Ă�����s���܂����A�p�X�t���[�Y���͂��L����
		�Z�����ꂽ�ꍇ�͎��s���܂���B


4. �R���p�C���E�C���X�g�[��

	cygwin ���C���X�g�[�����ꂽ��Ԃ� make ���Ă��������B�o�C�i��
	�͒񋟂��܂���B


5. �T�|�[�g

	http://www.ganaware.jp/S/win-ssh-askpass/


6. Copyright

	Copyright (c) 2001-2002, TAGA Nayuta <nayuta@ganaware.jp>
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


7. ����

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
