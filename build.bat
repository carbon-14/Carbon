
echo off
cls

echo -- premake build -- only windows 32-bits for now
echo Configure build :
echo 1. vs2010
echo 2. vs2008
echo 3. vs2005
echo 4. vs2003
echo 5. vs2002
echo 6. gmake
echo 7. xcode3
echo 8. codeblocks
echo 9. codelite
set /p v=:

if %v%==1 set a=vs2010
if %v%==2 set a=vs2008
if %v%==3 set a=vs2005
if %v%==4 set a=vs2003
if %v%==5 set a=vs2002
if %v%==6 set a=gmake
if %v%==7 set a=xcode3
if %v%==8 set a=codeblocks
if %v%==9 set a=codelite
if "%a%"=="" goto end

echo [ building %a% - windows - x32 ]
utilities\\premake\\premake4  --file=sources\\premake.lua --os=windows --platform=x32 %a%

:end
pause
