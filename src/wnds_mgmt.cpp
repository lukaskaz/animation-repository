/**
  ***************************************************************************************
  * @file    wnds_mgmt.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.3
  * @date    27/07/2013
  * @brief   This source file provides support for windows control operations
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
#include "wnds_mgmt.h"
#include <ctime>
#include <vector>
#include <Windows.h>

#include "stream.h"
#include "time_mgmt.h"
#include "process_mgmt.h"

#define MOD_WNDS_MGMT               "wnds_mgmt"

using namespace std;

static DWORD WINAPI closeWindowsProc(LPVOID lpParam);
static FunctionSuccesFailure closeOneWindow(window_info &wndToClose);
static INT isClassWndLast(const window_info &wnd);
static FunctionSuccesFailure waitWndExpiration(HWND hwnd);
static BOOL &getWndProcUpdateBlockade();
static FunctionSuccesFailure fontDefaultSettings(LOGFONT &fontDefaultStruct);
static INT fontFactorInit(UINT_PTR hWindowRaw);


/******************************************************************************
*
*               text decomposition class
*
******************************************************************************/
class decompose_text {
public:
    decompose_text(): component_vect(0) {}
    decompose_text(const string &text) { this->decompose_text::decompose_text(); tokenize(text); }

    LONG getMaxWidth()    const { return token_get_width_max(); }
    LONG getTotalHeight() const { return token_get_height_total(); }
    FunctionSuccesFailure getCoords(const string &line, SIZE &coords) const { return token_get_coord(line, coords); }

    FunctionSuccesFailure tokenize(const string &text);
    FunctionSuccesFailure peekNext(string &line);
    FunctionSuccesFailure getNext(string &line);
    FunctionSuccesFailure setCoords(const string &line, SIZE coords) { return token_set_coord(line, coords); }

private:
    struct component {
        component():sequneceNb(0), line(EMPTY_STRING) { coords.cx=0, coords.cy=0; }
        component(UINT nb, string &text):sequneceNb(nb), line(text) { coords.cx=0, coords.cy=0; }

        UINT sequneceNb;
        string line;
        SIZE coords;
    };
    vector<component> component_vect;

    LONG token_get_width_max()    const;
    LONG token_get_height_total() const;
    FunctionSuccesFailure token_get_coord(const string &line, SIZE &coords) const;

    FunctionSuccesFailure token_add(component token) { component_vect.insert(component_vect.begin(), token); return FUNCTION_SUCCESS; }
    FunctionSuccesFailure token_rotate(component &token);
    FunctionSuccesFailure token_remove(component &token);
    FunctionSuccesFailure token_set_coord(const string &line, SIZE coords);
};

FunctionSuccesFailure decompose_text::tokenize(const string &text) {
    string line = EMPTY_STRING;
    UINT sequenceNb = 1;

    for(UINT i=0, k=text.size(); i<k; ++i) {
        if(text[i] == NEW_LINE || i==(k-1)) {
            if(text[i] != NEW_LINE) {
                line += text[i];
            }
            token_add(component(sequenceNb, line));
            line = EMPTY_STRING;
            sequenceNb++;
        }
        else {
            line += text[i];
        }
    }
    token_add(component(sequenceNb, line));

    return FUNCTION_SUCCESS;
}

FunctionSuccesFailure decompose_text::peekNext(string &line) {
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    component token;
    if(token_rotate(token) != FUNCTION_FAILURE) {
        line = token.line;
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure decompose_text::getNext(string &line) {
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    component token;
    if(token_remove(token) != FUNCTION_FAILURE) {
        line = token.line;
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure decompose_text::token_rotate(component &token) 
{ 
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(component_vect.size() != 0) {
        token = component_vect.back(); 
        component_vect.pop_back(); 
        component_vect.insert(component_vect.begin(), token);

        if(token.sequneceNb != component_vect.size()) {
            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

FunctionSuccesFailure decompose_text::token_remove(component &token)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(component_vect.size() != 0) {
        token = component_vect.back();
        component_vect.pop_back();
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure decompose_text::token_set_coord(const string &line, SIZE coords)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    for(size_t i=0, k=component_vect.size(); i<k; ++i) {
        if(component_vect[i].line == line) {
            component_vect[i].coords = coords;
            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

FunctionSuccesFailure decompose_text::token_get_coord(const string &line, SIZE &coords) const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    for(size_t i=0, k=component_vect.size(); i<k; ++i) {
        if(component_vect[i].line == line) {
            coords = component_vect[i].coords;
            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

LONG decompose_text::token_get_width_max() const
{
    LONG maxWidth = 0;

    for(size_t i=0, k=component_vect.size(); i<k; ++i) {
        if(maxWidth < component_vect[i].coords.cx) {
            maxWidth = component_vect[i].coords.cx;
        }
    }

    return maxWidth;
}

LONG decompose_text::token_get_height_total() const
{
    LONG totalHeight = 0;

    for(size_t i=0, k=component_vect.size(); i<k; ++i) {
        totalHeight += component_vect[i].coords.cy;
    }

    return totalHeight;
}

/******************************************************************************
*
*               window info struct members
*
******************************************************************************/
window_info::window_info
(const string &cNameRaw, const string &tNameRaw, LRESULT (CALLBACK *wProc)(HWND, UINT, WPARAM, LPARAM), UINT sX, UINT sY, UINT wd, UINT ht)
{
    this->window_info::window_info();

    if(cNameRaw != UNNAMED) {
        wstring cNameWs = stringToWc(cNameRaw);
        LPCTSTR cName   = cNameWs.c_str();
        UINT cNameSize  = cNameWs.size()+SPACE_FOR_NULL;
        wClassName      = new WCHAR[cNameSize];

        if(wClassName != NULL) {
            copyWCharString(wClassName, cNameSize, cName);
        }
    }

    if(tNameRaw != UNNAMED) {
        wstring tNameWs = stringToWc(tNameRaw);
        LPCTSTR tName   = tNameWs.c_str();
        UINT tNameSize  = tNameWs.size()+SPACE_FOR_NULL;
        wTitleName      = new WCHAR[tNameSize];

        if(wTitleName != NULL) {
            copyWCharString(wTitleName, tNameSize, tName);
        }
    }

    WndProc = wProc; 
    start_X = sX; 
    start_Y = sY; 
    width  = wd; 
    height = ht;
}

window_info::window_info(const window_info &toCopy)
{
    this->window_info::window_info(wcToString(toCopy.wClassName), wcToString(toCopy.wTitleName),
                             toCopy.WndProc, toCopy.start_X, toCopy.start_Y, toCopy.width, toCopy.height);
    hwnd = toCopy.hwnd;
}

window_info &window_info::operator=(const window_info &toAssign)
{
    if(this != &toAssign) {
        this->window_info::~window_info();
        this->window_info::window_info(wcToString(toAssign.wClassName), wcToString(toAssign.wTitleName),
                                 toAssign.WndProc, toAssign.start_X, toAssign.start_Y, toAssign.width, toAssign.height);
        hwnd = toAssign.hwnd;
    }

    return *this;
}

window_info::window_info(HWND wndHwnd)
{
    static const UINT bufferSizeMax = 255+SPACE_FOR_NULL;
    WCHAR buffer[bufferSizeMax] = {0};
    UINT  wcTextSize            = 0;

    this->window_info::window_info();
    hwnd = wndHwnd;

    GetClassName(wndHwnd, buffer, bufferSizeMax);
    wcTextSize        = wcslen(buffer)+SPACE_FOR_NULL;
    wClassName        = new WCHAR[wcTextSize];

    if(wClassName != NULL) {
        copyWCharString(wClassName, wcTextSize, buffer);
    }

    GetWindowText(wndHwnd, buffer, bufferSizeMax);
    wcTextSize        = wcslen(buffer)+SPACE_FOR_NULL;
    wTitleName        = new WCHAR[wcTextSize];

    if(wTitleName != NULL) {
        copyWCharString(wTitleName, wcTextSize, buffer);
    }

}


/******************************************************************************
*
*               create window class members
*
******************************************************************************/
window_create::window_create(window_info wnd, deploy dmode, visibl vmode, style smode)
{
    this->window_create::window_create();

    new_window  = wnd;
    deploy_mode = dmode;
    visibl_mode = vmode;
    style_mode  = smode;
}

FunctionSuccesFailure window_create::validateData() const
{ 
    if( new_window.wClassName == NULL || new_window.wTitleName == NULL || new_window.WndProc == NULL) {
        return FUNCTION_FAILURE;
    }

    return FUNCTION_SUCCESS;
}

FunctionSuccesFailure window_create::registerClass() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    WNDCLASSEX wc = {0};
    HINSTANCE hInstance = GetModuleHandle(NULL);

    if( GetClassInfoEx(hInstance, new_window.wClassName, &wc) != 0 ) {
        fresult = FUNCTION_SUCCESS;
    }
    else {
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = 0;
        wc.lpfnWndProc   = new_window.WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor       = LoadCursor(NULL, IDC_WAIT);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = new_window.wClassName;
        wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

        if(RegisterClassEx(&wc) != 0) {
            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

FunctionSuccesFailure window_create::unregisterClass() const
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if(UnregisterClass(new_window.wClassName, hInstance) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

VOID window_create::messagePump() const
{
    BOOL inner_loop_failure = FALSE;
    BOOL outer_loop_failure = FALSE;
    INT bResult = FALSE;

    window_info wnd;
    do {
        MSG message = {0};
        bResult = GetMessage(&message, NULL, 0, 0);
        while(bResult) {
            if(bResult == -1) {
                inner_loop_failure = FUNCTION_FAILURE;
            }
            if(message.message == WM_CLOSE) {
                wnd = window_info(message.hwnd);
            }

            TranslateMessage( &message );
            DispatchMessage ( &message );
            bResult = GetMessage(&message, NULL, 0, 0);
        }
    }while( (outer_loop_failure = isClassWndLast(wnd)) == FALSE);

    if( inner_loop_failure == FUNCTION_FAILURE || outer_loop_failure == FUNCTION_FAILURE   ||
        waitWndExpiration(wnd.hwnd) == FUNCTION_FAILURE                                     ) {
            stream::error_message(MOD_WNDS_MGMT, "message pump killed, faults occured"); 
    }
    else {
        stream::system_message(MOD_WNDS_MGMT, "message pump killed");
    }

}

FunctionSuccesFailure window_create::create() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if( validateData() == FUNCTION_SUCCESS && registerClass() == FUNCTION_SUCCESS ) {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        DWORD styleFlags = WS_VISIBLE;
        switch(style_mode) {
            case WND_STY_NORMAL:
                break;
            case WND_STY_MAXIMIZED:
                styleFlags |= WS_MAXIMIZE;
                break;
            case WND_STY_NORMAL_POPUP:
                styleFlags |= WS_POPUP;
                break;
            case WND_STY_MAXIMIZED_POPUP:
                styleFlags |= (WS_MAXIMIZE|WS_POPUP);
                break;
            default:
            { /* no action defined */ }
        }

        HWND hwnd = CreateWindowEx(
                                    WS_EX_LAYERED, new_window.wClassName, new_window.wTitleName, styleFlags,
                                    new_window.start_X, new_window.start_Y, new_window.width, new_window.height,
                                    0, NULL, hInstance, NULL
                                  );

        if(hwnd != NULL) {
            stream::system_message(MOD_WNDS_MGMT, "window created, hWnd: "+decToHexString((UINT_PTR)hwnd));
            SetWindowLong(hwnd, GWL_WNDPROC, (LONG)new_window.WndProc);

            switch(visibl_mode) {
                case WND_VIS_TRANSPARENT_25:
                    SetLayeredWindowAttributes(hwnd, 0, 190, LWA_ALPHA);
                    break;
                case WND_VIS_TRANSPARENT_50:
                    SetLayeredWindowAttributes(hwnd, 0, 127, LWA_ALPHA);
                    break;
                case WND_VIS_TRANSPARENT_75:
                    SetLayeredWindowAttributes(hwnd, 0, 63, LWA_ALPHA);
                    break;
                case WND_VIS_OPAQUE:
                    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                    break;
                default:
                { /* no action defined */ }
            }

            switch(deploy_mode) {
                case WND_DEP_DEPLOY:
                    messagePump();
                    if(unregisterClass() == FUNCTION_SUCCESS) {
                        stream::system_message(MOD_WNDS_MGMT, "class unregistered");
                        fresult = FUNCTION_SUCCESS;
                    }
                    else {
                        stream::error_message(MOD_WNDS_MGMT, "class unregistering FAILED");
                    }
                    break;
                case WND_DEP_NOTDEPLOY:
                    fresult = FUNCTION_SUCCESS;
                    break;
                default:
                { /* no action defined */ }
            }
        }
    }

    return fresult;
}


/******************************************************************************
*
*               windows searching functions
*
******************************************************************************/
VOID window_search::startSearch() const
{
    EnumWindows( EnumWndProc, (LPARAM)(this) );
}

window_search::search_mode window_search::get_searchMode() const
{
    search_mode curr_mode = WND_SRCH_UNDEF;

    if( wndData.wClassName != NULL && wndData.wTitleName != NULL ) {
        curr_mode = WND_SRCH_TITLE_N_CLASS;
    }
    else if( wndData.wClassName == NULL && wndData.wTitleName != NULL ) {
        curr_mode = WND_SRC_TITLE;
    }
    else if( wndData.wClassName != NULL && wndData.wTitleName == NULL ) {
        curr_mode = WND_SRCH_CLASS;
    }
    else {
        curr_mode = WND_SRCH_ALL;
    }

    return curr_mode;
}

BOOL window_search::is_matched(window_info wnd) const
{
    BOOL match_result = FALSE;

    switch(get_searchMode()) {
        case WND_SRCH_ALL:
            match_result = TRUE;
            break;
        case WND_SRC_TITLE:
            if( is_titleSame(wnd.wTitleName) == TRUE ) {
                match_result = TRUE;
            }
            break;
        case WND_SRCH_CLASS:
            if( is_cNameSame(wnd.wClassName) == TRUE ) {
                match_result = TRUE;
            }
            break;
        case WND_SRCH_TITLE_N_CLASS:
            if( is_cNameSame(wnd.wClassName) == TRUE && is_titleSame(wnd.wTitleName) == TRUE ) {
                match_result = TRUE;
            }
            break;
        default: { /* no action determined */ }
    }

    return match_result;
}

window_info window_search::getExtracted()
{
    window_info first = window_info();
    if(isExtractedEmpty() != TRUE) {
        first = extracted.front();
        extracted.erase(extracted.begin());
    }

    return first;
}

/******************************************************************************
*
*               windows closing functions
*
******************************************************************************/
FunctionSuccesFailure closeWindows(window_info &wnd)
{
    UINT_PTR closeWndThread = processes::thread_create(closeWindowsProc, (LPVOID)&wnd, 0);
    if(closeWndThread != NULL) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

DWORD WINAPI closeWindowsProc(LPVOID lpParam)
{
    window_info wnd = *(window_info*)(lpParam);
    processes::thread_ready();

    window_search search(wnd);
    UINT closing_failures = 0;

    if(search.isExtractedEmpty() == TRUE)  {
        static const string allWndsName = "all[*]";
        string wTitle = wcToString(wnd.wTitleName);
        string wClass = wcToString(wnd.wClassName);

        if(wTitle == UNNAMED) {
            wTitle = allWndsName;
        }
        if(wClass == UNNAMED) {
            wClass = allWndsName;
        }

        stream::system_message(MOD_WNDS_MGMT, "Window of class:'"+wClass+"' and name:'"+wTitle+"' does not exist or already closed");
    }
    else {
        while(search.isExtractedEmpty() == FALSE) {
            if(closeOneWindow(search.getExtracted()) == FUNCTION_FAILURE) {
                closing_failures++;
            }
        }
    }

    if(closing_failures == 0) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

FunctionSuccesFailure closeOneWindow(window_info &wndToClose)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    PostMessage(wndToClose.hwnd, WM_CLOSE, 0, 0);
    if(waitWndExpiration(wndToClose.hwnd) == FUNCTION_SUCCESS) {
        stream::system_message(MOD_WNDS_MGMT, "Window[hwnd]: "+wcToString(wndToClose.wTitleName)
                               +" | "+wcToString(wndToClose.wClassName)
                               +" ["+decToHexString((DWORD)wndToClose.hwnd)+"] closed.");
        fresult = FUNCTION_SUCCESS;
    }
    else {
        stream::error_message(MOD_WNDS_MGMT, "CANNOT close window[hwnd]: "+wcToString(wndToClose.wTitleName)
                                +" | "+wcToString(wndToClose.wClassName)
                                +" ["+decToHexString((UINT)wndToClose.hwnd)+"]");
    }

    return fresult;
}

/******************************************************************************
*
*               windows text functions
*
******************************************************************************/
UINT_PTR const window_text::wndTextEvent = processes::event_create(FALSE, UNNAMED);

FunctionSuccesFailure fontDefaultSettings(LOGFONT &fontDefaultStruct)
{
    wstring defaultFont = stringToWc("Consolas");

    fontDefaultStruct.lfHeight      = 50;
    fontDefaultStruct.lfWidth       = 0;
    fontDefaultStruct.lfEscapement  = 0;
    fontDefaultStruct.lfOrientation = 0;
    fontDefaultStruct.lfWeight      = FW_NORMAL;
    fontDefaultStruct.lfItalic      = FALSE;
    fontDefaultStruct.lfUnderline   = FALSE;
    fontDefaultStruct.lfStrikeOut   = FALSE;
    fontDefaultStruct.lfQuality     = ANTIALIASED_QUALITY;
    fontDefaultStruct.lfCharSet        = DEFAULT_CHARSET;
    fontDefaultStruct.lfOutPrecision   = OUT_DEFAULT_PRECIS;
    fontDefaultStruct.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    fontDefaultStruct.lfPitchAndFamily = DEFAULT_PITCH;

    if(copyWCharString(fontDefaultStruct.lfFaceName, LF_FACESIZE-SPACE_FOR_NULL, defaultFont.c_str()) == FUNCTION_SUCCESS) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

INT fontFactorInit(UINT_PTR hWindowRaw)
{
    static const UINT fontDivFactor    = 72;
    static const INT  fontSignedFactor = -1;

    if(hWindowRaw == NULL) {
        hWindowRaw = (UINT_PTR)GetForegroundWindow();
    }

    HWND hWindow = (HWND)hWindowRaw;
    HDC  hdc     = GetDC(hWindow);
    UINT fontFactor = 0;

    SetMapMode(hdc, MM_TEXT);
    fontFactor = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hWindow, hdc);

    return fontSignedFactor*(fontFactor/fontDivFactor);
}

FunctionSuccesFailure window_text::invalidate()
{
    static const UINT invalidateMsTimeout    = 250;
    static const UINT invalidateMsAdditional = 5;
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    HWND hWindow = (HWND)hWindowRaw;

    InvalidateRect(hWindow, NULL, TRUE);
    processes::event_reset(wndTextEvent);
    if(processes::event_wait(wndTextEvent, invalidateMsTimeout) == FUNCTION_SUCCESS) {
        timers::timeout_create(invalidateMsAdditional);
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

UINT_PTR window_text::font_adjust(UINT size, BOOL bBold, BOOL bItalic, BOOL bUnderline)
{
    HFONT hFontNew       = NULL;
    UINT_PTR hFontNewRaw = NULL;
    LOGFONT  defaultFont  = {0};

    fontDefaultSettings(defaultFont);
    defaultFont.lfItalic = bItalic;
    defaultFont.lfUnderline = bUnderline;

    if(size != 0) {
        defaultFont.lfHeight = getFontConverteredSize(size);
    }
    if(bBold == TRUE) {
        defaultFont.lfWeight = FW_EXTRABOLD;
    }

    hFontNew = CreateFont(defaultFont.lfHeight, defaultFont.lfWidth,
                    defaultFont.lfEscapement, defaultFont.lfOrientation,
                    defaultFont.lfWeight, defaultFont.lfItalic,
                    defaultFont.lfUnderline, defaultFont.lfStrikeOut,
                    defaultFont.lfCharSet, defaultFont.lfOutPrecision,
                    defaultFont.lfClipPrecision, defaultFont.lfQuality,
                    defaultFont.lfPitchAndFamily, defaultFont.lfFaceName);

    hFontNewRaw = (UINT_PTR)hFontNew;
    return hFontNewRaw;
}

FunctionSuccesFailure
window_text::printText(UINT posX, UINT posY, UINT text_size, BOOL bBold, BOOL bItalic, BOOL bUnderline, const string &text)
{
    static UINT_PTR printTextMutex = processes::mutex_create(UNNAMED);

    processes::mutex_take_infinite(printTextMutex);

    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    HFONT hFontNew   = NULL;
    HWND  hWindow    = (HWND)hWindowRaw;

    hFontNew = (HFONT)font_adjust(text_size, bBold, bItalic, bUnderline);
    HDC hdc = GetDC(hWindow);

    if( (hdc != NULL)                            &&
        (SelectObject(hdc, hFontNew) != NULL)    &&
        (SetTextColor(hdc, 0x0) != CLR_INVALID)   )
    {
        decompose_text displayedText(text);

        if(invalidate() != FUNCTION_FAILURE) {
            UINT textPosX = posX;
            UINT textPosY = posY;
            UINT widthLimit  = 0;
            UINT heightLimit = 0;
            string line = EMPTY_STRING;

            getWindowDimentions((UINT_PTR)hWindow, widthLimit, heightLimit);
            while(displayedText.peekNext(line) != FUNCTION_FAILURE) {
                SIZE textDimentions  = {0};
                wstring wline = stringToWc(line);
                GetTextExtentPoint32(hdc, wline.c_str(), wline.size(), &textDimentions);
                displayedText.setCoords(line, textDimentions);
            }

            textPosX %= widthLimit  - displayedText.getMaxWidth() + 1;
            textPosY %= heightLimit - displayedText.getTotalHeight() + 1;

            while(displayedText.peekNext(line) != FUNCTION_FAILURE) {
                wstring wline = stringToWc(line);
                SIZE textDimentions = {0};
                TextOut(hdc, textPosX, textPosY, wline.c_str(), wline.size());
                displayedText.getCoords(line, textDimentions);
                textPosY += textDimentions.cy;
            }

            wtext = text_data(posX, posY, text_size, bBold, bItalic, bUnderline);
            fresult = FUNCTION_SUCCESS;
        }

        if(ReleaseDC(hWindow, hdc) == NULL) {
            stream::error_message(MOD_WNDS_MGMT, "DC device context not released");
            fresult = FUNCTION_FAILURE;
        }
    }

    processes::mutex_release(printTextMutex);
    return fresult;
}

FunctionSuccesFailure
window_text::printTextRandomPosition(UINT text_size, BOOL bBold, BOOL bItalic, BOOL bUnderline, const string &text)
{
    static FunctionSuccesFailure randomGeneratorInit = randomInit();
    return printText(rand(), rand(), text_size, bBold, bItalic, bUnderline, text);
}

FunctionSuccesFailure window_text::randomInit()
{
    srand((UINT)time(NULL));
    return FUNCTION_SUCCESS;
}

INT window_text::getFontConverteredSize(UINT size)
{
    static UINT frontScaleFactor = 0;
    static const UINT stepSize   = 3;
    if(frontScaleFactor == 0) {
        frontScaleFactor = fontFactorInit(hWindowRaw);
    }

    return stepSize*size*frontScaleFactor;
}


/******************************************************************************
*
*               window auxiliary functions
*
******************************************************************************/
INT isClassWndLast(const window_info &wnd)
{
    INT fresult = FUNCTION_FAILURE;

    if(*wnd.wClassName != NULL) {
        BOOL isWndExists = IsWindow(wnd.hwnd);
        window_search is_last(window_info(wcToString(wnd.wClassName)));

        if( (isWndExists == TRUE  && is_last.isExtractedLast()  == TRUE)  ||
            (isWndExists == FALSE && is_last.isExtractedEmpty() == TRUE)   ) {
                fresult = TRUE;
        }
        else {
            fresult = FALSE;
        }
    }

    return fresult;
}

FunctionSuccesFailure waitWndExpiration(HWND hwnd)
{
    static const UINT wndTermTimeoutMs = 20;

    if( IsWindow(hwnd) == FALSE                                                                  ||
       (timers::timeout_create(wndTermTimeoutMs) == FUNCTION_SUCCESS && IsWindow(hwnd) == FALSE)  ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

BOOL &getWndProcUpdateBlockade()
{
    static BOOL WndProcUpdateBlockade = FALSE;
    return WndProcUpdateBlockade;
}

BOOL isWndProcUpdateBlockadeActive()
{
    return getWndProcUpdateBlockade();
}

VOID setWndProcUpdateBlockade(BOOL blockadeState)
{
    BOOL &currentBlockadeStatus = getWndProcUpdateBlockade();
    currentBlockadeStatus = blockadeState;
}

/******************************************************************************
*
*               window exposure functions
*
******************************************************************************/
FunctionSuccesFailure bringConsoleForeground()
{
    HWND hConsole = GetConsoleWindow();

    if( hConsole != GetForegroundWindow() ) {
        SetForegroundWindow(hConsole);
        while( hConsole != GetForegroundWindow() ) {
            DWORD lockTimeOut    = 0;
            DWORD lockTimeOutNew = 0;
            DWORD dwThisTID      = GetCurrentThreadId();
            DWORD dwCurrTID      = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

            AttachThreadInput(dwThisTID, dwCurrTID, TRUE);

            SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOut, 0);
            SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOutNew, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

            AllowSetForegroundWindow(ASFW_ANY);
            SetForegroundWindow(hConsole);

            SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOut, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
            AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
        }
    }

    if( hConsole == GetForegroundWindow() ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure bringWindowForeground(const UINT_PTR hWndRaw)
{
    HWND hWnd = (HWND)hWndRaw;

    if( hWnd != NULL && hWnd != GetForegroundWindow() ) {
        SetForegroundWindow(hWnd);
        while( hWnd != GetForegroundWindow() ) {
            DWORD lockTimeOut    = 0;
            DWORD lockTimeOutNew = 0;
            DWORD dwThisTID      = GetCurrentThreadId();
            DWORD dwCurrTID      = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

            AttachThreadInput(dwThisTID, dwCurrTID, TRUE);

            SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOut, 0);
            SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOutNew, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

            AllowSetForegroundWindow(ASFW_ANY);
            SetForegroundWindow(hWnd);

            SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOut, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
            AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
        }
    }

    if( hWnd == GetForegroundWindow() ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure getMonitorDimentions(UINT &width, UINT &height)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    HWND hWnd = GetShellWindow();
    width  = 0;
    height = 0;

    if(hWnd != 0) {
        HMONITOR    monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO minfo   = {0}; 
        minfo.cbSize        = sizeof(MONITORINFO);

        if(GetMonitorInfo(monitor, &minfo) != 0) {
            if(minfo.rcMonitor.right > minfo.rcMonitor.left) {
                width  = minfo.rcMonitor.right - minfo.rcMonitor.left;
            }
            else {
                width  = minfo.rcMonitor.left - minfo.rcMonitor.right;
            }

            if(minfo.rcMonitor.bottom > minfo.rcMonitor.top) {
                height = minfo.rcMonitor.bottom - minfo.rcMonitor.top;
            }
            else {
                height = minfo.rcMonitor.top - minfo.rcMonitor.bottom;
            }

            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

FunctionSuccesFailure getWindowDimentions(UINT_PTR hWndRaw, UINT &width, UINT &height)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    width  = 0;
    height = 0;

    if(hWndRaw != 0) {
        HWND hWnd           = (HWND)hWndRaw;
        HMONITOR    monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO minfo   = {0}; 
        minfo.cbSize        = sizeof(MONITORINFO);

        if(GetMonitorInfo(monitor, &minfo) != 0) {
            if(minfo.rcMonitor.right > minfo.rcMonitor.left) {
                width  = minfo.rcMonitor.right - minfo.rcMonitor.left;
            }
            else {
                width  = minfo.rcMonitor.left - minfo.rcMonitor.right;
            }

            if(minfo.rcMonitor.bottom > minfo.rcMonitor.top) {
                height = minfo.rcMonitor.bottom - minfo.rcMonitor.top;
            }
            else {
                height = minfo.rcMonitor.top - minfo.rcMonitor.bottom;
            }

            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

