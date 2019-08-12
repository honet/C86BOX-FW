# C86BOX Firmware
<img src="https://github.com/honet/C86BOX-FW/raw/master/doc/topdesign.jpg" width=800>
Firmware for C86BOX (NEC PC-98Series C-BUS to USB Bridge BOX).   

C86BOX http://c86box.com/ の内部ファームウェアです。


## 関連ソフト
#### C86BoxFirmware
C86BOXファームウェアの本体。本ソフト。  
https://github.com/honet/C86BOX-FW

#### C86BoxFirmware Bootloader 
C86BOXファームウェアのブートローダ部。  
https://github.com/honet/C86BOX-BOOTFW

#### C86CTL
C86BOXをWindowsから制御する際に使うライブラリ  
https://github.com/honet/c86ctl

#### C86BoxConfig, Util, Sample
C86BOXへボード種別を指定したりするためのwindows用ユーティリティ  
https://github.com/honet/C86BoxUtils

#### C86BoxFirmwareUpdater
ブートローダが書き込まれたC86BOXのファームウェアを更新するためのwindows用ユーティリティ  
https://github.com/honet/C86BoxUpdater




## NOTE
- v0005 は PSoC Creator 3.3 でビルドしています。
- デバッグを行う際や、ブートローダが無い状態でFWを書き込むには
[miniProg3](https://japan.cypress.com/documentation/development-kitsboards/cy8ckit-002-psoc-miniprog3-program-and-debug-kit
)
を利用します。
