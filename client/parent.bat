@echo off
if not exist parent mkdir parent
cd parent
asn1c -S C:\asn1c\skeletons -fskeletons-copy ..\..\aqi.asn
for %%i in (client-parent.c assist.c asn.c aqi.h md5.h md5.c asn.h client.c client.h) do copy /Y ..\..\%%~ni%%~xi .\
del converter-sample.c
gcc -c *.c -I.
gcc -o ..\parent.exe *.o -lws2_32
cd ..
echo success
pause
