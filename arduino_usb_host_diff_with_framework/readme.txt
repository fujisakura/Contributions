【修正ファイルのarduino IDEへの適用とＬチカプログラムの作成方法】

１）修正ファイルの適用
　　arduino-1.5.2-win配下のディレクトリ、ファイルを修正適用先のarduino IDEの
　　ディレクトリへ上書きコピーします。

２）arduino IDEの実行
　　修正ファイルを適用したarduino IDE内の arduino.exeを実行します。

３）Ｌチカ用プログラムを開く
　　メニューから「ファイル」－「開く」で以下のファイルを開きます。

　　\arduino-1.5.2\libraries\USBHost\examples\UsbStorageFatFs\UsbStorageFatFs.ino
    \arduino-1.5.2\libraries\USBHost\examples\KeyboardController\KeyboardController.ino
    \arduino-1.5.2\libraries\USBHost\examples\MouseController\MouseController.ino

４）ボードの選択
　　メニューから「ツール」－「board」－「Interface FM3」を選択。

５）ビルド
　　メニューから「ファイル」－「マイコンボードに書き込む」でビルドを実行。

６）生成ファイルの確認
　　以下のディレクトリ内にファイルが生成されていることを確認。
　　\arduino-1.5.2\build
　　　　UsbStorageFatFs.cpp
　　　　UsbStorageFatFs.cpp.elf
　　　　UsbStorageFatFs.cpp.hex
　　　　UsbStorageFatFs.cpp.map
　　　　UsbStorageFatFs.cpp.o

　　　　KeyboardController.cpp
　　　　KeyboardController.cpp.elf
　　　　KeyboardController.cpp.hex
　　　　KeyboardController.cpp.map
　　　　KeyboardController.cpp.o

　　　　MouseController.cpp
　　　　MouseController.cpp.elf
　　　　MouseController.cpp.hex
　　　　MouseController.cpp.map
　　　　MouseController.cpp.o

　　※sam3ボードへの書き込み処理を改造し、次の USB DIRECT Programmerでの
　　　ＦＭ３ボードへの書き込みを容易にするため、上記処理としています。

７）ＦＭ３ボード書き込みツールの用意
　　富士通セミコンダクターの以下のページから USB DIRECT Programmerを
　　ダウンロードし、インストールする。

８）ＦＭ３ボードへ書き込む
　　USB DIRECT Programmerを起動し、以下のファイルをＦＭ３ボードへ書き込む。

　　\arduino-1.5.2\build\UsbStorageFatFs.cpp.hex
　　\arduino-1.5.2\build\KeyboardController.cpp.hex
　　\arduino-1.5.2\build\MouseController.cpp.hex

　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　以上
