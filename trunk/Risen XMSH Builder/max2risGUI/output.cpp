#include "StdAfx.h"
#include "output.h"
#include <iostream> 
using namespace std;

int output::lang = 1;

void output::msg(char* msggerman, char *msgenglish)
{
	System::String ^cap;
	System::String ^msg;
	if (lang == 0) //english
	{
		msg = gcnew System::String(msgenglish);
		cap = gcnew System::String("Message");
	}
	else //german
	{
		msg = gcnew System::String(msggerman);
		cap = gcnew System::String("Meldung");
	}
	System::Windows::Forms::MessageBox::Show(msg, cap);
}

void output::warn(char* msggerman, char *msgenglish)
{
	System::String ^cap;
	System::String ^msg;
	if (lang == 0) //english
	{
		msg = gcnew System::String(msgenglish);
		cap = gcnew System::String("Warning");
	}
	else //german
	{
		msg = gcnew System::String(msggerman);
		cap = gcnew System::String("Warnung");
	}
	System::Windows::Forms::MessageBox::Show(msg, cap);
}

void output::error(char* msggerman, char *msgenglish)
{
	System::String ^cap;
	System::String ^msg;
	if (lang == 0) //english
	{
		msg = gcnew System::String(msgenglish);
		cap = gcnew System::String("Error");
	}
	else //german
	{
		msg = gcnew System::String(msggerman);
		cap = gcnew System::String("Fehler");
	}
	System::Windows::Forms::MessageBox::Show(msg, cap);
	//exit(1);
}

void output::setlanguage(int language)
{
	lang = language;
}

int output::getlanguage(void)
{
	return lang;
}

