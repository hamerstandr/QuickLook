﻿// Copyright © 2017 Paddy Xu
// 
// This file is part of QuickLook program.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "HelperMethods.h"

void HelperMethods::GetSelectedInternal(CComQIPtr<IWebBrowserApp> pwba, PWCHAR buffer)
{
	CComQIPtr<IServiceProvider> psp;
	if (FAILED(pwba->QueryInterface(IID_IServiceProvider, reinterpret_cast<void**>(&psp))))
		return;

	CComPtr<IShellBrowser> psb;
	if (FAILED(psp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, reinterpret_cast<LPVOID*>(&psb))))
		return;

	CComPtr<IShellView> psv;
	if (FAILED(psb->QueryActiveShellView(&psv)))
		return;

	CComPtr<IDataObject> dao;
	if (FAILED(psv->GetItemObject(SVGIO_SELECTION, IID_IDataObject, reinterpret_cast<void**>(&dao))))
		return;

	return ObtainFirstItem(dao, buffer);
}

void HelperMethods::ObtainFirstItem(CComPtr<IDataObject> dao, PWCHAR buffer)
{
	FORMATETC formatetc;
	STGMEDIUM medium = {sizeof medium};

	formatetc.cfFormat = CF_HDROP;
	formatetc.ptd = nullptr;
	formatetc.dwAspect = DVASPECT_CONTENT;
	formatetc.lindex = -1;
	formatetc.tymed = TYMED_HGLOBAL;

	medium.tymed = TYMED_HGLOBAL;

	if (FAILED(dao->GetData(&formatetc, &medium)))
		return;

	int n = DragQueryFile(HDROP(medium.hGlobal), 0xFFFFFFFF, nullptr, 0);

	if (n < 1)
		return;

	DragQueryFile(HDROP(medium.hGlobal), 0, buffer, MAX_PATH - 1);
}

bool HelperMethods::IsListaryToolbarVisible()
{
	auto CALLBACK findListaryWindowProc = [](__in HWND hwnd, __in LPARAM lParam)-> BOOL
	{
		WCHAR classBuffer[MAX_PATH] = {'\0'};
		if (FAILED(GetClassName(hwnd, classBuffer, MAX_PATH)))
			return TRUE;

		if (wcsncmp(classBuffer, L"Listary_WidgetWin_", 18) == 0)
		{
			if (IsWindowVisible(hwnd))
			{
				*reinterpret_cast<bool*>(lParam) = true;
				return FALSE;
			}
		}
		return TRUE;
	};

	auto found = false;
	EnumWindows(findListaryWindowProc, reinterpret_cast<LPARAM>(&found));

	return found;
}

bool HelperMethods::IsCursorActivated(HWND hwnd)
{
	auto tId = GetWindowThreadProcessId(hwnd, nullptr);

	GUITHREADINFO gui = {sizeof gui};
	GetGUIThreadInfo(tId, &gui);
	return gui.flags || gui.hwndCaret || IsListaryToolbarVisible();
}

bool HelperMethods::IsUWP()
{
	auto pGCPFN = decltype(&GetCurrentPackageFullName)(
		GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetCurrentPackageFullName"));

	if (!pGCPFN)
		return false;

	UINT32 pn = 0;
	return pGCPFN(&pn, nullptr) == ERROR_INSUFFICIENT_BUFFER;
}
