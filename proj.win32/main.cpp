#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "IniMgr.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("FacebookPlugin");

	int nSimW = CIniMgr::GetInstance()->GetValue(DEBUG_INI, "Sim", "Width", 480);
	int nSimH = CIniMgr::GetInstance()->GetValue(DEBUG_INI, "Sim", "Height", 720);

    eglView->setFrameSize(nSimW, nSimH);
    return CCApplication::sharedApplication()->run();
}
