/**
  ***************************************************************************************
  * @file    wnds_proc.cpp
  * @author  �ukasz Ka�mierczak
  * @version V1.0
  * @date    24/07/2013
  * @brief   This source file provides windows callbacks definitions
  ***************************************************************************************
  * @copy
  *
  * Autor:  �ukasz Ka�mierczak, 2013 POLSKA
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Prawa autorskie � 2013 �ukasz Ka�mierczak. Wszystkie prawa zastrze�one.
  * 
  * To Oprogramowanie zamieszczone w �r�d�ach internetowych lub innych zasobach podlega
  * ochronie praw autorskich. Wszelkie dzia�ania zwi�zane z rozpowszechnianiem, modyfikowaniem,
  * publikowaniem rozwi�za� zamieszczonych w Kodzie �r�d�owym wymaga uzgodnienia z autorem.
  *
  * AUTOR PREZENTOWANYCH ROZWI�ZA� NIE PONOSI ODPOWIEDZIALNO�CI ZA WSZELKIEGO RODZAJU
  * SZKODY(W TYM TAK�E OBOWI�ZKI ZWI�ZANE Z ICH LIKWIDACJ�) SPOWODOWANE U�YWANIEM LUB
  * MAJ�CE ZWI�ZEK Z U�YWANIEM OPROGRAMOWANIA. DOTYCZY TO R�WNIE� PRZYPADKU GDY OSTRZE�ENIE
  * LUB UWAGA O MO�LIWO�CI POJAWIENIA SI� SZKODY LUB PROBLEMU ZOSTA�O PRZEKAZANE.
  *
  * Author: �ukasz Ka�mierczak, 2013 POLAND
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Copyright � 2013 �ukasz Ka�mierczak. All rights reserved.
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
  * <h2><center>&copy; COPYRIGHT 2013 �ukasz Ka�mierczak </center></h2>
*/
#include "wnds_proc.h"
#include <Windows.h>

#include "stream.h"
#include "process_mgmt.h"
#include "wnds_mgmt.h"

#define MOD_WNDS_PROC               "wnds_proc"

using namespace std;


BOOL CALLBACK EnumWndProc(HWND hwnd, __in LPARAM lParam)
{
    if(lParam != NULL) {
        window_search *wnd = (window_search*)(lParam);
        wnd->analyze_element(window_info(hwnd)); 
        return TRUE;
    }

    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            break;
        case WM_CREATE:
            ShowCursor(FALSE);
            break;
        case WM_SETCURSOR:
            break;
        case WM_ERASEBKGND:
            if(isWndProcUpdateBlockadeActive() == TRUE) {
                return 0;
            }
            processes::event_signal(window_text::wndTextEvent);
            break;
        case WM_PAINT: 
            return 0;
            break;
        case WM_KILLFOCUS:
            break;
        case WM_LBUTTONDOWN:
            break;
        case WM_TIMER:
/*         switch(wParam) {
               case IDT_TIMER2: 
                    bringConsoleForeground();
                    bringWindowForeground(hwnd);
                    break;
            }
*/
            break;
        default: { /* no action defined */ }
     }

     return DefWindowProc(hwnd, message, wParam, lParam);
}

