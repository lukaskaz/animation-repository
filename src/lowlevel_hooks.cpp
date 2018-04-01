/**
  ***************************************************************************************
  * @file    lowlevel_hooks.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.0
  * @date    20/07/2013
  * @brief   This source file provides support for LL hooks operations
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

