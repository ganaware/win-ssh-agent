

		    win-ssh-agent, win-ssh-askpass


1. はじめに

	X を使用する場合、.xsession や .xinitrc で ssh-agent を起動し
	ておけば、環境変数 SSH_AGENT_PID と SSH_AUTH_SOCK を全てのプロ
	グラムが参照できるようになり、非常に便利です。

	ところが Windows の cygwin の openssh を使用している時に同様の
	動作をさせようとしてもなかなかうまくいきません。ssh を使用する
	可能性のある全てのプログラムを ssh-agent を使用した bash 経由
	で起動するなどの方法をとらねばなりません。

	win-ssh-agent は、ssh-agent の環境変数 SSH_AGENT_PID と 
	SSH_AUTH_SOCK を全てのプログラムが参照できるようにします。

	win-ssh-askpass は、X 用の ssh-askpass と同様の機能を提供しま
	す。

2. 動作

	win-ssh-agent.exe は起動された後、内部で勝手に ssh-agent.exe 
	を起動します。そして、以下の環境変数の設定をします。

		SSH_AGENT_PID	ssh-agent.exe のプロセス ID。
		SSH_AUTH_SOCK	ssh-agent.exe の認証用ソケットパス。
		SSH_ASKPASS	win-ssh-askpass.exe のパス。
		DISPLAY		localhost:0

	(※DISPLAY 環境変数が設定される条件は、既に DISPLAY 環境変数が
	設定されておらず、かつ win-ssh-agent.exe を起動する時に 
	--no-DISPLAY オプションを指定しなかった場合のみです)

	これらの設定は Explorer に対して行われ、以後 Explorer 経由で起
	動する全てのアプリケーションはこれらの環境変数を継承します。

	win-ssh-agent.exe はタスクトレイに常駐し、終了時にこれらの環境
	変数を削除します。
	

	win-ssh-askpass.exe は、環境変数 DISPLAY と SSH_ASKPASS が適切
	に設定されていると ssh-add.exe が win-ssh-askpass.exe を起動し
	ます。この場合 X 用の ssh-askpass と同様に、パスフレーズを入力
	するように促されます。
	

	openssh のツールを内部で使用するため、cygwin の /bin へパスが
	とおっている必要があります。cygwin の openssh の 3.4p1-5 以降
	を使用してください。それ以前のバージョンでも動作するかもしれま
	せんが、わかりません。


3. win-ssh-agent のオプション

	--no-ssh-agent

		通常は、win-ssh-agent は起動された後、内部で勝手に 
		ssh-agent を起動しますが、起動しないようにします。

	--no-DISPLAY

		DISPLAY 環境変数を win-ssh-agent が勝手に設定しない
		ようにします。(2. (a) 参照)

	--hide-console

		win-ssh-agent が属しているコンソールウィンドウを隠しま
		す。win-ssh-agent が終了すると、再び表示されます。

	-i, --identity FILENAME

		複数の identity ファイルを使用する場合、このオプション
		で identity ファイルを指定します。このオプションは複数
		指定可能です。

	-I, --default-identity FILENAME

		-i と同じですが、win-ssh-agent 起動時にパスフレーズ
		を問い合わせるダイアログを表示します。このオプションは
		一つしか指定できません。

		デフォルトでは -I - が暗黙に指定されています。FILENAME 
		として - を指定すると、ssh-add を引数なしで実行した場
		合と同じファイル (~/.ssh/id_rsa, ~/.ssh/id_dsa
		~/.ssh/identity) を指定したことになります。

	--no-default-identity

		デフォルトで -I - が指定されないようにします。

	-e, --exec PROGRAM [OPTION ...]

		このオプション以降に書いたプログラムを win-ssh-agent 
		起動後に実行します。-I が指定されていた場合は、ダイア
		ログが閉じてから実行しますが、パスフレーズ入力がキャン
		セルされた場合は実行しません。


4. コンパイル・インストール

	cygwin がインストールされた状態で make してください。バイナリ
	は提供しません。

	win-ssh-agent.exe と win-ssh-askpass.exe は同じディレクトリに
	置いてください。

	ssh-agent.exe と ssh-add.exe にパスが通っている必要があります。


5. サポート

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


7. 履歴

	2002/09/22 1.04
		* win-ssh-agent と win-ssh-askpass を分離。
		* -I - がデフォルトで指定されるように変更。
		* --hide-console を追加。
		* sample.bat を廃止。

	2002/02/27 1.03
		* -I と -e を同時に指定した場合、パスフレーズを問い合
		  わせるダイアログをキャンセルすると -e は実行されない
		  ようにした。(Patched by 外山純生)
		* sample.bat を付属。

	2001/12/09 1.02
		* 複数の identity ファイルを使用可能にした。
		* win-ssh-askpass 実行時にパスフレーズ問い合わせダイア
		  ログが開くようにできるようにした。
		* win-ssh-askpass 実行後に起動するアプリケーションを指
		  定できるようにした。

	2001/11/10 1.01
		* make した時にリンクワーニングが出ないようにした。
		
	2001/11/04 1.00
		* 作った。
