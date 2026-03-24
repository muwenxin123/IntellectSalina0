
cd qrc

lrelease ..\..\FireProt.pro
xcopy ..\..\*.qm .\qm\ /F /Y

del FireProt.qrc

rcc -project -o ..\FireProt.qrc
move ..\FireProt.qrc FireProt.qrc
rcc -binary FireProt.qrc -o ..\..\..\bin\FireProt.rcc

