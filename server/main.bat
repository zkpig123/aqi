@echo off
if not exist main mkdir main
cd main
asn1c -S C:\asn1c\skeletons -fskeletons-copy ..\..\aqi.asn
for %%i in (server-main.c server.c server.h assist.c asn.c aqi.h md5.h md5.c asn.h libmysql.lib) do copy /Y ..\..\%%~ni%%~xi .\
del converter-sample.c
gcc -c *.c -I.
gcc -o ..\main.exe *.o -lws2_32 -lmswsock -lpthread libmysql.lib
cd ..
copy /Y ..\libmysql.dll .\
echo success
pause
