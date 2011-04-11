#include "StdAfx.h"
#include "output.h"
#include <iostream> 
using namespace std;

int output::lang = 0;

void output::msg(char* msggerman, char *msgenglish)
{
	if (lang == 1)
	{
		cout << "Nachricht: " << msggerman << endl;
	}
	else
	{
		cout << "Message: " << msgenglish << endl;
	}
}

void output::warn(char* msggerman, char *msgenglish)
{
	if (lang == 1)
	{
		cout << "Warnung: " << msggerman << endl;
	}
	else
	{
		cout << "Warning: " << msgenglish << endl;
	}
}

void output::error(char* msggerman, char *msgenglish)
{
	if (lang == 1)
	{
		cout << "Fehler: " << msggerman << endl << "Das Programm wurde beendet. Drücken Sie Enter." << endl;
	}
	else
	{
		cout << "Error: " << msgenglish << endl << "Program has been canceled. Press enter." << endl;
	}
	cin.ignore(999, '\n');
	cin.get();
	exit(1);
}

void output::setlanguage(int language)
{
	lang = language;
}

