#include "StdAfx.h"
#include "..\Allgemein\output.h"
#include <iostream> 
using namespace std;

wchar_t output::msgbuf[1000];
wchar_t output::hmessage[30];
wchar_t output::hwarning[30];
wchar_t output::herror[30];
int output::lang = 0;

void output::msg(char* msggerman, char *msgenglish)
{
	getmsg(msggerman, msgenglish);
	::MessageBox( NULL, msgbuf, hmessage, MB_OK );
}

void output::warn(char* msggerman, char *msgenglish)
{
	getmsg(msggerman, msgenglish);
	::MessageBox( NULL, msgbuf, hwarning, MB_OK );
}

void output::error(char* msggerman, char *msgenglish)
{
	getmsg(msggerman, msgenglish);
	::MessageBox( NULL, msgbuf, herror, MB_OK );
	exit(1);
}

void output::setlanguage(int language)
{
	size_t dummy;
	lang = language;
	if (lang == 0) //Englisch
	{
		mbstowcs_s(&dummy, hmessage, 30, "Message", 30);
		mbstowcs_s(&dummy, hwarning, 30, "Warning", 30);
		mbstowcs_s(&dummy, herror, 30, "Error", 30);
	}
	else //Deutsch
	{
		mbstowcs_s(&dummy, hmessage, 30, "Meldung", 30);
		mbstowcs_s(&dummy, hwarning, 30, "Warnung", 30);
		mbstowcs_s(&dummy, herror, 30, "Fehler", 30);
	}
}

void output::getmsg(char* msggerman, char *msgenglish)
{
	size_t dummy;
	if (lang == 0) //Englisch
	{
		mbstowcs_s(&dummy, msgbuf, 1000, msgenglish, 1000);
	}
	else //Deutsch
	{
		mbstowcs_s(&dummy, msgbuf, 1000, msggerman, 1000);
	}
}

