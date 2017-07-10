@ECHO OFF
CD /D "%~dp0"

SET "ToolDir=F:\Code\_Tools\WppLog\Windows"
SET "LogName=MultiWindowHelper"

"%ToolDir%\tracelog.exe" -stop "%LogName%"
"%ToolDir%\tracelog.exe" -stop "577C26C1-1F1E-4257-8D0C-115E54ED2DF1"

PAUSE
