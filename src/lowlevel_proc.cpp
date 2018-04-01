/**
  ***************************************************************************************
  * @file    lowlevel_proc.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.0
  * @date    22/07/2013
  * @brief   This source file provides hooks callbacks definitions
  ***************************************************************************************
  * @copy
  *
  * Autor:  £ukasz Kaümierczak, 2013 POLSKA
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Prawa autorskie © 2013 £ukasz Kaümierczak. Wszystkie prawa zastrzeøone.
  * 
  * To Oprogramowanie zamieszczone w ürÛd≥ach internetowych lub innych zasobach podlega
  * ochronie praw autorskich. Wszelkie dzia≥ania zwiπzane z rozpowszechnianiem, modyfikowaniem,
  * publikowaniem rozwiπzaÒ zamieszczonych w Kodzie èrÛd≥owym wymaga uzgodnienia z autorem.
  *
  * AUTOR PREZENTOWANYCH ROZWI•ZA— NIE PONOSI ODPOWIEDZIALNOåCI ZA WSZELKIEGO RODZAJU
  * SZKODY(W TYM TAKØE OBOWI•ZKI ZWI•ZANE Z ICH LIKWIDACJ•) SPOWODOWANE UØYWANIEM LUB
  * MAJ•CE ZWI•ZEK Z UØYWANIEM OPROGRAMOWANIA. DOTYCZY TO R”WNIEØ PRZYPADKU GDY OSTRZEØENIE
  * LUB UWAGA O MOØLIWOåCI POJAWIENIA SI  SZKODY LUB PROBLEMU ZOSTA£O PRZEKAZANE.
  *
  * Author: £ukasz Kaümierczak, 2013 POLAND
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Copyright © 2013 £ukasz Kaümierczak. All rights reserved.
  * 
  * This Software available in Internet sources or other resources is protected by copyrights.
  * All actions related to redistribution, modifying or publishing solutions included in 
  * the Source Code require permission of the Author.
  *
  * AUTHOR OF PRESENTED SOLUTIONS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR ANY
  * TYPE OF DAMAGES(INCLUDING SERVICES OF LIQUIDATION OF THESE DAMAGES) ARISING IN
  * ANY WAY OF USE OF THIS SOFTWARE. THIS SHALL ALSO BE EFFECTIVE IF WARNING OR NOTICE
  * ABOUT POSSIBLE DAMAGE OR PROBLEM WAS COMMUNICATED.
  *
  * <h2><center>&copy; COPYRIGHT 2013 £ukasz Kaümierczak </center></h2>
*/
#include "lowlevel_proc.h"
#include <Windows.h>

#include "stream.h"
#include "wnds_mgmt.h"
#include "animation_mgmt.h"

#define MOD_HOOKS_PROC               "hooks_proc"

using namespace std;


LRESULT CALLBACK LowLevelMouseProc(INT code, WPARAM wParam, LPARAM lParam)
{
    return LPARAM(0);

    if (code == HC_ACTION)
    {
        const char *msg;
        char msg_buff[128];
        switch (wParam)
        {
            case WM_MBUTTONDOWN: break;
            case WM_MBUTTONUP:   break;
            case WM_LBUTTONDOWN: msg = "WM_LBUTTONDOWN"; break;
            case WM_LBUTTONUP:   msg = "WM_LBUTTONUP";   break;
            case WM_MOUSEMOVE:   msg = "WM_MOUSEMOVE";   return 1;
            case WM_MOUSEWHEEL:  msg = "WM_MOUSEWHEEL";  break;
            case WM_MOUSEHWHEEL: msg = "WM_MOUSEHWHEEL"; break;
            case WM_RBUTTONDOWN: msg = "WM_RBUTTONDOWN"; break;
            case WM_RBUTTONUP:   msg = "WM_RBUTTONUP";   break;
            default:
                sprintf_s(msg_buff, "Unknown msg: %u", wParam);
                msg = msg_buff;
                break;
        }
        switch (wParam)
        {
            case WM_LBUTTONDOWN: msg = "WM_LBUTTONDOWN"; 
                break;
            default:
                sprintf_s(msg_buff, "Unknown msg: %u", wParam);
                msg = msg_buff;
                break;
        }

        stream::system_message(MOD_HOOKS_PROC, "mouse: "+ string(msg));
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
    static const DWORD key_c = 0x43;

    if (nCode == HC_ACTION)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        DWORD Keystroke    = p->vkCode;
        //stream::system_message("hook", "button test: "+decToString(nCode)+" / "+decToString(p->vkCode));
        switch (wParam)
        {
            case WM_KEYDOWN:
            {
                switch(Keystroke) {
                    case VK_RETURN:
                    {
                        extern animation *coreAnimation;
                        if(coreAnimation != NULL) {
                            coreAnimation->terminateAnimationWNotification("_END_");
                            return 1;
                        }
                        break;
                    }
                    case key_c:
                        if( isKeyDown(VK_CONTROL) || isKeyDown(VK_LCONTROL) || isKeyDown(VK_RCONTROL) ) {
                           //closeWindows(window_info(GetForegroundWindow()));
                           //return 1;
                          //ExitProcess(EXIT_SUCCESS);
                        }
                        break;
                    case VK_LWIN:
                    case VK_RWIN:
                        stream::system_message(MOD_HOOKS_PROC, "WIN button blocked!");
                        return 1;
                    case VK_APPS:
                        stream::system_message(MOD_HOOKS_PROC, "APP button blocked!");
                        return 1;
                    default:
                    {}
                }
            }
            case WM_SYSKEYDOWN:
            {
                switch(Keystroke) {
                    case VK_F4:
                    case 91:
                        if(isKeyDown(VK_MENU)) {
                            stream::system_message(MOD_HOOKS_PROC, "ALT+F4 blocked!");
                            return 1;
                        }
                        break;
                    case VK_TAB:
                        if(isKeyDown(VK_MENU)) {
                            stream::system_message(MOD_HOOKS_PROC, "ALT+TAB blocked!");
                            return 1;
                        }
                        break;
                    case VK_ESCAPE:
                        if(isKeyDown(VK_MENU)) {
                            stream::system_message(MOD_HOOKS_PROC, "ALT+ESC blocked!");
                            return 1;
                        }
                        if( isKeyDown(VK_CONTROL) || isKeyDown(VK_LCONTROL) || isKeyDown(VK_RCONTROL) ) {
                            stream::system_message(MOD_HOOKS_PROC, "CTRL+ESC blocked!");
                            return 1;
                        }
                        break;
                    default:
                    {}
                }
            }
            case WM_KEYUP:
            case WM_SYSKEYUP:
            default: { /* no action defined */ }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

