/**
  ***************************************************************************************
  * @file    app_params.cpp
  * @author  �ukasz Ka�mierczak
  * @version V1.2
  * @date    25/07/2013
  * @brief   This source file provides support for parameters extracting operations
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
#include "app_params.h"

#define MOD_APP_PARAMS               "app_params"

using namespace std;


FunctionSuccesFailure get_main_parameters(INT argc, CHAR *argv[])
{
    static const CHAR *pipeNameParam = "-p";
    static const CHAR *textSizeParam = "-s";
    static const CHAR *textRepositionTimeParam = "-rp";
    static const CHAR *textUpdateTimeParam     = "-u";

    for(INT i=0; i<argc; ++i) {
        if(strncmp(argv[i], pipeNameParam, strlen(argv[i])) == 0) {
            app_parameters::instance().pipeName = argv[i+1];
        }
        else if(strncmp(argv[i], textSizeParam, strlen(argv[i])) == 0) {
            app_parameters::instance().textSize = strtoul(argv[i+1], NULL, 0);
        }
        else if(strncmp(argv[i], textRepositionTimeParam, strlen(argv[i])) == 0) {
            app_parameters::instance().textRepositionTime = strtoul(argv[i+1], NULL, 0);
        }
        else if(strncmp(argv[i], textUpdateTimeParam, strlen(argv[i])) == 0) {
            app_parameters::instance().textUpdateTime = strtoul(argv[i+1], NULL, 0);
        }
        else {
            /* unexpected process parameter occurred */
        }
     }

    return FUNCTION_SUCCESS;
}

