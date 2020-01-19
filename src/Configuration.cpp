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

#include "stdafx.h"
#include <atlbase.h>
#include <vector>
#include <shlobj.h>
#include "Configuration.h"
#include <locale>
#include <codecvt>

#define BUFSIZE 10240
DWORD cbToWrite, cbWritten, dwMode;

QvSpeedBar_DataSource::QvSpeedBar_DataSource()
{
    _pages.Pages.clear();
    static bool ole_initialized = false;
    if (!ole_initialized) {
        if (SUCCEEDED(CoInitialize(nullptr))) {
            ole_initialized = true;
        }
    }
    TryConnectPipe();
}

void QvSpeedBar_DataSource::SetSingleLineMessage(std::string msg) {
    QvBarPage page;
    QvBarLine line;
    line.Message = msg;
    page.Lines.push_back(line);
    page.OffsetYpx = 12;
    _pages.Pages.clear();
    _pages.Pages.push_back(page);
}

bool QvSpeedBar_DataSource::TryConnectPipe()
{
    if (hQv2rayServerPipe != NULL)
    {
        bool hResult = PeekNamedPipe(hQv2rayServerPipe, NULL, NULL, NULL, NULL, NULL);
        if (hResult) {
            return true;
        }
    }
    bool fSuccess = FALSE;
    while (1)
    {
        SetSingleLineMessage("未连接到 Qv2ray");
        hQv2rayServerPipe = CreateFile(lpszPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if (hQv2rayServerPipe != INVALID_HANDLE_VALUE)
            return true;

        if (GetLastError() != ERROR_PIPE_BUSY)
            return false;

        if (!WaitNamedPipe(lpszPipename, 5000))
            return false;
    }
    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(hQv2rayServerPipe, &dwMode, NULL, NULL);
    return fSuccess;

}
void QvSpeedBar_DataSource::LoopRead()
{
    DWORD  cbRead;
    char   chBuf[BUFSIZE * 2] = { 0 };
    BOOL   fSuccess = FALSE;

    while (!IsTerminating)
    {
        if (TryConnectPipe()) {
            LPCTSTR lpvMessage = _data.c_str();
            cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);

            fSuccess = WriteFile(hQv2rayServerPipe, lpvMessage, cbToWrite, &cbWritten, NULL);
            do
            {
                fSuccess = ReadFile(hQv2rayServerPipe, chBuf, BUFSIZE * sizeof(TCHAR), &cbRead, NULL);

                if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
                {
                    break;
                }

            } while (!fSuccess);
            auto rcvd = std::string(chBuf);
            _data = L"OK";

            QvNetSpeedBarConfig config;
            x2struct::X::loadjson(rcvd, config, false);
            _pages = config;
        }
        Sleep(500);
    }
}

DWORD WINAPI NamePipeThreadFunc(LPVOID data) {
    QvSpeedBar_DataSource* source = static_cast<QvSpeedBar_DataSource*>(data);
    source->LoopRead();
    return 0;
}

std::wstring utf8_to_utf16(const std::string& utf8)
{
    try
    {
        std::vector<unsigned long> unicode;
        size_t i = 0;
        while (i < utf8.size())
        {
            unsigned long uni;
            size_t todo;
            bool error = false;
            unsigned char ch = utf8[i++];
            if (ch <= 0x7F)
            {
                uni = ch;
                todo = 0;
            }
            else if (ch <= 0xBF)
            {
                throw std::logic_error("not a UTF-8 string");
            }
            else if (ch <= 0xDF)
            {
                uni = ch & 0x1F;
                todo = 1;
            }
            else if (ch <= 0xEF)
            {
                uni = ch & 0x0F;
                todo = 2;
            }
            else if (ch <= 0xF7)
            {
                uni = ch & 0x07;
                todo = 3;
            }
            else
            {
                throw std::logic_error("not a UTF-8 string");
            }
            for (size_t j = 0; j < todo; ++j)
            {
                // Should be j?
                if (i == utf8.size())
                    throw std::logic_error("not a UTF-8 string");
                unsigned char ch = utf8[i++];
                if (ch < 0x80 || ch > 0xBF)
                    throw std::logic_error("not a UTF-8 string");
                uni <<= 6;
                uni += ch & 0x3F;
            }
            if (uni >= 0xD800 && uni <= 0xDFFF)
                throw std::logic_error("not a UTF-8 string");
            if (uni > 0x10FFFF)
                throw std::logic_error("not a UTF-8 string");
            unicode.push_back(uni);
        }
        std::wstring utf16;
        for (size_t i = 0; i < unicode.size(); ++i)
        {
            unsigned long uni = unicode[i];
            if (uni <= 0xFFFF)
            {
                utf16 += (wchar_t)uni;
            }
            else
            {
                uni -= 0x10000;
                utf16 += (wchar_t)((uni >> 10) + 0xD800);
                utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
            }
        }
        return utf16;
    }
    catch (const std::exception & wex)
    {
        return utf8_to_utf16(wex.what());
    }
}