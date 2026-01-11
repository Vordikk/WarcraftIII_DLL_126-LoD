#include "Main.h"
// only for test ( http://i.imgur.com/40qQr8X.jpg )



unsigned char* DefaultCStatus = 0;
unsigned char* LoadFramesVar1 = 0;
unsigned char* LoadFramesVar2 = 0;

pLoadNewFrameDef LoadNewFrameDef_org;
pCreateNewFrame CreateNewFrame;
pShowFrameAlternative ShowFrameAlternative;
pSetFramePos SetFramePos;
pShowThisFrame ShowThisFrame;
pDestructThisFrame DestructThisFrame;//Without clean memory
pGetFrameItemAddress GetFrameItemAddress;

bool usedcustomframes = false;

char ConfigPath[MAX_PATH];


void ReadAllConfig()
{

}


void WriteAllConfig()
{

}


void LoadNewFrameDef(const char* filename)
{
	usedcustomframes = true;
	LoadNewFrameDef_org(filename, LoadFramesVar1, LoadFramesVar2, DefaultCStatus);
}


unsigned char* CreateNewFrameAndShow(const char* FrameName, int Show = false)
{
	unsigned char* pGlobalGameClass = (unsigned char*)GameUIObjectGet();
	unsigned char* FrameAddr = CreateNewFrame(FrameName, pGlobalGameClass, 0, 0, 0);
	if (Show && FrameAddr)
	{
		ShowThisFrame(FrameAddr);
	}
	return FrameAddr;
}

void DestroyFrame(unsigned char* FrameAddr, int unk = true)
{
	DestructThisFrame(FrameAddr, unk);
}

void ShowFrameWithPos(unsigned char* FrameAddr, float left, float bottom, int unk = true)
{
	SetFramePos(FrameAddr + 180, 6, left, bottom, true);
	ShowThisFrame(FrameAddr);
}

void ShowFrameItemWithPos(unsigned char* FrameAddr, float left, float bottom, int unk = true)
{
	SetFramePos(FrameAddr, 6, left, bottom, true);
	ShowThisFrame(FrameAddr);
}


void ShowFrameWithPosAligned(unsigned char* FrameAddr, float left, float bottom, int align, int unk = true)
{
	SetFramePos(FrameAddr + 180, align, left, bottom, true);
	ShowThisFrame(FrameAddr);
}

void ShowFrameWithPosAlternativeAligned(unsigned char* FrameAddr, float left, float bottom, int alignFirst, int alignTwo, int unk = true)
{
	unsigned char* pGlobalGameClass = (unsigned char*)GameUIObjectGet();
	ShowFrameAlternative(FrameAddr + 180, alignFirst, pGlobalGameClass + 180, alignTwo, left, bottom, 1);
	ShowThisFrame(FrameAddr);
}


void ShowFrameWithPosAlternative(unsigned char* FrameAddr, float left, float bottom)
{
	unsigned char* pGlobalGameClass = (unsigned char*)GameUIObjectGet();
	ShowFrameAlternative(FrameAddr + 180, 6, pGlobalGameClass + 180, 6, left, bottom, 1);
	ShowThisFrame(FrameAddr);
}


struct FrameDefStatus
{
	int FDefVtable;
	int zeroint;
	int this_add_8;
	int this_add_8_negative;
	int this_one;
};
FrameDefStatus fStatus;

bool FrameDefFileUpdated = false;

void LoadCustomFramesIfNeed()
{
	if (!FrameDefFileUpdated)
	{
		FrameDefFileUpdated = true;
		LoadNewFrameDef("CustomFrames.txt");
	}
}

pUpdateFrameFlags UpdateFrameFlags;


bool GlyphButtonEnabled = true;
bool GlyphButtonCreated = false;
unsigned char* GlyphButtonAddr = 0;

jRCString CallBackFuncName;

int __stdcall CreateGlyphButton(const char* callbackfunc, int create)
{
	LoadCustomFramesIfNeed();

	if (create && GlyphButtonCreated)
	{
		return 0;
	}

	if (create)
	{
		str2jstr(&CallBackFuncName, callbackfunc);

		GlyphButtonAddr = CreateNewFrameAndShow("GlyphButton");
		if (GlyphButtonAddr)
		{
			GlyphButtonCreated = true;
			ShowFrameWithPos(GlyphButtonAddr, 0.155f, 0.006f);
		}
	}

	if (!create && GlyphButtonCreated)
	{
		GlyphButtonCreated = false;
		DestroyFrame(GlyphButtonAddr);
		GlyphButtonAddr = 0;
	}


	return 0;
}



int __stdcall SetGlyphButtonEnabled(int enabled)
{
	GlyphButtonEnabled = enabled;

	if (GlyphButtonCreated && GlyphButtonAddr)
	{
		unsigned char* GlyphButtonItemFrame = GetFrameItemAddress("GlyphItemButton", 0);
		if (GlyphButtonItemFrame)
		{
			if (enabled)
			{
				if (!(*(unsigned int*)(GlyphButtonItemFrame + 0x1D4) & 1))
					*(unsigned int*)(GlyphButtonItemFrame + 0x1D4) += 1;
			}
			else
			{
				if ((*(unsigned int*)(GlyphButtonItemFrame + 0x1D4) & 1))
					*(unsigned int*)(GlyphButtonItemFrame + 0x1D4) -= 1;
			}

			//if ( !( *( unsigned int* )( GlyphButtonItemFrame + 0x1CC ) & 0x80 ) )
			//{
				// Block change focus
			//	*( unsigned int* )( GlyphButtonItemFrame + 0x1CC ) += 0x80;
			//}

			UpdateFrameFlags(GlyphButtonItemFrame, 0);
		}
	}
	return enabled;
}



void ProcessClickAtCustomFrames()
{
	/*if ( GlyphButtonCreated && GlyphButtonAddr )
	{
		int GlyphButtonItemFrame = GetFrameItemAddress( "GlyphItemButton", 0 );
		if ( GlyphButtonItemFrame > 0 )
		{
			int GlyphBtnFlags = *( unsigned int* )( GlyphButtonItemFrame + 0x1D4 );
			if ( GlyphBtnFlags & 1 && GlyphBtnFlags & 4 && GlyphBtnFlags & 0x10 )
			{
				ExecuteFunc( &CallBackFuncName );
			}
		}
	}*/
}

pWc3ControlClickButton Wc3ControlClickButton_ptr;
pWc3ControlClickButton Wc3ControlClickButton_org;

int __fastcall Wc3ControlClickButton_my(void* btnaddr, int, int unk)
{
	int retval = Wc3ControlClickButton_ptr(btnaddr, unk);
	if (ClickHelperDisabled)
		return retval;
	if (GlyphButtonCreated && GlyphButtonAddr)
	{
		void* GlyphButtonItemFrame = (void*)GetFrameItemAddress("GlyphItemButton", 0);
		if (GlyphButtonItemFrame)
		{
			if (btnaddr == GlyphButtonItemFrame)
			{
				ExecuteFunc(&CallBackFuncName);
			}
		}
	}
	return retval;
}
