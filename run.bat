@ECHO OFF

SET RUNNER=jsonscanner.exe
SET INPUT_FILEPATH=./sample/text.txt
SET OUTPUT_FILEPATH=./sample/result.txt

%RUNNER% %INPUT_FILEPATH% %OUTPUT_FILEPATH%

pause