------------------------------------------------------------------------
　　OSASK計画＆はりぼて友の会　コラボレーションCD-ROMの使い方
　　for OSC2006 Okinawa 　　　　　　　　　　　　　　　　　（2006/12/02）
------------------------------------------------------------------------

・使い方

1.Windows上からエミュレータで起動する
　CDを入れると自動でランチャーが起動するので、起動したいOSを選択してくだ
さい。Windows9x(Windows98以上)/NT両対応です(seribotoOSのみ、 Windows9x非
対応)。自動で起動しない場合は、haritomoディレクトリのharitomo.exeを起動
してください。

2.CDから直接ブートして起動する
　CDを入れた状態でPCを起動するとOS選択メニューが表示されるので、起動した
いOSを選択してください。メニュー表示後15秒間キー入力がなければ、通常通り
HDD 起動が始まります。OSを終了するには、CD-ROMをPCから取り出し、PCのリセ
ットボタンで再起動するか、電源ボタンで電源を切ってください。


・ディレクトリ構成図
　/
  +- bcdw/		CDブート用プログラム
  +- haritomo/		エミュレータ起動用OSランチャープログラム
  +- osimage/		各OSディスクイメージ
  +- ossrc/		各OSソースコード
  |   +- BayOS/		BayOS 20061029版
  |   +- HiyOS/		HiyOS 0.1.0.20061112
  |   +- Mtorz/		Mt.orz(OS) Ver.0.2 (おまけ)
  |   +- OSAkkie/	OSAkkie Ver.0.1.02
  |   +- OSASK/		OSASK Ver.4.8
  |   +- rapuOS/	rapuOS ver.0.1.1.1
  |   +- seriboteOS/	seriboteOS 2006/10/16リリース版
  +- qemu/		qemu置き場
  |   +- nt/		NT用qemu
  |   +- 9x/		9x用qemu
  |   +- serial/	SeriboteOS用シリアル対応版qemu
  +- tolset/		各OS開発環境


------------------------------------------------------------------------
