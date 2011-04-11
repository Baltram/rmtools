#pragma once

class output
{
public:
	static void msg(char* msggerman, char *msgenglish);
	static void warn(char* msggerman, char *msgenglish);
	static void error(char* msggerman, char *msgenglish);
	static void setlanguage(int language);
private:
	static int lang;
};
