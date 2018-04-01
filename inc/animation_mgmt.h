/**
  ***************************************************************************************
  * @file    animation_mgmt.h
  * @author  �ukasz Ka�mierczak
  * @version V2.2
  * @date    30/07/2013
  * @brief   This header file provides animaated screen operation set
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
#ifndef ANIMATION_MGMT_H
#define ANIMATION_MGMT_H

#include "common.h"


class animation {
public:
    animation(): textSize(0), textRepositionTime(0), textUpdateTime(0), hPipeNamedRaw(NULL), animationTextBuffer(EMPTY_STRING),
        animationTextWnd(NULL), hWndThreadRaw(NULL), hUpdThreadRaw(NULL), hKillUpdEventRaw(NULL) {}
    animation(const std::string &pipeName, UINT size, UINT updateTime, UINT repositionTime) { this->animation::animation();
        textSize=size; textUpdateTime=updateTime; textRepositionTime=(updateTime<repositionTime)?repositionTime:updateTime; initialize(pipeName); }
    ~animation() { delete animationTextWnd; animationTextWnd = NULL; }

    FunctionSuccesFailure initialize(const std::string &pipeName);
    BOOL isActive() const;
    FunctionSuccesFailure terminateAnimationWNotification(std::string notification);

private:
    UINT textSize;
    UINT textRepositionTime;
    UINT textUpdateTime;
    UINT_PTR hPipeNamedRaw;
    std::string animationTextBuffer;
    VOID *animationTextWnd;
    UINT_PTR animationHwndRaw;
    UINT_PTR hWndThreadRaw;
    UINT_PTR hUpdThreadRaw;
    UINT_PTR hKillUpdEventRaw;

    FunctionSuccesFailure textValidate();
    FunctionSuccesFailure bufferUpdate();
    FunctionSuccesFailure textUpdate();
    FunctionSuccesFailure textReposition();
    FunctionSuccesFailure textWaitForDestroy();
    FunctionSuccesFailure terminateAnimation();
    FunctionSuccesFailure destroyUpdateThread() const;


    friend DWORD WINAPI animationWindowThread(LPVOID lpParam);
    friend DWORD WINAPI animationUpdateThread(LPVOID lpParam);
    friend DWORD WINAPI animationTerminationThread(LPVOID lpParam);
};


#endif
