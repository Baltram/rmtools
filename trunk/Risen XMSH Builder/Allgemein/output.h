#pragma once

class output
{
public:
	static void msg(char* msggerman, char *msgenglish);
	static void warn(char* msggerman, char *msgenglish);
	static void error(char* msggerman, char *msgenglish);
	static void setlanguage(int language);
private:
	static wchar_t msgbuf[1000];
	static wchar_t hmessage[30];
	static wchar_t hwarning[30];
	static wchar_t herror[30];
	static int lang;
	static void getmsg(char* msggerman, char *msgenglish);
};
