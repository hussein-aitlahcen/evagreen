cd /d "E:\Projects\EvaGreen\src\EvaGreen.Central" &msbuild "EvaGreen.Central.xproj" /t:sdvViewer /p:configuration="Debug" /p:platform=Any CPU
exit %errorlevel% 