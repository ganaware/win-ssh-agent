

		win-ssh-agent, win-ssh-askpass (for the cygwin openssh)


1. はじめに

	win-ssh-agent を使用すると、cygwin の openssh の ssh-agent をよ
	りスマートに利用できるようになります。

	通常 ssh-agent を利用するためには、ssh-agent を起動したシェル
	(例: bash) からその他のプログラムを起動する必要があります。これ
	は、ssh-agent が設定する環境変数 SSH_AUTH_SOCK を他のソフトウェ
	アから参照できなければならないためです。

	win-ssh-agent は、ssh-agent が設定する環境変数を全てのプログラ
	ムが参照できるようにしますので、特定のシェルから他のプログラム
	を起動する必要がなくなります。


2. インストール

	win-ssh-agent.exe と win-ssh-askpass.exe を同じディレクトリに置
	いてください。(例: /usr/local/bin/ など)

		32bit 版の Cygwin の場合は付属の win-ssh-agent.exe と
		win-ssh-askpass.exe を使用することができます。

		64bit 版の Cygwin の場合は、ソースをコンパイルしなおす
		必要があります。「6. コンパイル」の手順を行ってから、上
		記手順を行ってください。

	cygwin の openssh が必要です。動作確認は openssh 6.2p2-1 で行い
	ました。

	Windows の環境変数 PATH に cygwin の /bin を追加してください。
	(例: PATH=C:\cygwin\bin;... や PATH=C:\cygwin64\bin;...)


3. 実行・終了

	win-ssh-agent.exe をダブルクリックして起動してください。
	
	すると、パスフレーズを聞かれます。入力後、win-ssh-agent はタス
	クバーの端の通知領域に常駐します。
	
	終了するには、通知領域内のアイコンを右クリックして Exit を選ん
	でください。


4. 動作解説

	win-ssh-agent は起動された後、内部で勝手に ssh-agent を起動しま
	す。そして、以下の環境変数の設定をします。

		SSH_AGENT_PID	ssh-agent.exe のプロセス ID
		SSH_AUTH_SOCK	ssh-agent.exe の認証用ソケットパス
		SSH_ASKPASS	win-ssh-askpass.exe のパス
		DISPLAY		:0

	(※DISPLAY 環境変数が設定される条件は、既に DISPLAY 環境変数が
	設定されておらず、かつ win-ssh-agent を起動する時に
	--no-DISPLAY オプションを指定しなかった場合のみです)

	これらの設定は Explorer に対して行われ、以後 Explorer 経由で起
	動する全てのアプリケーションはこれらの環境変数を継承します。

	win-ssh-agent はタスクバーの端の通知領域に常駐し、終了時にこれ
	らの環境変数を削除します。


5. win-ssh-agent.exe のオプション

	--no-ssh-agent

		通常は、win-ssh-agent は起動された後、内部で勝手に 
		ssh-agent を起動しますが、起動しないようにします。

	--no-DISPLAY

		DISPLAY 環境変数を win-ssh-agent が勝手に設定しない
		ようにします。

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
		合と同じファイル (~/.ssh/id_rsa, ~/.ssh/id_dsa,
		~/.ssh/id_ecdsa, ~/.ssh/identity) を指定したことになり
		ます。

	--no-default-identity

		デフォルトで -I - が指定されないようにします。

	-e, --exec PROGRAM [OPTION ...]

		このオプション以降に書いたプログラムを win-ssh-agent 
		起動後に実行します。-I が指定されていた場合は、ダイア
		ログが閉じてから実行しますが、パスフレーズ入力がキャン
		セルされた場合は実行しません。

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


6. コンパイル

	$ make clean && make && make install

	NOTE:
		もし XP 環境で win-ssh-askpass.exe がうまく動作しないよ
		うならば、以下のコマンドラインを試してみてください。
		
		$ make install-xp


7. サポート

	https://github.com/ganaware/win-ssh-agent


8. Copyright

	Copyright (c) 2001-2006, 2011, 2013 TAGA Nayuta <nayuta@ganaware.jp>
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


9. 履歴

	2016/03/10 1.09
		* Cygwin 64-bit 用修正

	2013/08/08 1.08
		* make install-xp で Visual スタイルダイアログボックス
		  を使用しない exe を作成するようにしました。Windows
		  XP 環境で win-ssh-askpass.exe の起動に失敗するケース
		  があるケースに対応するためです。
		  (Thanks to Patrick Mézard)
		* Cygwin 64-bit 用修正

	2011/11/02 1.07
		* プロジェクト名を win-ssh-askpass から win-ssh-agent へ変更。
		* --hide-console オプションを削除。指定しても無視されます。
		* win-ssh-agent.exe をダブルクリックして起動したときに、
		  コンソールウィンドウを開かないように変更。
		* openssh 5.9p1-1 のバグを回避するために、内部構造を変更
		* README の修正
		* 細かな修正

	2011/10/14 1.06
		* Cygwin 1.7 環境でビルドできなくなっていたので修正しました
		* Visualスタイルのダイアログボックスを使用するようになりました
		* Unicode API を積極的に使うようにしました

	2006/04/02 1.05
		* DISPLAY 環境変数に設定する値を :0 にした。(以前は 
                  localhost:0 だった)
		* 英語 README の添付。(Thanks to Ralf Hauser and Naoko)
		* -a と -t オプションの追加。
		* cygwin 1.5.19 に対応。

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
