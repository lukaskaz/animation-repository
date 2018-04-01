/**
  ***************************************************************************************
  * @file    app_params.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    25/07/2013
  * @brief   This source file provides support for parameters extracting operations
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

