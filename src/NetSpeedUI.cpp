/*
    Copyright (C) 2019 Leroy.H.Y
    Copyright (C) 2008 Danny Couture

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Configuration.h"
#include <atlbase.h>
#include <unordered_map>
#include <vector>
#include <atlctl.h>
#include "NetSpeedUI.h"

HANDLE hWorkingThread;

STDMETHODIMP QvSpeedBar_UI::FinalConstruct()
{
    hWorkingThread = CreateThread(NULL, 0, NamePipeThreadFunc, &m_config, 0, NULL);
    if (hWorkingThread) {
    }
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::StartConnection()
{
    m_config._data = L"START";
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::RestartConnection()
{
    m_config._data = L"RESTART";
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::StopConnection()
{
    m_config._data = L"STOP";
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
{
    UNREFERENCED_PARAMETER(dwBandID);
    UNREFERENCED_PARAMETER(dwViewMode);
    HRESULT hr = E_FAIL;

    if (pdbi) {
        if (pdbi->dwMask & DBIM_MINSIZE) {
            pdbi->ptMinSize.x = 10;
            pdbi->ptMinSize.y = 10;
        }

        if (pdbi->dwMask & DBIM_MAXSIZE) {
            pdbi->ptMaxSize.x = -1;
            pdbi->ptMaxSize.y = -1;
        }

        if (pdbi->dwMask & DBIM_INTEGRAL) {
            pdbi->ptIntegral.x = 1;
            pdbi->ptIntegral.y = 1;
        }

        if (pdbi->dwMask & DBIM_ACTUAL) {
            pdbi->ptActual.x = 0;
            pdbi->ptActual.y = 0;
        }

        if (pdbi->dwMask & DBIM_MODEFLAGS) {
            pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT | DBIF_VIEWMODE_TRANSPARENT;
        }

        /*if(pdbi->dwMask & DBIM_BKCOLOR)
            pdbi->dwMask &= ~DBIM_BKCOLOR;*/

        hr = S_OK;
    }
    else {
        hr = E_INVALIDARG;
    }

    return hr;
}

STDMETHODIMP QvSpeedBar_UI::GetWindow(HWND* phWnd)
{
    HRESULT hr = S_OK;

    if (phWnd == nullptr) {
        hr = E_INVALIDARG;
    }
    else {
        *phWnd = m_hWnd;
        hr = S_OK;
    }

    return hr;
}

STDMETHODIMP QvSpeedBar_UI::ContextSensitiveHelp(BOOL bHelp)
{
    UNREFERENCED_PARAMETER(bHelp);
    return E_NOTIMPL;
}

STDMETHODIMP QvSpeedBar_UI::ShowDW(BOOL fShow)
{
    if (IsWindow()) {
        if (fShow) {
            ShowWindow(SW_SHOW);
        }
        else {
            ShowWindow(SW_HIDE);
        }
    }

    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::CloseDW(DWORD dwReserved)
{
    UNREFERENCED_PARAMETER(dwReserved);
    m_config.IsTerminating = true;
    //TerminateThread(hWorkingThread, 0);b
    ShowDW(FALSE);
    //CloseWindow(this->m_hWnd);
    if (IsWindow()) {
        DestroyWindow();
    }
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::ResizeBorderDW(LPCRECT prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
{
    UNREFERENCED_PARAMETER(prcBorder);
    UNREFERENCED_PARAMETER(punkToolbarSite);
    UNREFERENCED_PARAMETER(fReserved);
    return E_NOTIMPL;
}

STDMETHODIMP QvSpeedBar_UI::CanRenderComposited(BOOL* pfCanRenderComposited)
{
    *pfCanRenderComposited = TRUE;
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::SetCompositionState(BOOL fCompositionEnabled)
{
    m_fCompositionEnabled = fCompositionEnabled;
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::GetCompositionState(BOOL* pfCompositionEnabled)
{
    *pfCompositionEnabled = m_fCompositionEnabled;
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::SetSite(IUnknown* punkSite)
{
    HRESULT hr = E_FAIL;
    ATL::CComQIPtr<IOleWindow> spOleWindow;
    RECT rect = { 0 };
    HWND hWndParent = nullptr;

    if (punkSite != nullptr) {
        spOleWindow = punkSite;

        if (spOleWindow != nullptr) {
            hr = spOleWindow->GetWindow(&hWndParent);

            if (SUCCEEDED(hr)) {
                ::GetClientRect(hWndParent, &rect);
                Create(hWndParent, rect, nullptr, WS_CHILD);
                m_spInputObjSite = punkSite;
                hr = m_spInputObjSite ? S_OK : E_FAIL;
            }
        }
    }

    return hr;
}

STDMETHODIMP QvSpeedBar_UI::GetSite(REFIID riid, LPVOID* ppvReturn)
{
    HRESULT hr = E_FAIL;

    if (ppvReturn == nullptr) {
        hr = E_INVALIDARG;
    }
    else if (m_spInputObjSite != nullptr) {
        *ppvReturn = nullptr;
        hr = m_spInputObjSite->QueryInterface(riid, ppvReturn);
    }

    return hr;
}

STDMETHODIMP QvSpeedBar_UI::GetCommandString(UINT_PTR idCmd, UINT     uFlags, UINT* pwReserved, LPSTR    pszName, UINT     cchMax)
{
    UNREFERENCED_PARAMETER(idCmd);
    UNREFERENCED_PARAMETER(uFlags);
    UNREFERENCED_PARAMETER(pwReserved);
    UNREFERENCED_PARAMETER(pszName);
    UNREFERENCED_PARAMETER(cchMax);
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr = S_OK;

    if (HIWORD(pici->lpVerb) != 0) {
        hr = E_INVALIDARG;
    }
    else {
        switch (LOWORD(pici->lpVerb)) {
        case QV2RAY_STOP:
            hr = StopConnection();
            break;
        case QV2RAY_START:
            hr = StartConnection();
            break;
        case QV2RAY_RESTART:
            hr = RestartConnection();
            break;
        default:
            hr = E_INVALIDARG;
        }
    }

    return hr;
}

STDMETHODIMP QvSpeedBar_UI::QueryContextMenu(HMENU hMenu, UINT  indexMenu, UINT  idCmdFirst, UINT  idCmdLast, UINT  uFlags)
{
    UNREFERENCED_PARAMETER(idCmdLast);
    HRESULT hr = S_OK;

    if (CMF_DEFAULTONLY & uFlags) {
        hr = MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }
    else {
        InsertMenuW(hMenu, indexMenu, MF_SEPARATOR | MF_BYPOSITION, idCmdFirst, 0);
        InsertMenuW(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + QV2RAY_RESTART, L"Restart Connection");
        InsertMenuW(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + QV2RAY_STOP, L"Stop Connection");
        InsertMenuW(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + QV2RAY_START, L"Start Connection");

        hr = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 4);
    }

    return hr;
}

STDMETHODIMP QvSpeedBar_UI::GetClassID(LPCLSID pClassID)
{
    *pClassID = CLSID_QvNetSpeedBar;
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::IsDirty()
{
    return S_FALSE;
}

STDMETHODIMP QvSpeedBar_UI::Load(LPSTREAM pStream)
{
    UNREFERENCED_PARAMETER(pStream);
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::Save(LPSTREAM pStream, BOOL bClearDirty)
{
    UNREFERENCED_PARAMETER(pStream);
    UNREFERENCED_PARAMETER(bClearDirty);
    return S_OK;
}

STDMETHODIMP QvSpeedBar_UI::GetSizeMax(ULARGE_INTEGER* pcbSize)
{
    if (pcbSize == nullptr) {
        return E_INVALIDARG;
    }

    ULISet32(*pcbSize, 0);

    return S_OK;
}

LRESULT QvSpeedBar_UI::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);
    Invalidate();
    return 0;
}

LRESULT QvSpeedBar_UI::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);

    size_t pageCount = m_config.GetPages().size();
    if (pageCount <= 0) return 0;
    m_currentPage++;
    m_currentPage = m_currentPage % pageCount;
    Invalidate();

    return 0;
}

void QvSpeedBar_UI::PaintData(HDC hdc, POINT offset)
{
    auto pages = m_config.GetPages();
    if (pages.empty()) {
        return;
    }

    if (pages.size() <= m_currentPage) {
        m_currentPage = pages.size() - 1;
    }

    auto page = pages[m_currentPage];

    if (page.Lines.empty()) {
        return;
    }

    TEXTMETRIC textMetric;
    GetTextMetrics(hdc, &textMetric);

    wchar_t buf[1024] = { 0 };
    wchar_t display[1024] = { 0 };

    for (size_t i = 0; i < page.Lines.size(); ++i) {
        buf[0] = 0;
        auto line = page.Lines[i];
        //
        std::wstring _wsMessage;
        _wsMessage = utf8_to_utf16(line.Message);
        //
        swprintf_s(display, _countof(display), TEXT("%s\r\n"), _wsMessage.c_str());
        wcscat_s(buf, _countof(buf), display);

        RECT rc;
        GetClientRect(&rc);

        int offsetY = page.OffsetYpx + ((int)i * (textMetric.tmHeight - textMetric.tmExternalLeading));

        rc.top += offsetY + offset.y;
        rc.bottom += offset.y;
        rc.left += offset.x;
        rc.right += offset.x;

        wstring _wsFontFamily;
        _wsFontFamily = utf8_to_utf16(line.Family);

        HFONT font = CreateFont(-MulDiv((int)line.Size, GetDeviceCaps(hdc, LOGPIXELSY), 72),
            0, 0, 0,
            line.Bold ? FW_BOLD : 0,
            line.Italic,
            FALSE, FALSE, ANSI_CHARSET,
            0, 0, 0, 0,
            _wsFontFamily.c_str());

        COLORREF fontColorREF = RGB(line.ColorR, line.ColorG, line.ColorB);
        fontColorREF |= ((255 - line.ColorA) << 24);
        SetTextColor(hdc, fontColorREF);
        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        DrawText(hdc, buf, (int)wcslen(buf), &rc, DT_LEFT | DT_TOP);
        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }
}

void QvSpeedBar_UI::DestroyBar()
{
}

LRESULT QvSpeedBar_UI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);

    PAINTSTRUCT ps = { 0 };
    RECT        rect = { 0 };
    HDC         hdcMem = nullptr;
    HBITMAP     hbmMem = nullptr;
    HBITMAP     hbmOld = nullptr;
    HFONT       hfOld = nullptr;

    BeginPaint(&ps);

    HWND parent = GetParent();

    ::GetClientRect(parent, &rect);

    hdcMem = CreateCompatibleDC(ps.hdc);
    hbmMem = CreateCompatibleBitmap(ps.hdc, rect.right - rect.left, rect.bottom - rect.top);

    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    hfOld = (HFONT)SelectObject(hdcMem, m_font);

    SendMessage(parent, WM_ERASEBKGND, (WPARAM)hdcMem, 0);
    SendMessage(parent, WM_PAINT, (WPARAM)hdcMem, 0);

    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, 0x00FFFFFF);

    RECT rc;
    GetClientRect(&rc);
    ClientToScreen(&rc);

    POINT offset;
    offset.x = 0;
    offset.y = 0;

    ::ClientToScreen(parent, &offset);

    offset.x = rc.left - offset.x;
    offset.y = rc.top - offset.y;

    PaintData(hdcMem, offset);

    BitBlt(ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcMem, offset.x, offset.y, SRCCOPY);

    SelectObject(hdcMem, hfOld);
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);

    EndPaint(&ps);

    return 0;
}

LRESULT QvSpeedBar_UI::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);
    return TRUE;
}

LRESULT QvSpeedBar_UI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    bHandled = FALSE;

    HDC hDC = GetDC();

    m_font =
        CreateFontW(
            -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72),
            0,
            0,
            0,
            FW_BOLD,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            0,
            0,
            0,
            0,
            L"Î¢ÈíÑÅºÚ"
        );

    ReleaseDC(hDC);

    SetTimer(1000, 1000, nullptr);

    return 0;
}

LRESULT QvSpeedBar_UI::OnGoodBye(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    bHandled = FALSE;

    if (m_font) {
        DeleteObject(m_font);
        m_font = nullptr;
    }

    return 0;
}
