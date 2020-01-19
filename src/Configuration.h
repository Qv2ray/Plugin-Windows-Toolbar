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

#include "stdafx.h"
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"
#include <unordered_map>
#include <vector>
#include <stdlib.h>

#include <iostream>
#include <locale>
#include <cwchar>
#include <stdlib.h>
#include <string>
#include <codecvt>

#include <string>
#include "../3rdparty/x2struct/x2struct.hpp"

using namespace x2struct;

constexpr UINT QV2RAY_START = 1;
constexpr UINT QV2RAY_STOP = 2;
constexpr UINT QV2RAY_RESTART = 3;
// #define IDR_PERFMONBAR                  101
#define IDR_QvNetSpeedBar                  102

DWORD WINAPI NamePipeThreadFunc(LPVOID data);
std::wstring utf8_to_utf16(const std::string& utf8);

struct QvBarLine {
    std::string  Family;
    bool         Bold;
    bool         Italic;
    int ColorA;
    int ColorR;
    int ColorG;
    int ColorB;
    double       Size;
    std::string        Message;
    QvBarLine()
        : Family("微软雅黑")
        , Bold(true)
        , Italic(false)
        , ColorA(255), ColorR(255), ColorG(255), ColorB(255)
        , Size(8.5),
        Message() {
    }
    XTOSTRUCT(O(Bold, Italic, ColorA, ColorR, ColorG, ColorB, Size, Family, Message))
};

struct QvBarPage {
    int                    OffsetYpx;
    std::vector<QvBarLine> Lines;
    QvBarPage() : OffsetYpx(0) {}
    XTOSTRUCT(O(OffsetYpx, Lines))
};

struct QvNetSpeedBarConfig {
    std::vector<QvBarPage> Pages;
    XTOSTRUCT(O(Pages))
};

class QvSpeedBar_DataSource
{
public:
    explicit QvSpeedBar_DataSource();
    void LoopRead();
    bool TryConnectPipe();
    void SetSingleLineMessage(std::string msg);
    std::vector<QvBarPage>& GetPages()
    {
        return _pages.Pages; 
    } 
    std::wstring _data = L"OK";
    bool IsTerminating = false;

private:
    HANDLE					hQv2rayServerPipe;
    QvNetSpeedBarConfig		_pages;
    LPTSTR		            lpszPipename = TEXT("\\\\.\\pipe\\qv2ray_desktop_netspeed_toolbar_pipe");
};
