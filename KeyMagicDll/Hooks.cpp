#include "Hooks.h"
#include "shortcut.h"
#include "InputProcessor.h"

//HKL hkl;
//char KLID[KL_NAMELENGTH];

LRESULT CALLBACK HookKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	int index;

	if (nCode < 0){
		return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
	}

	//Key Up
	if (nCode == HC_ACTION && lParam & 0x80000000)
	{
#ifdef _DEBUG
		Debug(L"HookKeyProc::Key Up\nlParam = 0x%.8x wParam = 0x%.8x\n", lParam, wParam);
#endif
		return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
	}
	//Menu Mode is active
	if (nCode == HC_ACTION && (lParam & 0x10000000)){
		Debug(L"Menu Mode ACTIVE\n");
		return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
	}
	//Key Down
	else if (nCode == HC_ACTION && lParam)
	{
#ifdef _DEBUG
		Debug(L"HookKeyProc::Key Down\nlParam = 0x%.8x wParam = 0x%.8x\n", lParam, wParam);
#endif
		//If there is no focus
		if (!GetFocus())
			return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

		// If ActiveWindow is Keymagic Application
		if(GetActiveWindow() == hwndKWindows)
			return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

		index = ShortCutCheck(wParam);

		if (index >= 0){
			if ( (ActiveIndex == index && isActive) || index == 0) {
				isActive = false;
				PostMessage(hwndKWindows, KM_GETFOCUS, 0, 0);
				//UnloadKeyboardLayout(hkl);
				//if (KLID[0])
				//	LoadKeyboardLayout(KLID, KLF_ACTIVATE | KLF_SETFORPROCESS );
			}
			else {
				isActive = true;
				ActiveIndex = index;
				LoadKeymapFile(ActiveIndex);
				InternalEditor.Restart();
				PostMessage(hwndKWindows, KM_GETFOCUS, ActiveIndex, 0);
				//GetKeyboardLayoutName(KLID);
				//hkl = LoadKeyboardLayout("00000409", KLF_ACTIVATE | KLF_SETFORPROCESS);
			}
			return 1;
		}

		if (isActive == false)
			return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

		if (ProcessInput(wParam, lParam))
			return true;
	}
	return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

}

LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	char Name[50];

	if (nCode < 0)
		return CallNextHookEx(hWndProcHook, nCode, wParam, lParam);

	CWPSTRUCT* cwp = (CWPSTRUCT*)lParam;
	
	switch (cwp->message)
	{
	case WM_ACTIVATE:
		if (LOWORD(cwp->wParam) == WA_INACTIVE){

			if (cwp->hwnd == hwndKWindows)
				break;

			GetClassName(cwp->hwnd, (LPSTR)Name, 50);

			if (!lstrcmp(Name,"Shell_TrayWnd"))
				break;
			if (!lstrcmp(Name, "TrayNotifyWnd"))
				break;

			PostMessage(hwndKWindows, KM_KILLFOCUS, 0,(LPARAM) cwp->hwnd);
#ifdef _TRACK_WND_RPOC_
			InternalEditor.Restart();
#endif
		}

		if (LOWORD(cwp->wParam) == WA_ACTIVE){

			if (cwp->hwnd == hwndKWindows)
				break;

			GetClassName(cwp->hwnd, (LPSTR)Name, 50);
			
			if (!lstrcmp(Name,"Shell_TrayWnd"))
				break;
			if (!lstrcmp(Name, "TrayNotifyWnd"))
				break;

			if (isActive)
				PostMessage(hwndKWindows, KM_GETFOCUS, ActiveIndex,(LPARAM) cwp->hwnd);
			else if (!isActive)
				PostMessage(hwndKWindows, KM_GETFOCUS, 0, (LPARAM) cwp->hwnd);
		}
		break;

	case KM_SETKBID:
		isActive = cwp->lParam;

		if (isActive) {
			if (cwp->wParam == ActiveIndex) {
				isActive = false;
				ActiveIndex = -1;
			}
			else
			{
				ActiveIndex = cwp->wParam;
				LoadKeymapFile(ActiveIndex);
				InternalEditor.Restart();
			}
		}
		else
			ActiveIndex = -1;

		break;

	case KM_RESCAN:
		vtSC.clear();
		GetShortCuts();
	}

	return CallNextHookEx(hWndProcHook, nCode, wParam, lParam);
}


LRESULT CALLBACK HookGetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hGetMsgHook, nCode, wParam, lParam);

	MSG* msg = (MSG*)lParam;
	switch (msg->message)
	{
#ifdef _TRACK_WND_RPOC_
		case WM_KEYDOWN:
			if (isActive)
				InternalEditor.KeyDown(msg->wParam, msg->lParam);
			break;
		case WM_KEYUP:
			if (isActive)
				InternalEditor.KeyUp(msg->wParam, msg->lParam);
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				if (isActive)
					InternalEditor.Restart();
				break;
			}
			break;
#endif
	}

	return CallNextHookEx(hGetMsgHook, nCode, wParam, lParam);
}