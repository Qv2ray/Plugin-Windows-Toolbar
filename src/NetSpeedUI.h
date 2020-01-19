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

#pragma once

#include <shlobj.h>
#include <atlctl.h>
#include "Configuration.h"

#include "Qv2ray_NetSpeedBar_i.h"

// QvSpeedBar_UI
class ATL_NO_VTABLE QvSpeedBar_UI :
    public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
    public ATL::CComCoClass<QvSpeedBar_UI, &CLSID_QvNetSpeedBar>,
    public ATL::IDispatchImpl <IQvNetSpeedBar, &IID_IQvNetSpeedBar, &LIBID_QvNetSpeedBarLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IObjectWithSite,
    public IPersistStream,
    public IDeskBand2,
    public IContextMenu,
    public ATL::CWindowImpl<QvSpeedBar_UI>
{
private:
    ATL::CComQIPtr<IInputObjectSite> m_spInputObjSite;
    HFONT         m_font;
    QvSpeedBar_DataSource m_config;
    size_t        m_currentPage;
    BOOL          m_fCompositionEnabled;

public:
    // Default Constructor.
    QvSpeedBar_UI() : m_font(nullptr), m_currentPage(0), m_fCompositionEnabled(FALSE) {}

    DECLARE_REGISTRY_RESOURCEID(IDR_QvNetSpeedBar)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(QvSpeedBar_UI)
        COM_INTERFACE_ENTRY(IQvNetSpeedBar)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(IObjectWithSite)
        COM_INTERFACE_ENTRY(IDeskBand)
        COM_INTERFACE_ENTRY(IDeskBand2)
        COM_INTERFACE_ENTRY(IPersist)
        COM_INTERFACE_ENTRY(IPersistStream)
        COM_INTERFACE_ENTRY(IDockingWindow)
        COM_INTERFACE_ENTRY(IOleWindow)
        COM_INTERFACE_ENTRY(IContextMenu)
    END_COM_MAP()

    BEGIN_CATEGORY_MAP(QvSpeedBar_UI)
        IMPLEMENTED_CATEGORY(CATID_DeskBand)
        IMPLEMENTED_CATEGORY(CATID_CommBand)
    END_CATEGORY_MAP()

    STDMETHOD(FinalConstruct)();

    ///////////////////////////////////////////////////////////////
    // IDeskBand methods

    STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);
    STDMETHOD(GetWindow)(HWND* phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL bEnterMode);

    STDMETHOD(ShowDW)(BOOL bShow);
    STDMETHOD(CloseDW)(DWORD dwReserved);
    STDMETHOD(ResizeBorderDW)(LPCRECT prcBorder, IUnknown* punkToolbarSite, BOOL bReserved);

    STDMETHOD(CanRenderComposited)(BOOL* pfCanRenderComposited);
    STDMETHOD(SetCompositionState)(BOOL fCompositionEnabled);
    STDMETHOD(GetCompositionState)(BOOL* pfCompositionEnabled);

    ///////////////////////////////////////////////////////////////
    // IObjectWithSite methods

    STDMETHOD(SetSite)(IUnknown* pUnkSite);
    STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

    ///////////////////////////////////////////////////////////////
    // IPersistStream methods

    STDMETHOD(GetClassID)(LPCLSID pClassID);
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)(LPSTREAM pStream);
    STDMETHOD(Save)(LPSTREAM pStream, BOOL bClearDirty);
    STDMETHOD(GetSizeMax)(ULARGE_INTEGER* pcbSize);

    ////////////////////////////////////////////////////////////////
    // IContext Menu Methods

    STDMETHOD(GetCommandString)(UINT_PTR idCmd, UINT     uFlags, UINT* pwReserved, LPSTR    pszName, UINT     cchMax);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO pici);
    STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT  indexMenu, UINT  idCmdFirst, UINT  idCmdLast, UINT  uFlags);

    STDMETHOD(StopConnection)();
    STDMETHOD(StartConnection)();
    STDMETHOD(RestartConnection)();

    ///////////////////////////////////////////////////////////////
    // Windows Message Map

    BEGIN_MSG_MAP(QvSpeedBar_UI)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnGoodBye)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    END_MSG_MAP()


    ////////////////////////////////////////////////////////////////
    // Message Handlers

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnGoodBye(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    /////////////////////////////////////////////////////////////////
    // QvSpeedBar_UI Methods
    void PaintData(HDC hdc, POINT offset);
    void DestroyBar();
};

OBJECT_ENTRY_AUTO(__uuidof(QvNetSpeedBar), QvSpeedBar_UI)
