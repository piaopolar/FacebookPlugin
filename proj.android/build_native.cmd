@echo off
set APPNAME="CMWar"
set NDK_ROOT=D:\SDK\adt-bundle\android-ndk-r9b
set APP_ANDROID_ROOT=%~dp0
set APP_ROOT=%APP_ANDROID_ROOT%..
set COCOS2DX_ROOT=%APP_ROOT%/../..
set NDK_MODULE_PATH=%COCOS2DX_ROOT%;%COCOS2DX_ROOT%/cocos2dx/platform/third_party/android/prebuilt
::set NDK_MODULE_PATH=%NDK_MODULE_PATH%;%COCOS2DX_ROOT%/cocos2dx/platform/third_party/android/source

echo NDK_ROOT = %NDK_ROOT%
echo COCOS2DX_ROOT = %COCOS2DX_ROOT%
echo APP_ROOT = %APP_ROOT%
echo APP_ANDROID_ROOT = %APP_ANDROID_ROOT%
echo NDK_MODULE_PATH = %NDK_MODULE_PATH%

del %APP_ANDROID_ROOT%assets /Q /S /F  1 > nul 2> nul
echo d|xcopy %APP_ROOT%\Resources %APP_ANDROID_ROOT%\assets /E /R /Y > nul
echo d|xcopy %APP_ROOT%\libs\UMCocos2dxAnalytics\platforms\android\libs %APP_ANDROID_ROOT%\libs /E /R /Y > nul

@echo on
call %NDK_ROOT%/ndk-build.cmd %1