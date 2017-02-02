// GetWifiPasswords.cpp : 
// ©2016 Secured Globe, Inc. (developed by Michael Haephrati)

#include "stdafx.h"
#include "GetWifiPasswords.h"
#pragma comment(lib, "wlanapi.lib")
#include <Wlanapi.h>
#define CRT_SECURE_NO_WARNINGS
#ifndef WLAN_PROFILE_GET_PLAINTEXT_KEY
#define WLAN_PROFILE_GET_PLAINTEXT_KEY 4 // Dont have the latest platform SDK on this box
#endif


#include <stdio.h>
#include <windows.h>
#include <wlanapi.h>
#include <stdint.h>
LPCWSTR utilsLogFilename = L"SG_WiFiPasswordList.txt";


#define LOG_COLOR_WHITE 7
#define LOG_COLOR_GREEN 10
#define LOG_COLOR_YELLOW 14 
#define LOG_COLOR_MAGENTA 13
#define LOG_COLOR_CIAN 11

void SetColor(int ForgC)
{
	WORD wColor;
	static int LastColor = -1;
	if (LastColor == ForgC) return;
	LastColor = ForgC;
	//This handle is needed to get the current background attribute

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//csbi is used for wAttributes word

	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		//To mask out all but the background attribute, and to add the color
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	return;
}

void WriteStatus(LPCTSTR lpText, ...)
{
	FILE *fp;
	CTime Today = CTime::GetCurrentTime();
	CString sMsg;
	CString sLine;
	va_list ptr;
	va_start(ptr, lpText);
	sMsg.FormatV(lpText, ptr);

	sLine.Format(L"%s",(LPCTSTR)sMsg);
	_wfopen_s(&fp, utilsLogFilename, L"a");
	if (fp)
	{
		fwprintf(fp, L"%s", sLine);
		fclose(fp);
	}
	wprintf(L"%s", sMsg);
}

BOOL IsElevated()
{
	DWORD dwSize = 0;
	HANDLE hToken = NULL;
	BOOL bReturn = FALSE;

	TOKEN_ELEVATION tokenInformation;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return FALSE;

	if (GetTokenInformation(hToken, TokenElevation, &tokenInformation, sizeof(TOKEN_ELEVATION), &dwSize))
	{
		bReturn = (BOOL)tokenInformation.TokenIsElevated;
	}

	CloseHandle(hToken);
	return bReturn;
}

CString StringToHexString(CString cs)
{
	CString ret, tmp;
	for (int i = 0; i<cs.GetLength(); ++i)
	{
		unsigned char c = cs[i];
		tmp.Format(L"%hX", c);
		ret += tmp;
	}
	return ret;
}
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);
	bool result = false;
	HANDLE hWlan;
	DWORD dwIgnored = 0;
	SetColor(LOG_COLOR_CIAN);
	if (!IsElevated()) WriteStatus(L"[!] Running without administrative rights\n");
	WriteStatus(L"WiFi Stored Credentials Report\nproduced by GetWifiData, by Secured Globe, Inc.\n\n");
	SetColor(LOG_COLOR_MAGENTA);
	WriteStatus(L"http://www.securedglobe.com\n\n\n");
	SetColor(LOG_COLOR_CIAN);
	DWORD dwResult = WlanOpenHandle(1, NULL, &dwIgnored, &hWlan);
	if (dwResult != ERROR_SUCCESS) return result;
	WLAN_INTERFACE_INFO_LIST* pWirelessAdapterList = NULL;
	dwResult = WlanEnumInterfaces(hWlan, NULL, &pWirelessAdapterList);
	if (dwResult != ERROR_SUCCESS)
	{
		WlanCloseHandle(hWlan, NULL);
		return result;
	}

	int nResCount = 1;
	WLAN_INTERFACE_INFO* pWirelessAdapterInfo = NULL;
	// Prepare data structure
	result = true;
	WriteStatus(L"Found network %d adaptors\n", pWirelessAdapterList->dwNumberOfItems+1);
	WriteStatus(L"WiFi Networks List\n\nDetailed report can be found is file ");
	SetColor(LOG_COLOR_YELLOW);
	WriteStatus(L"'%s'\n", utilsLogFilename);
	SetColor(LOG_COLOR_CIAN);
	WriteStatus(L"**************************************************\n");
	SetColor(LOG_COLOR_GREEN);
	system("pause");
	COORD newSize = { 100, 2500 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), newSize);

	HWND console = GetConsoleWindow();
	RECT r;
	int DesktopW, DesktopH;
	GetDesktopResolution(DesktopW, DesktopH);

	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, 10, DesktopW, DesktopH, TRUE);

	SetColor(LOG_COLOR_WHITE);

	for (DWORD i = 0; i < pWirelessAdapterList->dwNumberOfItems; i++)
	{
		pWirelessAdapterInfo = &pWirelessAdapterList->InterfaceInfo[i];

		WLAN_PROFILE_INFO_LIST* pProfileList;
		dwResult = WlanGetProfileList(hWlan, &pWirelessAdapterInfo->InterfaceGuid, NULL, &pProfileList);
		if (dwResult != ERROR_SUCCESS)
			continue;

		WriteStatus(L"Found %d items (Adaptor %d)\n\n", pProfileList->dwNumberOfItems,i+1);
		for (DWORD j = 0; j < pProfileList->dwNumberOfItems; j++)
		{
			WLAN_PROFILE_INFO* pProfileInfo = &pProfileList->ProfileInfo[j];

			LPWSTR lpszXmlProfile;
			DWORD dwFlags = WLAN_PROFILE_GET_PLAINTEXT_KEY | WLAN_PROFILE_USER;
			DWORD dwAccess = WLAN_READ_ACCESS;

			dwResult = WlanGetProfile(hWlan, &pWirelessAdapterInfo->InterfaceGuid, pProfileInfo->strProfileName, NULL,
				&lpszXmlProfile, &dwFlags, &dwAccess);
			if (dwResult == ERROR_SUCCESS)
			{
				CString strXml = lpszXmlProfile;
				WlanFreeMemory(lpszXmlProfile);

				// starting from the 'SSID' element
				int nFirstIndex = strXml.Find(L"<SSID>");
				int nLastIndex = strXml.Find(L"</SSID>");

				// extracting the Wifi network 'name' element 
				CString strSSID = CString(((LPCTSTR)strXml) + nFirstIndex, nLastIndex - nFirstIndex);
				nFirstIndex = strSSID.Find(L"<name>");
				nLastIndex = strSSID.Find(L"</name>");
				strSSID = CString(((LPCTSTR)strSSID) + nFirstIndex + 6, nLastIndex - (nFirstIndex + 6));

				// seeking for the 'Authentication' element
				nFirstIndex = strXml.Find(L"<authentication>");
				nLastIndex = strXml.Find(L"</authentication>");
				CString strAuth = CString(((LPCTSTR)strXml) + nFirstIndex + 16, nLastIndex - (nFirstIndex + 16));

				// seeking for the 'Encryption' element
				nFirstIndex = strXml.Find(L"<encryption>");
				nLastIndex = strXml.Find(L"</encryption>");
				CString strEnc = CString(((LPCTSTR)strXml) + nFirstIndex + 12, nLastIndex - (nFirstIndex + 12));

				// seeking for the 'keyMaterial' element
				CString strKey;
				nFirstIndex = strXml.Find(_T("<keyMaterial>"));
				if (nFirstIndex != -1)
				{
					nLastIndex = strXml.Find(_T("</keyMaterial>"));
					strKey = CString(((LPCTSTR)strXml) + nFirstIndex + 13, nLastIndex - (nFirstIndex + 13));

					// Decrypting the key
					BYTE byteKey[1024] = { 0 };
					DWORD dwLength = 1024;
					DATA_BLOB dataOut, dataVerify;

					BOOL bRes = CryptStringToBinary(strKey, strKey.GetLength(), CRYPT_STRING_HEX, byteKey, &dwLength, 0, 0);

					if (bRes)
					{
						dataOut.cbData = dwLength;
						dataOut.pbData = (BYTE*)byteKey;

						if (CryptUnprotectData(&dataOut, NULL, NULL, NULL, NULL, 0, &dataVerify))
						{
							TCHAR str[MAX_PATH] = { 0 };
							wsprintf(str, L"%hs", dataVerify.pbData);
							strKey = str;
						}
					}
				}

				CString strSpace;
				if (nResCount >= 1 && nResCount <= 9)
					strSpace = _T("   ");
				else if (nResCount >= 10 && nResCount <= 99)
					strSpace = _T("    ");
				else if (nResCount >= 100 && nResCount <= 999)
					strSpace = _T("     ");
				else if (nResCount >= 1000 && nResCount <= 9999)
					strSpace = _T("      ");
				else if (nResCount >= 10000 && nResCount <= 99999)
					strSpace = _T("       ");
				WCHAR network[1024];
				WCHAR user[1024];
				WCHAR pass[1024];
				wcsncpy_s(network, _countof(network), strSSID, _TRUNCATE);
				wcsncpy_s(user, _countof(user), strAuth, _TRUNCATE);
				wcsncpy_s(pass, _countof(pass), strKey, _TRUNCATE);
				SetColor(LOG_COLOR_YELLOW);
				WriteStatus(L"WiFi Name (SSID):\t\t\t\t %s\n", network);
				SetColor(LOG_COLOR_WHITE);
				WriteStatus(L"Security Settings:\t\t\t\t %s\n", wcscmp(user, L"open") ? L"WPA2PSK (AES)" : L"Open (No Password)");
				WriteStatus(L"User name:\t\t\t\t\t %s\n", user);
				WriteStatus(L"Password Type:\t\t\t\t\t %s\n",(wcscmp(pass,L"")?L"PassPhrase":L""));
				CString HexPassword = StringToHexString(pass);
				WriteStatus(L"Password (Hex):\t\t\t\t\t 0x%s\n", HexPassword);
				WriteStatus(L"Password:\t\t\t\t\t %s\n", pass);
				WriteStatus(L"-------------------------\n\n");

				nResCount++;
			}
			else
				WriteStatus(L"Error with this profile (%d)\n",j);
		}
		WlanFreeMemory(pProfileList);
	}
	WlanFreeMemory(pWirelessAdapterList);
	WlanCloseHandle(hWlan, NULL);
	CTime Today = CTime::GetCurrentTime();
	WriteStatus(L"Report created on %s\n\n\n", (LPCTSTR)Today.FormatGmt(L"%d.%m.%Y %H:%M"));

	system("pause");
	return 0;
}


