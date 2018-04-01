/**
  ***************************************************************************************
  * @file    wnds_mgmt.h
  * @author  £ukasz Kaümierczak
  * @version V1.3
  * @date    27/07/2013
  * @brief   This header file provides windows control operations set
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
#ifndef WNDS_MGMT_H
#define WNDS_MGMT_H

#include <vector>
#include "common.h"
#include "wnds_proc.h"


struct window_info {
public:
    window_info(): hwnd(0), start_X(0), start_Y(0), width(0), height(0) { wClassName = NULL; wTitleName = NULL; WndProc = NULL; }
    window_info(const std::string &, const std::string &, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM), UINT, UINT, UINT, UINT);
    window_info(const std::string &cName, const std::string &tName) { this->window_info::window_info(cName, tName, NULL, 0, 0, 0, 0); }
    window_info(const std::string &cName) { this->window_info::window_info(cName, UNNAMED, NULL, 0, 0, 0, 0); }
    window_info(HWND wndHwnd);
    ~window_info() { delete wClassName; wClassName = NULL; delete wTitleName; wTitleName = NULL; }

    WCHAR *wClassName;
    WCHAR *wTitleName;
    LRESULT (CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM);
    UINT  start_X;
    UINT  start_Y;
    UINT  width;
    UINT  height;
    HWND  hwnd;

    window_info(const window_info &toCopy);
    window_info &operator=(const window_info &toAssign);
};

class window_create {
public:
    static const enum deploy { WND_DEP_UNDEF=0, WND_DEP_DEPLOY, WND_DEP_NOTDEPLOY,
                               WND_DEP_DEFAULT = WND_DEP_DEPLOY };
    static const enum visibl { WND_VIS_UNDEF=0, WND_VIS_TRANSPARENT_25, WND_VIS_TRANSPARENT_50, WND_VIS_TRANSPARENT_75, WND_VIS_OPAQUE,
                               WND_VIS_DEFAULT = WND_VIS_OPAQUE };
    static const enum style  { WND_STY_UNDEF=0, WND_STY_MAXIMIZED, WND_STY_MAXIMIZED_POPUP, WND_STY_NORMAL, WND_STY_NORMAL_POPUP,
                               WND_STY_DEFAULT = WND_STY_MAXIMIZED };

    window_create(window_info wnd, deploy dmode, visibl vmode, style smode);
    window_create(window_info wnd) { this->window_create::window_create(wnd, WND_DEP_DEFAULT, WND_VIS_DEFAULT, WND_STY_DEFAULT); }

    FunctionSuccesFailure create() const;
window_info new_window;
private:
    window_create(): new_window(window_info()), deploy_mode(WND_DEP_DEFAULT), visibl_mode(WND_VIS_DEFAULT), style_mode(WND_STY_DEFAULT) {}

    FunctionSuccesFailure validateData()    const;
    FunctionSuccesFailure registerClass()   const;
    FunctionSuccesFailure unregisterClass() const;
    VOID messagePump()                      const;

    deploy   deploy_mode;
    visibl   visibl_mode;
    style    style_mode;
};

class window_search {
public:
    static const enum search_mode{ WND_SRCH_UNDEF=0, WND_SRCH_ALL, WND_SRC_TITLE, WND_SRCH_CLASS, WND_SRCH_TITLE_N_CLASS };
    window_search(): extracted(0), wndData() {}
    window_search(const window_info &wnd) { this->window_search::window_search(); wndData=wnd;
                                            if(wndData.hwnd==0) startSearch(); else insert_matched(wnd); }

    window_info getExtracted();
    BOOL isExtractedEmpty()           const { if(extracted.size() == 0) return TRUE; return FALSE; }
    BOOL isExtractedLast()            const { if(extracted.size() == 1) return TRUE; return FALSE; }
    VOID analyze_element(window_info wnd) { if(is_matched(wnd) == TRUE) insert_matched(wnd); }
    HWND getWndHandle(const window_info &wnd) { if( *wnd.wTitleName == NULL || *wnd.wClassName == NULL) return 0;
                                             this->window_search::window_search(wnd); return getExtracted().hwnd; }

private:
    window_info wndData;
    std::vector<window_info> extracted;

    VOID  startSearch()               const;
    search_mode get_searchMode()      const;
    BOOL  is_matched(window_info wnd) const;
    BOOL  is_cNameSame(LPTSTR cname)  const { return isWcSame(cname, wndData.wClassName); }
    BOOL  is_titleSame(LPTSTR tname)  const { return isWcSame(tname, wndData.wTitleName); }
    VOID  insert_matched(window_info wnd) { extracted.push_back(wnd); };
};

class window_text {
public:
    window_text(): hWindowRaw(NULL), wtext(text_data()) {}
    window_text(UINT_PTR hWndRaw) { this->window_text::window_text(); hWindowRaw = hWndRaw; }
    static const UINT_PTR wndTextEvent;

    FunctionSuccesFailure printText(UINT posX, UINT posY, UINT text_size, BOOL bBold, BOOL bItalic, BOOL bUnderline, const std::string &text);
    FunctionSuccesFailure printText(UINT posX, UINT posY, UINT text_size, const std::string &text) 
        { return printText(posX, posY, text_size, FALSE, FALSE, FALSE, text); }
    FunctionSuccesFailure printTextRandomPosition(UINT text_size, BOOL bBold, BOOL bItalic, BOOL bUnderline, const std::string &text);
    FunctionSuccesFailure printTextRandomPosition(UINT text_size, const std::string &text)
        { return printTextRandomPosition(text_size, FALSE, FALSE, FALSE, text); }
    FunctionSuccesFailure printTextUpdate(const std::string &text)
    { if(wtext.textSize != 0) { return printText(wtext.posX, wtext.posY, wtext.textSize, wtext.bBold, wtext.bItalic, wtext.bUnderline, text); }
      return FUNCTION_FAILURE; }

private:
    UINT_PTR hWindowRaw;
    struct text_data{
        text_data():posX(0), posY(0), textSize(0), bBold(FALSE), bItalic(FALSE), bUnderline(FALSE) {}
        text_data(UINT pX, UINT pY, UINT tSize, BOOL bold, BOOL italic, BOOL uline)
            :posX(pX), posY(pY), textSize(tSize), bBold(bold), bItalic(italic), bUnderline(uline) {}
        UINT posX;
        UINT posY;
        UINT textSize;
        BOOL bBold;
        BOOL bItalic;
        BOOL bUnderline;
    }wtext;

    FunctionSuccesFailure randomInit();
    FunctionSuccesFailure invalidate();
    INT getFontConverteredSize(UINT size);
    UINT_PTR font_adjust(UINT size, BOOL bBold, BOOL bItalic, BOOL bUnderline);
};

BOOL isWndProcUpdateBlockadeActive();
VOID setWndProcUpdateBlockade(BOOL blockadeState);
FunctionSuccesFailure closeWindows(window_info &wnd);
FunctionSuccesFailure bringConsoleForeground();
FunctionSuccesFailure bringWindowForeground(const UINT_PTR hwnd);
FunctionSuccesFailure getMonitorDimentions(UINT &width, UINT &height);
FunctionSuccesFailure getWindowDimentions(UINT_PTR hWndRaw, UINT &width, UINT &height);


#endif
