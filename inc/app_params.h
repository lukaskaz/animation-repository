/**
  ***************************************************************************************
  * @file    app_params.h
  * @author  �ukasz Ka�mierczak
  * @version V1.2
  * @date    25/07/2013
  * @brief   This header file provides parameters extracting operations set
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
#ifndef APP_PARAMS_H
#define APP_PARAMS_H

#include "common.h"


class app_parameters {
public:
    static app_parameters &instance() { static app_parameters ainstance; return ainstance; }

    const std::string &getPipeName()   const { return pipeName; }
    const UINT getTextSize()           const { return textSize; }
    const UINT getTextRepositionTime() const { return textRepositionTime; }
    const UINT getTextUpdateTime()     const { return textUpdateTime; }

private:
    app_parameters(): pipeName(UNNAMED), textSize(10), textRepositionTime(1000), textUpdateTime(500) {}
    app_parameters(const app_parameters&);
    app_parameters &operator=(const app_parameters&);

    std::string pipeName;
    UINT textSize;
    UINT textRepositionTime;
    UINT textUpdateTime;

    friend FunctionSuccesFailure get_main_parameters(INT argc, CHAR *argv[]);
};

FunctionSuccesFailure get_main_parameters(INT argc, CHAR *argv[]);


#endif
