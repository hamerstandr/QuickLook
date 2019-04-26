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

#pragma once
class DialogHook
{
public:
	static void GetSelected(PWCHAR buffer);

private:
	static void getSelectedInternal(CComPtr<IShellBrowser> psb, PWCHAR buffer);
	static void GetSelectedFromWoW64HookHelper(PWCHAR buffer);
	static HMODULE ModuleFromAddress(PVOID pv);
	static LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wParam, LPARAM lParam);
};
