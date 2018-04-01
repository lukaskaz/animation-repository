/**
  ***************************************************************************************
  * @file    animation_mgmt.cpp
  * @author  £ukasz Kaümierczak
  * @version V2.2
  * @date    30/07/2013
  * @brief   This source file provides support for screen animation operations
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
#include "animation_mgmt.h"
#include "time_mgmt.h"
#include "wnds_mgmt.h"
#include "process_mgmt.h"
#include "lowlevel_hooks.h"
#include "stream.h"

#define MOD_ANIM               "anim_mgmt"

using namespace std;

#define DEFAULT_ANIMATION_TEXT "TeSt...\ntest1"
#define ANIMATION_WINDOW_CLASS "AnimationLayerClass"
#define ANIMATION_WINDOW_TITLE "AnimationLayer"

animation *coreAnimation = NULL;


DWORD WINAPI animationWindowThread(LPVOID lpParam)
{
    processes::thread_ready();
    UINT_PTR hhkKybd  = hooks::hook_create(hooks::HOOK_KEYBOARD, LowLevelKeyboardProc);

    window_info wndParams(ANIMATION_WINDOW_CLASS, ANIMATION_WINDOW_TITLE, WndProc, 0, 0, 0, 0);
    window_create new_window(wndParams, window_create::WND_DEP_DEPLOY, window_create::WND_VIS_TRANSPARENT_25, window_create::WND_STY_MAXIMIZED_POPUP);
    new_window.create();

    animation *aupdateCtrl = (animation*)lpParam;
    aupdateCtrl->destroyUpdateThread();

    hooks::hook_destroy(hhkKybd);
    return EXIT_SUCCESS;
}

DWORD WINAPI animationUpdateThread(LPVOID lpParam) 
{
    processes::thread_ready();

    animation *aupdate = (animation*)lpParam;
    while(processes::event_wait(aupdate->hKillUpdEventRaw, 0) == FUNCTION_FAILURE){
        aupdate->textValidate();
        timers::instance().timeout_create(50);
    }
    aupdate->textWaitForDestroy();

    processes::event_destroy(aupdate->hKillUpdEventRaw);
    aupdate->hKillUpdEventRaw = NULL;
    stream::system_message(MOD_ANIM, "animation killed!");

    return EXIT_SUCCESS;
}

DWORD WINAPI animationTerminationThread(LPVOID lpParam)
{
    const UINT aNotificationDisplayTimeMs = 4000;
    processes::thread_ready();
    animation *atermination = (animation*)lpParam;

    if(atermination->hPipeNamedRaw == NULL) {
        while(isWndProcUpdateBlockadeActive() == FALSE){
            timers::instance().timeout_create(10);
        };
        setWndProcUpdateBlockade(FALSE);
        ((window_text*)(atermination->animationTextWnd))->printTextUpdate(atermination->animationTextBuffer);
        setWndProcUpdateBlockade(TRUE);
    }

    timers::timeout_create(aNotificationDisplayTimeMs);
    atermination->destroyUpdateThread();
    closeWindows(window_info((HWND)atermination->animationHwndRaw));

    return EXIT_SUCCESS;
}


FunctionSuccesFailure animation::initialize(const string &pipeName)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    hKillUpdEventRaw = processes::event_create(TRUE, UNNAMED);
    if(hKillUpdEventRaw != NULL) {
        hWndThreadRaw = processes::thread_create(animationWindowThread, (LPVOID)this, 0);
        hUpdThreadRaw = processes::thread_create(animationUpdateThread, (LPVOID)this, 0);

        if(hWndThreadRaw != NULL && hUpdThreadRaw != NULL) {
            animationTextBuffer = DEFAULT_ANIMATION_TEXT;
            window_search search;

            do {
                search = window_search(window_info(ANIMATION_WINDOW_CLASS, ANIMATION_WINDOW_TITLE));
            }while(search.isExtractedEmpty() == TRUE);

            animationHwndRaw = (UINT_PTR)search.getExtracted().hwnd;
            animationTextWnd = (VOID*)(new window_text(animationHwndRaw));
            bringWindowForeground(animationHwndRaw);

            if(pipeName != UNNAMED) {
                hPipeNamedRaw = processes::pipe_named_open(TRUE, pipeName);
                while(1) {
                    while(processes::pipe_named_read(hPipeNamedRaw, animationTextBuffer) == FUNCTION_PENDING);
                    if(animationTextBuffer != EMPTY_STRING) {
                        break;
                    }
                }
            }

            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

BOOL animation::isActive() const
{
    if(processes::thread_wait(hWndThreadRaw, 0) != FUNCTION_SUCCESS) {
        return TRUE;
    }

    return FALSE;
}

FunctionSuccesFailure animation::terminateAnimationWNotification(string notification)
{
    if(hPipeNamedRaw != NULL) {
        while(processes::pipe_named_write(hPipeNamedRaw, decToString(FUNCTION_FAILURE)) == FUNCTION_FAILURE);
    }
    else {
        animationTextBuffer = notification;
        destroyUpdateThread();
    }

    if(processes::thread_create(animationTerminationThread, (LPVOID)this, 0) != NULL) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure animation::terminateAnimation()
{
    return animation::terminateAnimationWNotification(UNNAMED);
}

FunctionSuccesFailure animation::textValidate()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if( textReposition() == FUNCTION_SUCCESS ) {
        textUpdate();
        fresult = FUNCTION_SUCCESS;
    }
    else if( textUpdate() == FUNCTION_SUCCESS ) {
        textReposition();
        fresult = FUNCTION_SUCCESS;
    }
    else {
        // text is not to be validated now, continue
    }

    return fresult;
}

FunctionSuccesFailure animation::bufferUpdate()
{
    static UINT i = 0;
    if(hPipeNamedRaw != NULL) {
        while(processes::pipe_named_read(hPipeNamedRaw, animationTextBuffer) == FUNCTION_FAILURE);
    }

    return FUNCTION_SUCCESS;
}

FunctionSuccesFailure animation::textUpdate()
{
    static INT textUpdateTimer = timers::invalidTimer;
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(hKillUpdEventRaw == NULL || processes::event_wait(hKillUpdEventRaw, 0) == FUNCTION_FAILURE) {
        if( animationTextWnd != NULL &&
            (textUpdateTimer == timers::invalidTimer || timers::timeout_peek_left_ms(textUpdateTimer) <= 0) ) {

            textUpdateTimer = timers::timeout_peek_create(textUpdateTime);
            bufferUpdate();
            setWndProcUpdateBlockade(FALSE);
            ((window_text*)animationTextWnd)->printTextUpdate(animationTextBuffer);
            setWndProcUpdateBlockade(TRUE);

            fresult = FUNCTION_SUCCESS;
        }
    }
    else {
        timers::timeout_kill(textUpdateTimer);
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure animation::textReposition()
{
    static INT textRepositionTimer = timers::invalidTimer;
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(hKillUpdEventRaw == NULL || processes::event_wait(hKillUpdEventRaw, 0) == FUNCTION_FAILURE) {
        if( animationTextWnd != NULL &&
            (textRepositionTimer == timers::invalidTimer || timers::timeout_peek_left_ms(textRepositionTimer) <= 0) ) {

            textRepositionTimer = timers::timeout_peek_create(textRepositionTime);
            bufferUpdate();
            setWndProcUpdateBlockade(FALSE);
            ((window_text*)animationTextWnd)->printTextRandomPosition(textSize, TRUE, FALSE, FALSE, animationTextBuffer);
            setWndProcUpdateBlockade(TRUE);

            fresult = FUNCTION_SUCCESS;
        }
    }
    else {
        timers::timeout_kill(textRepositionTimer);
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure animation::textWaitForDestroy()
{
    if(textUpdate() == FUNCTION_SUCCESS && textReposition() == FUNCTION_SUCCESS) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure animation::destroyUpdateThread() const
{
    return processes::event_signal(hKillUpdEventRaw);
}

