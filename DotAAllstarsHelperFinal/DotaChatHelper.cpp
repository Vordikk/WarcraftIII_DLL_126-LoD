#include "Main.h"
#include <cctype>



#define MAX_CHAT_MSG_LEN 128

unsigned char* GetChatOffset()
{
	unsigned char* pclass = *(unsigned char**)pW3XGlobalClass;
	if (pclass)
	{
		return *(unsigned char**)(pclass + 0x3FC);
	}

	return 0;
}




char* GetChatString()
{
	unsigned char* pChatOffset = GetChatOffset();
	if (pChatOffset)
	{
		pChatOffset = *(unsigned char**)(pChatOffset + 0x1E0);
		if (pChatOffset)
		{
			pChatOffset = *(unsigned char**)(pChatOffset + 0x1E4);
			return (char*)pChatOffset;
		}
	}
	return 0;
}

LPARAM lpReturnScanKeyUP = (LPARAM)(0xC0000001 | (LPARAM)(MapVirtualKey(VK_RETURN, 0) << 16));
LPARAM lpReturnScanKeyDOWN = (LPARAM)(0x00000001 | (LPARAM)(MapVirtualKey(VK_RETURN, 0) << 16));



LPARAM lpRShiftScanKeyUP = (LPARAM)(0xC0000001 | (LPARAM)(MapVirtualKey(VK_RSHIFT, 0) << 16));
LPARAM lpRShiftScanKeyDOWN = (LPARAM)(0x00000001 | (LPARAM)(MapVirtualKey(VK_RSHIFT, 0) << 16));

LPARAM lpShiftScanKeyUP = (LPARAM)(0xC0000001 | (LPARAM)(MapVirtualKey(VK_SHIFT, 0) << 16));
LPARAM lpShiftScanKeyDOWN = (LPARAM)(0x00000001 | (LPARAM)(MapVirtualKey(VK_SHIFT, 0) << 16));

LPARAM lpLShiftScanKeyUP = (LPARAM)(0xC0000001 | (LPARAM)(MapVirtualKey(VK_LSHIFT, 0) << 16));
LPARAM lpLShiftScanKeyDOWN = (LPARAM)(0x00000001 | (LPARAM)(MapVirtualKey(VK_LSHIFT, 0) << 16));






pGameChatSetState GameChatSetState;


typedef int(__fastcall* pGameChatSendMessage)(int GlobalGlueObjAddr, int zero, unsigned char* event_vtable);
pGameChatSendMessage GameChatSendMessage;

pSetChatTargetUsers pSetChatTargetUsers_org;
pSetChatTargetUsers pSetChatTargetUsers_ptr;

int UsingCustomChatTarget = false;

int CustomChatTarget = 0;

int __fastcall SetChatTargetUsers_my(int chataddr, int ecx, int valtype)
{
	if (!UsingCustomChatTarget)
		return pSetChatTargetUsers_ptr(chataddr, ecx, valtype);
	else
		return pSetChatTargetUsers_ptr(chataddr, ecx, CustomChatTarget);
}


time_t AntiSpam_LastTime = std::time(0);
unsigned int AntiSpam_Seconds = 4;
unsigned int AntiSpam_CurMsgCount = 0;
unsigned int AntiSpam_MsgLimit = 10;


void __stdcall SetAntiSpamLimits(unsigned int Messages, unsigned int Seconds)
{
	AntiSpam_Seconds = Seconds;
	AntiSpam_MsgLimit = Messages;
}

unsigned char* _EventVtable = 0;
unsigned char* _ChatSendEvent = 0;

int __stdcall SendMessageToChat(const char* msg, int toAll)
{
	if (!msg || msg[0] == '\0')
		return false;

	if (AntiSpam_MsgLimit && AntiSpam_Seconds)
	{
		time_t AntiSpam_CurTime = std::time(0);
		if (AntiSpam_CurTime - AntiSpam_LastTime > AntiSpam_Seconds)
		{
			AntiSpam_LastTime = AntiSpam_CurTime;
			AntiSpam_CurMsgCount = 0;
		}
		else
		{
			AntiSpam_CurMsgCount++;

			if (AntiSpam_CurMsgCount > AntiSpam_MsgLimit)
			{
				return false;
			}
		}
	}

	unsigned char* ChatOffset = GetChatOffset();
	if (!ChatOffset)
	{
		return false;
	}

	char* pChatString = GetChatString();

	if (!pChatString)
	{
		return false;
	}

	//unsigned char tmpbuf[ 256 ];
	//unsigned char tmpbuf2[ 256 ];
	//memset( tmpbuf2, 0, 256 );
	//GetKeyboardState( tmpbuf );
	//SetKeyboardState( tmpbuf2 );


	//char tmpdeb[ 512 ];
	//sprintf_s( tmpdeb, "%X->%s", pChatString, pChatString );
	//MessageBox( 0, tmpdeb, tmpdeb, 0 );

	BlockInput(true);

	if (/**(int*)ChatFound*/IsChatActive())
	{
		UsingCustomChatTarget = true;
		if (toAll)
		{
			CustomChatTarget = 0;
		}
		else
		{
			CustomChatTarget = 1;
		}
		/* Close chat */
		pChatString[0] = '\0';
		GameChatSetState(ChatOffset, 0, 0);

		/* Open chat */
		GameChatSetState(ChatOffset, 0, 1);

		/* Set message */
		sprintf_s(pChatString, MAX_CHAT_MSG_LEN, "%.128s", msg);

		/* Send Event */
		GameChatSendMessage = (pGameChatSendMessage)(_ChatSendEvent);
		GameChatSendMessage(*(int*)_GlobalGlueObj, 0, _EventVtable);

		UsingCustomChatTarget = false;
	}
	else
	{
		UsingCustomChatTarget = true;

		if (toAll)
		{
			CustomChatTarget = 0;
		}
		else
		{
			CustomChatTarget = 1;
		}

		/* Open chat */
		GameChatSetState(ChatOffset, 0, 1);

		/* Set message */
		sprintf_s(pChatString, MAX_CHAT_MSG_LEN, "%.128s", msg);

		/* Send Event */
		GameChatSendMessage = (pGameChatSendMessage)(_ChatSendEvent);
		GameChatSendMessage(*(int*)_GlobalGlueObj, 0, _EventVtable);

		UsingCustomChatTarget = false;

	}

	BlockInput(false);
	//	SetKeyboardState( tmpbuf );

	return 0;
}


bool starts_with(const std::string& str, const std::string& prefix) {
	return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}
bool starts_with(const std::wstring& str, const std::wstring& prefix) {
	return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}
bool ends_with(const std::string& str, const std::string& suffix) {
	if (str.size() < suffix.size()) return false;
	return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
bool ends_with(const std::wstring& str, const std::wstring& suffix) {
	if (str.size() < suffix.size()) return false;
	return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
bool starts_with(const std::string& str, char prefix) {
	return !str.empty() && str.front() == prefix;
}

bool starts_with(const std::wstring& str, wchar_t prefix) {
	return !str.empty() && str.front() == prefix;
}

bool ends_with(const std::string& str, char suffix) {
	return !str.empty() && str.back() == suffix;
}

bool ends_with(const std::wstring& str, wchar_t suffix) {
	return !str.empty() && str.back() == suffix;
}

std::wstring utf8_to_utf16(const std::string& utf8_str, bool remove_notprint)
{
	int wstr_size = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
	std::wstring wstr(wstr_size, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wstr_size);
	while (!wstr.empty() && wstr.back() == L'\0') {
		wstr.pop_back();
	}
	if (remove_notprint)
	{
		for (auto& ch : wstr) {
			if (!iswprint(ch)) {
				ch = L' ';
			}
		}
	}
	return wstr;
}
std::string utf16_to_utf8(const std::wstring& wideStr) {
	int ansiSize = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string ansiStr(ansiSize, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &ansiStr[0], ansiSize, NULL, NULL);
	while (ends_with(ansiStr, '\0'))
	{
		ansiStr.pop_back();
	}
	return ansiStr;
}

std::string ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return (unsigned char)std::tolower(c); }
	);
	return str;
}
std::string ToUpper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return (unsigned char)std::toupper(c); }
	);
	return str;
}
