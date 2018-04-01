/**
  ***************************************************************************************
  * @file    lowlevel_hooks.cpp
  * @author  �ukasz Ka�mierczak
  * @version V1.0
  * @date    20/07/2013
  * @brief   This source file provides support for LL hooks operations
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
#include "lowlevel_hooks.h"
#include <Windows.h>

#define MOD_HOOKS_MGMT               "hooks_mgmt"

using namespace std;


UINT_PTR hooks::hook_create( hookType type, LRESULT (CALLBACK *hkFct)(INT, WPARAM, LPARAM) )
{
    INT hkType       = 0;
    UINT_PTR fresult = NULL;
    HINSTANCE hinst  = GetModuleHandle(NULL);

    switch(type)
    {
        case HOOK_KEYBOARD:
            hkType = WH_KEYBOARD_LL;
            break;
        case HOOK_MOUSE:
            hkType = WH_MOUSE_LL;
            break;
        default:
        {
            return fresult;
        }
    }

    HHOOK hkLowLevel = SetWindowsHookEx(WH_KEYBOARD_LL, (*hkFct), hinst, 0);
    fresult = (UINT_PTR)hkLowLevel;

    return fresult;
}

FunctionSuccesFailure hooks::hook_destroy(UINT_PTR hkHookRaw)
{
    HHOOK hkLowLevel = (HHOOK)hkHookRaw;

    if( UnhookWindowsHookEx(hkLowLevel) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

