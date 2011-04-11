#pragma once

#include <fstream>
using namespace std;

class file
{
public:
	file(char *filename, int mode); //1-read 2-write 3-both 4-readgmax
	~file(void);
	void close(void);
	int getreadpos(void);
	int getreadpos(int real);
	int getwritepos(void);
	void jump(int position);
	void jump(int position, int rwmode);
	void skip(int count);
	void linein(char *filter, int fcount);
	int numlineparts(void);
	char * lascii(int pos);
	int lint(int pos);
	float lfloat(int pos);
	float readfloat(void);
	unsigned int readlong(void);
	unsigned int readshort(void);
	unsigned int readbyte(void);
	char * readascii(void);
	char * readascii(int lenght);
	char * readunicode(int lenght);
	char * readunicode(void);
	void read(char *destination, int count);
	void writever(void);
	void writemv(void);
	void writelong(unsigned int val);
	void writeshort(unsigned int val);
	void writebyte(unsigned int val);
	void writefloat(float val);
	void writeasunicode(char *chararray, int lenght);
	void writeasunicode(char *chararray);
	static void writeasunicode(char *chararray, char *dst, int lenght);
	static void writeasunicode(char *chararray, char *dst);
	void directwrite(char *source, int size);
	void writechararray(char *chararray, int lenght);
	void writechararray(char *chararray);
	int searchbytes(char *bytes, int count);
	int findverts(float *firstverts, int count);
	int findfaces(char *fakeints, int count);
	int findtverts(char *firsttverts, int count);
	int findtvfaces(char *fakeints, int count);
	void write(void);
	int getfilesize(void);
	int getfilesize(int real);
	static int fakeintasint(float fakeint);
	int eof(void);

private:
	int active;
	int linesegments[80];
	int seglenghts[80];
	int lscount;
	int numdblocks;
	int dblock[500];
	int dblocksize[500];
	int dsizetodblock[500];
	int mode; //1 - lesen; 2 - schreiben; 3 - beides
	int filesize;
	int newfilesize;
	char textbuffer[255];
	char wbuffer[50000];
	char buffer[200000];
	int wbuffersize;
	int rdpos;
	int wrpos;
	int bufpos;
	void chkin(void);
	void chkout(void);
	void chk(void);
	void reserve(int count, bool expecteof);
	void buf(void);
	int nextnullbyte(int shift);
	ifstream istr;
	ofstream ostr;
	fstream iostr;
};