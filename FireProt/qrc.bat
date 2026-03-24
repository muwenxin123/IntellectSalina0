@rem
set path=%path%;D:\Qt\Qt5.11.2\5.11.2\msvc2015_64\bin
cd qrc
del FireProt.*
lrelease ..\..\Qt_zh_CN.ts -qm qm\Qt_zh_CN.qm
rcc -project -o ..\FireProt.qrc
move ..\FireProt.qrc FireProt.qrc
rcc -binary FireProt.qrc -o ..\..\..\bin\FireProt.rcc
@rem pause
