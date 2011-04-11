#include "StdAfx.h"
#include "file.h"
#include <fstream>
#include "output.h"
#include <sys/stat.h>
#include <math.h>
//#include "stdlib.h"

#include <iostream> 

using namespace std;

file::file(char *filename, int mode)
{
	struct stat fileinf;
	stat(filename, &fileinf);
	filesize = fileinf.st_size;
	newfilesize = filesize;
	numdblocks = 0;
	this->mode = mode;
	wbuffersize = 0;
	bufpos = 0;
	rdpos = 0;
	wrpos = 0;
	if (mode == 1)
	{
		istr.open(filename, ios::in | ios::binary);
		chk();
		buf();
	}
	else if (mode == 2)
	{
		ostr.open(filename, ios::out | ios::binary);
		if (ostr.fail())
		{
			output::error("Zieldatei konnte nicht erstellt werden", "Couldn't create target file");
		}
	}
	else if (mode == 3)
	{
		iostr.open(filename, ios::in | ios::out | ios::binary);
		buf();
	}
	else if (mode == 4)
	{
		char tmp[1000 * sizeof(int)];
		int *jblist = ((int *) &tmp[76]);
		istr.open(filename, ios::in | ios::binary);
		if (istr.fail())
		{
			output::error("Quelldatei konnte nicht geöffnet werden", "Couldn't open source file");
		}
		if (filesize < (1010 * sizeof(int)))
		{
			output::error("Quelldatei inkompatibel", "Source file incompatible");
		}
		istr.read(tmp, (1000 * sizeof(int)));
		numdblocks = (((int *) &tmp[44])[0] + 1);
		if ((numdblocks > 900) || (numdblocks < 0))
		{
			output::error("Quelldatei inkompatibel", "Source file incompatible");
		}
		newfilesize = (filesize - (numdblocks * 512));
		dblock[0] = 0;
		dsizetodblock[0] = 0;
		for (int i = 0; i < (numdblocks - 1); i++)
		{
			dblock[i + 1] = (jblist[i] * 512 + 1024);
			dblocksize[i] = (dblock[i + 1] - dblock[i] - 512);
			dsizetodblock[i + 1] = (dsizetodblock[i] + dblocksize[i]);
		}
		dblocksize[numdblocks - 1] = (filesize - dblock[numdblocks - 1]);
		//cout << numdblocks << endl;
		this->mode = 1;
		int guess = (newfilesize - 100000 - (newfilesize / 100));
		if (guess < 0)
		{
			guess = 0;
		}
		jump(guess);
		chk();
		buf();
	}
	chk();
	active = 111;
}

file::~file(void)
{
	if ((mode > 1) && (active == 111))
	{
		write();
	}
	active = 0;
}

void file::chk(void)
{
	if ((istr.fail()) || (ostr.fail()) || (iostr.fail()))
	{
		output::error("Bad filestream or filestream operation failed", "Bad filestream or filestream operation failed");
	}
}

void file::chkin(void) 
{
	if (mode == 2)
	{
		output::error("Read operation on non-read filestream", "Read operation on non-read filestream");
	}
}

void file::chkout(void) 
{
	if (mode == 1)
	{
		output::error("Write operation on non-write filestream", "Write operation on non-write filestream");
	}
}

int file::getreadpos(void)
{
	chkin();
	return (rdpos + bufpos);
}

int file::getreadpos(int real)
{
	chkin();
	if (real == 1)
	{
		int i = 0;
		while (((i + 1) < numdblocks) && (dsizetodblock[i + 1] <= (rdpos + bufpos)))
		{
			i++;
		}
		return ((dblock[i] + ((rdpos + bufpos) - dsizetodblock[i])));
	}
	else
	{
		return (rdpos + bufpos);
	}
}

void file::buf(void)
{
	//cout << "buf" << endl;
	if (rdpos >= 0)
	{
		if (rdpos <= newfilesize)
		{
			int count = (newfilesize - rdpos);
			if (count > 200000)
			{
				count = 200000;
			}
			if (mode == 1)
			{
				if (numdblocks > 0)
				{
					int i = 0;
					int newrdpos = 0;
					int bufsize = (-1);
					int newcount = 0;
					while (((i + 1) < numdblocks) && (dsizetodblock[i + 1] <= rdpos))
					{
						i++;
					}
					istr.seekg((dblock[i] + (rdpos - dsizetodblock[i])));
					newcount = min(count, (dblocksize[i] - (rdpos - dsizetodblock[i])));
					while (count > 0)
					{
						if (bufsize < 0)
						{
							bufsize = 0;
						}
						else
						{
							istr.seekg(512, ios_base::cur);
						}
						istr.read(&buffer[bufsize], newcount);
						count -= newcount;
						bufsize += newcount;
						i++;
						newcount = min(count, dblocksize[i]);
					}
				}
				else
				{
					istr.seekg(rdpos);
					istr.read(buffer, count);
				}
			}
			else if (mode == 3)
			{
				iostr.seekg(rdpos);
				iostr.read(buffer, count);
			}
		}
		else
		{
			output::error("BUF: Got reading position higher than filesize", "BUF: Got reading position higher than filesize");
		}
	}
	else
	{ 
		output::error("BUF: Got negative reading position", "BUF: Got negative reading position");
	}
	//cout << "####Buffered at " << rdpos << " bufpos: " << bufpos << endl;
	chk();
}

void file::jump(int position)
{
	if (mode == 1)
	{
		jump(position, 1);
	}
	else if (mode == 2)
	{
		jump(position, 2);
	}
	else if (mode == 3)
	{
		jump(position, 1);
		jump(position, 2);
	}
}

void file::jump(int position, int rwmode)
{
	if ((rwmode == 2) && (wbuffersize > 0))
	{
		write();
	}
	if (position >= 0)
	{
		if (position <= newfilesize)
		{
			if (rwmode == 1)
			{
				chkin();
				if ((position >= rdpos) && (position <= (rdpos + 200000 - 1)))
				{
					bufpos = (position - rdpos);
				}
				else
				{
					bufpos = 64;
					if ((position + 200000 - 64) >= newfilesize)
					{
						bufpos = (position + 200000 - newfilesize);
					}
					if (position < bufpos)
					{
						bufpos = position;
					}
					rdpos = (position - bufpos);
					buf();
				}
			}
			else if (rwmode == 2)
			{
				chkout();
				wrpos = position;
			}
			else
			{
				output::error("JUMP: Invalid write/read mode argument", "JUMP: Invalid write/read mode argument");
			}
		}
		else
		{
			output::error("JUMP: Jump to position higher than filesize", "JUMP: Jump to position higher than filesize");
		}
	}
	else
	{
		output::error("JUMP: Jump to negative position", "JUMP: Jump to negative position");
	}
	//cout << "####Jumped to " << position << " rdpos: " << rdpos << " bufpos: " << bufpos << endl;
}

void file::skip(int count)
{
	if (mode == 1)
	{
		jump((rdpos + bufpos + count), 1);
	}
	if (mode == 2)
	{
		jump((wrpos + count), 2);
	}
	if (mode == 3)
	{
		jump((rdpos + bufpos + count), 1);
		jump((wrpos + count), 2);
	}
}


void file::reserve(int count, bool expecteof)
{
	chkin();
	if ((count < 0) || (count > (200000 - 64)) || (count > newfilesize))
	{
		output::error("Invalid reservation count", "Invalid reservation count");
	}
	if ((rdpos + bufpos + count) > newfilesize)
	{
		if (expecteof == false)
		{
			output::warn("Unexpected end of file. Start reading from the beginning...", "Unexpected end of file. Start reading from the beginning...");
		}
		jump(0);
	}
	else if ((bufpos + count) > 200000)
	{
		skip(count);
		skip(-count);
		reserve(count, expecteof);
	}
}

int file::eof(void)
{
	if ((rdpos + bufpos) >= filesize)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void file::read(char *destination, int count)
{
	chkin();
	if (count > (newfilesize - rdpos - bufpos))
	{
		output::error("Tried to read over end of file", "Tried to read over end of file");
	}
	int len = 0;
	int destpos = 0;
	while (count > 0)
	{
		len = min(count, 199900);
		reserve(len, false);
		memcpy(&destination[destpos], &buffer[bufpos],len);
		skip(len);
		destpos += len;
		count -= len;
	}
}

void file::linein(char *filter, int fcount)
{
	int thirteen = 0;
	int data[2] = {0, 0};
	lscount = 0;
	char c = 99;
	while ((rdpos + bufpos) < filesize)
	{
		reserve(1, 0);
		c = buffer[bufpos];
		if (c == 13)
		{
			thirteen = 1;
		}
		else if (c == 10)
		{
			bufpos++;
			break;
		}
		data[0] = 1;
		for (int i = 0;i < fcount;i++)
		{
			if (c == filter[i])
			{
				data[0] = 0;
				if (data[1] == 1)
				{
					seglenghts[lscount - 1] = (rdpos + bufpos - linesegments[lscount - 1]); 
				}
				break;
			}
		}
		if ((data[0] == 1) && (data[1] == 0))
		{
			linesegments[lscount] = (rdpos + bufpos);
			lscount++;

		}
		data[1] = data[0];
		if (lscount > 78)
		{
			output::error("Too many line segments", "Too many line segments");
		}
		bufpos++;
	}
	if (data[0] == 1)
	{
		seglenghts[lscount - 1] = (rdpos + bufpos - linesegments[lscount - 1] - 1);
	}
	seglenghts[lscount - 1] -= thirteen;
	if ((seglenghts[lscount - 1] < 1) && (lscount > 0))
	{
		lscount--;
	}
}

int file::numlineparts(void)
{
	return lscount;
}

char * file::lascii(int pos)
{
	if (pos >= lscount)
	{
		output::error("Invalid line sequence", "Invalid line sequence"); 
	}
	int tmp = (rdpos + bufpos);
	jump(linesegments[pos]);
	int lenght = seglenghts[pos];
	reserve(lenght, false);
	memcpy(textbuffer,&buffer[bufpos], lenght);
	textbuffer[lenght] = ((char) 0);
	jump(tmp);
	return textbuffer;
}

int file::lint(int pos)
{
	if (pos >= lscount)
	{
		output::error("Invalid line sequence", "Invalid line sequence"); 
	}
	int tmp = (rdpos + bufpos);
	jump(linesegments[pos]);
	reserve(seglenghts[pos], false);
	int i = atoi(&buffer[bufpos]);
	jump(tmp);
	return i;
}

float file::lfloat(int pos)
{
	if (linesegments[pos] == 41430)
	{
		pos = pos;
	}
	if (pos >= lscount)
	{
		output::error("Invalid line sequence", "Invalid line sequence"); 
	}
	int tmp = (rdpos + bufpos);
	jump(linesegments[pos]);
	reserve(seglenghts[pos], false);
	/*
	char buff[100];
	memcpy(buff, &buffer[bufpos], 8);
	buff[8] = 'a';
	//strcat(buff, "1.03462245a");
	*/
	float f = atof((char *) &buffer[bufpos]);
	jump(tmp);
	return f;
}

float file::readfloat()
{
	chkin();
	reserve(4, false);
	skip(4);
	return (((float *) &buffer[bufpos - 4])[0]);
}

unsigned int file::readlong()
{
	chkin();
	reserve(4, false);
	skip(4);
	return (((unsigned int *) &buffer[bufpos - 4])[0]);
}

unsigned int file::readshort()
{
	chkin();
	reserve(2, false);
	skip(2);
	return ((unsigned int) ((unsigned short int *) &buffer[bufpos - 2])[0]);
}

unsigned int file::readbyte()
{
	chkin();
	reserve(1, false);
	skip(1);
	return ((unsigned int) ((unsigned char *) &buffer[bufpos - 1])[0]);
}

void file::write(void)
{
	chk();
	chkout();
	if (wbuffersize <= 50000)
	{
		if (mode == 2)
		{
			ostr.seekp(wrpos - wbuffersize);
			ostr.write(wbuffer, wbuffersize);
		}
		else if (mode == 3)
		{
			iostr.seekp(wrpos - wbuffersize);
			iostr.write(wbuffer, wbuffersize);
		}
	}
	else
	{
		output::error("Cannot write more then 50'000 Bytes", "Cannot write more then 50'000 Bytes");
	}
	if (wrpos > newfilesize)
	{
		newfilesize = wrpos;
	}
	if (wrpos > filesize)
	{
		filesize = wrpos;
	}
	wbuffersize = 0;
	chk();
}

void file::writelong(unsigned int val)
{
	chkout();
	if ((wbuffersize + 4) > 50000)
	{
		write();
	}
	memcpy(&wbuffer[wbuffersize],&val,4);
	wbuffersize += 4;
	wrpos += 4;
}

void file::writeshort(unsigned int val)
{
	chkout();
	if ((wbuffersize + 2) > 50000)
	{
		write();
	}
	memcpy(&wbuffer[wbuffersize],&val,2);
	wbuffersize += 2;
	wrpos += 2;
}

void file::writebyte(unsigned int val)
{
	chkout();
	if ((wbuffersize + 1) > 50000)
	{
		write();
	}
	memcpy(&wbuffer[wbuffersize],&val,1);
	wbuffersize += 1;
	wrpos += 1;
}

void file::writefloat(float val)
{
	chkout();
	if ((wbuffersize + 4) > 50000)
	{
		write();
	}
	memcpy(&wbuffer[wbuffersize],&val,4);
	wbuffersize += 4;
	wrpos += 4;
}

void file::writechararray(char *chararray, int lenght)
{
	chkout();
	if ((wbuffersize + lenght) > 50000)
	{
		write();
	}
	memcpy(&wbuffer[wbuffersize],chararray,lenght);
	wbuffersize += lenght;
	wrpos += lenght;
}

void file::directwrite(char *source, int size)
{
	chkout();
	if (wbuffersize > 0)
	{
		write();
	}
	if (mode == 2)
	{
		ostr.write(source, size);
	}
	else
	{
		iostr.write(source, size);
	}
	wrpos += size;
	if (wrpos > newfilesize)
	{
		newfilesize = wrpos;
	}
	if (wrpos > filesize)
	{
		filesize = wrpos;
	}
}

void file::writechararray(char *chararray)
{
	writechararray(chararray, strlen(chararray));
}

void file::writeasunicode(char *chararray, int lenght)
{
	chkout();
	if ((wbuffersize + (2 * lenght)) > 50000)
	{
		write();
	}
	for (int i = 0;i < lenght;i++)
	{
		wbuffer[wbuffersize + (2 * i)] = chararray[i];
		wbuffer[wbuffersize + (2 * i) + 1] = ((char) 0);
	}
	wbuffersize += (2 * lenght);
	wrpos += (2 * lenght);
}


void file::writeasunicode(char *chararray)
{
	writeasunicode(chararray, strlen(chararray));
}

void file::writeasunicode(char *chararray, char *dst, int lenght)
{
	for (int i = 0;i < lenght;i++)
	{
		dst[2 * i] = chararray[i];
		dst[2 * i + 1] = ((char) 0);
	}
}

void file::writeasunicode(char *chararray, char *dst)
{
	writeasunicode(chararray, dst, strlen(chararray));
}

char * file::readascii(void)
{
	return (readascii(nextnullbyte(1)));
}

char * file::readascii(int lenght)
{
	chkin();
	if (lenght < 0)
	{
		output::error("READASCII: Got negative text lenght", "READASCII: Got negative text lenght");
	}
	if (lenght < 255) 
	{
		reserve(lenght, false);
		memcpy(textbuffer,&buffer[bufpos],lenght);
		textbuffer[lenght] = ((char) 0);
	}
	else
	{
		output::error("READASCII: Can't read more than 254 characters", "READASCII: Can't read more than 254 characters");
	}
	skip(lenght);
	return &textbuffer[0];
}

char * file::readunicode(void)
{
	return (readunicode(nextnullbyte(2)));
}

char * file::readunicode(int lenght)
{
	chkin();
	if (lenght < 0)
	{
		output::error("READUNICODE: Got negative text lenght", "READUNICODE: Got negative text lenght");
	}
	if (lenght < 255)
	{
		reserve((lenght * 2), false);
		for (int i = 0; i < lenght;i++)
		{
			textbuffer[i] = buffer[bufpos + (2 * i)];
		}
		textbuffer[lenght] = ((char) 0);
	}
	else
	{
		output::error("READUNICODE: Can't read more than 254 characters", "READUNICODE: Can't read more than 254 characters");
	}
	skip(lenght * 2);
	return &textbuffer[0];
}

int file::nextnullbyte(int shift)
{
	int ret = 0;
	chkin();
	int reslen = (newfilesize - rdpos - bufpos);
	if (reslen > (254 * shift))
	{
		reslen = (254 * shift);
	}
	reserve(reslen, false);
	for (;ret<(reslen / shift);ret++)
	{
		if (buffer[bufpos + (ret * shift)] == ((char) 0))
		{
			reslen = 0;
		}
	}
	return ret;
}

int file::searchbytes(char *bytes, int count)
{
	int i = -1;
	int n = 0;
	int m = 0;
	chkin();
	if ((count > 120) || (count < 0))
	{
		output::error("SEARCHBYTES: Invalid search-charcount argument", "SEARCHBYTES: Invalid search-charcount argument");
	}
	int ret = -1;
	int newmatches[120];
	for (i = -1;i < count;i++)
	{
		m = 0;
		for (n = 1;n <= i;n++)
		{
			if (bytes[m] == bytes[n])
			{
				m++;
			}
			else
			{
				m = 0;
			}
		}
		newmatches[i + 1] = m;
	}
	int matches = 0;
	char cur;
	for (int i = 0;i < (newfilesize + count);i++)
	{
		reserve(1,true);
		//if ((((char) readbyte()) == bytes[matches]) || (bytes[matches] = 'b'))
		cur = readbyte();
		if (cur == bytes[matches])
		{
			matches += 1;
			if (matches == count)
			{
				break;
			}
			else
			{
				while (bytes[matches] == 'b')
				{
					matches += 1;
					reserve(1,true);
					skip(1);
				}
				if (matches == count)
				{
					break;
				}
			}
		}
		else
		{
			if (matches > 0)
			{
				skip(-1);
				i--;
			}
			matches = newmatches[matches];
		}
	}
	if (matches == count)
	{
		ret = ((rdpos + bufpos) - count);
		skip((-1) * count);
	}
	return ret;
}

int file::findverts(float *firstverts, int count)
{
	int c = 3;
	chkin();
	for (int i = 0;i < (newfilesize + 64);i++)
	{
		reserve(64, true);
		float *floats = ((float *) &buffer[bufpos + 4]);
		if (((int *) &buffer[bufpos])[0] == count)
		{
			while ((c < 15) && ((fabs(1 - ((floats[c] - floats[c - 3]) / (firstverts[c] - firstverts[c - 3]))) < 0.0001) || (fabs((floats[c] - floats[c - 3]) - (firstverts[c] - firstverts[c - 3])) < 0.0001)))
			{
				c++;
			}
			if (c == 15)
			{
				return (rdpos + bufpos + 4);
			}
			else
			{
				c = 3;
			}
		}
		skip(1);
	}
	return (-1);
}

int file::fakeintasint(float fakeint)
{
	return ((int) (fakeint * 1000000.0 + 0.5));
}

int file::findfaces(char *fakeints, int count)
{
	chkin();
	int ret = -1;
	int n = 0;
	char temp[104];
	for (int i = 0; i < 100; i++)
	{
		temp[i + 4] = 'b';
	}
	memcpy(&temp[0],&count,4);
	for (int i = 0;i < 25;i++)
	{
		if ((i % 5) < 3)
		{
			int fi = fakeintasint(((float *) &fakeints[n])[0]);
			memcpy(&temp[4 * i + 4],&fi,4);
			n += 4;
		}
	}
	ret = searchbytes(temp, 104);
	if (ret > (-1))
	{
		ret += 4;
	}
	return ret;
}

int file::findtvfaces(char *fakeints, int count)
{
	chkin();
	int ret = -1;
	char temp[64];
	memcpy(&temp[0],&count,4);
	for (int i = 0;i < 15;i++)
	{
		int fi = fakeintasint(((float *) &fakeints[4 * i])[0]);
		memcpy(&temp[4 * i + 4],&fi,4);
	}
	ret = searchbytes(temp, 64);
	if (ret > (-1))
	{
		ret += 4;
	}
	return ret;
}

int file::findtverts(char *firsttverts, int count)
{
	chkin();
	int ret = -1;
	char temp[64];
	memcpy(&temp[0],&count,4);
	memcpy(&temp[4],&firsttverts[0],60);
	ret = searchbytes(temp, 64);
	if (ret > (-1))
	{
		ret += 4;
	}
	return ret;
}

int file::getfilesize(void)
{
	return newfilesize;
}

int file::getfilesize(int real)
{
	if (real == 1)
	{
		return filesize;
	}
	else
	{
		return newfilesize;
	}
}

void file::writever(void)
{
	writeshort(201);
}

void file::writemv(void)
{
	writeshort(30);
}

int file::getwritepos(void)
{
	return wrpos;
}

void file::close(void)
{
	if (((mode == 2) || (mode == 3)) && (active == 111))
	{
		write();
	}
	wrpos = 0;
	rdpos = 0;
	bufpos = 0;
	filesize = 0;
	newfilesize = 0;
	istr.close();
	ostr.close();
	iostr.close();
	active = 0;
}