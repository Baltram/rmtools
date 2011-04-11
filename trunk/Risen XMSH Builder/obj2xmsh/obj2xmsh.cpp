// obj2xmsh.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h"
#include "obj2xmsh.h"
#include "..\Allgemein\output.h"
#include "..\Allgemein\file.h"
#include "..\Allgemein\mesh.h"
#include "..\Allgemein\twodim.h"
#include <math.h>
#include "windows.h"
#include <iostream>
#include <vector>

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE hInst;								// Aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];					// Titelleistentext
TCHAR szWindowClass[MAX_LOADSTRING];			// Klassenname des Hauptfensters

// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Hier Code einfügen.

	char args[2000];
	char *objfile = &args[1];
	char *xmshfile;
	char *language;
	char *cangle;
	int instring = 0;
	int argcount = 1;

	size_t dummy = 0;
	wcstombs_s(&dummy, args, 2000, lpCmdLine, 2000);
	for (int i = 0;i < 2000;i++)
	{
		if (args[i] == 0)
		{
			break;
		}
		if (args[i] == 34) //Anführungszeichen
		{
			instring = (!instring);
			args[i] = 0;
		}
		if ((args[i] == 32) && (instring == 0)) //Leerzeichen außerhalb eines Strings
		{
			argcount++;
			args[i] = 0;
			if (argcount == 2)
			{
				xmshfile = &args[i + 2];
			}
			if (argcount == 3)
			{
				cangle = &args[i + 1];
			}
			if (argcount == 4)
			{
				language = &args[i + 1];
			}
		}
	}

	if (argcount != 4)
	{
		output::error("Falsche Anzahl Argumente", "Wrong number of arguments");
	}

	if (language[0] == 'D')
	{
		output::setlanguage(1);
	}
	if (language[0] == 'E')
	{
		output::setlanguage(0);
	}

	float angle = atof(cangle);
	
	int allindpos = 0;
	int numprintedverts = 0;
	int *allindices = 0;
	file obj(objfile, 1);
	file xmsh(xmshfile, 2);

	char filter[3] = {47, 32, 9};
	char filter2[2] = {32, 9};

	char matnames[40000];
	int mnp = 0;
	int mats[1000];
	mats[0] = 0;
	int matcount = 0;

	char mnbuf[500] = "(null)";
	int mnblen = 6;

	int objcount = 1;

	int numverts = 0;
	int numfaces = 0;
	int numtverts = 0;
	int numvnormals = 0;
	int numnovnfaces = 0;

	char *text;

	vector<int> breaks;
	breaks.reserve(400 * 3);

	int newbreak = 1;

	int matid = 0;
	int hastverts = 0;
	int hasvnormals = 0;
	int numlfaces = 1;
	int numlindices = 3;
	int htv = 0;
	int hvn = 0;
	int nli = 0;

	int curline = 0;

	int linenum = -1;

	while (!obj.eof())
	{
		curline = obj.getreadpos();
		linenum++;
		//obj.linein(filter, 3);
		obj.linein("", 0);
		if (obj.numlineparts())
		{
			text = obj.lascii(0);
			if (text[0] == 'v')
			{
				if (text[1] == 't')
				{
					numtverts++;
				}
				else if (text[1] == 'n')
				{
					numvnormals++;
				}
				else
				{
					numverts++;
				}
			}
			else if (text[0] == 'f')
			{
				hvn = hasvnormals;
				htv = hastverts;
				nli = numlindices;
				obj.jump(curline);
				obj.linein(filter2, 2);
				numlindices = (obj.numlineparts() - 1);
				numfaces += (numlindices - 2);
				int dslashes = 0;
				text = obj.lascii(1);
				for (int i = 0;i < ((signed int) (strlen(text)) - 1);i++)
				{
					if ((text[i] == 47) && (text[i + 1] == 47)) //two following slashes
					{
						dslashes = 1;
					}
				}
				obj.jump(curline);
				obj.linein(filter, 3);
				int nlp = (obj.numlineparts() - 1);
				if (nlp == numlindices)
				{
					hastverts = 0;
					hasvnormals = 0;
				}
				else if (nlp == (2 * numlindices))
				{
					if (dslashes)
					{
						hastverts = 0;
						hasvnormals = 1;
					}
					else
					{
						hastverts = 1;
						hasvnormals = 0;
					}
				}
				else
				{
					hastverts = 1;
					hasvnormals = 1;
				}
				if ((hastverts != htv) || (hasvnormals != hvn) || (numlindices != nli))
				{
					newbreak = 1;
				}
				if (newbreak)
				{
					if (mnblen > 0)
					{
						memcpy(&matnames[mnp], mnbuf, mnblen);
						mats[matcount] = mnp;
						matid = matcount;
						mnp += (mnblen + 1);
						matnames[mnp - 1] = 0;
						matcount++;
						mnblen = 0;
					}
					breaks.push_back(linenum);
					breaks.push_back(matid);
					breaks.push_back(numlindices);
					breaks.push_back(hastverts);
					breaks.push_back(hasvnormals);
					newbreak = 0;
				}
				if (!hasvnormals)
				{
					numnovnfaces += (numlindices - 2);
				}
			}
			else
			{
				newbreak = 1;
				if (text[0] == 'u')
				{
					obj.jump(curline);
					obj.linein(filter, 3);
					int newmtl = 1;
					text = text = obj.lascii(1);
					for (int i = 0;i < matcount;i++)
					{
						if (strcmp(text, &matnames[mats[i]]) == 0)
						{
							//old mat
							matid = i;
							newmtl = 0;
							break;
						}
					}
					if (newmtl)
					{
						int lenght = strlen(text);
						memcpy(mnbuf, text, lenght);
						mnblen = lenght;
					}
				}
			}
		}
	}

	allindices = new int [numfaces * 3];

	breaks.push_back(0);
	//mesh newmesh(&obj, numverts, numfaces, numtverts, numvnormals, numnovnfaces, breaks, angle);

	//positions of dummys
	int offsets;
	int offset4;
	int offset5;
	int bbofall;
	int vdsize;
	int idsize;

	//positions
	int fileend;
	int numbersbeforemd;
	int endofobjs;
	int vdbegin;
	int vdend;
	int idbegin;
	int idend;

	//other stuff
	float min[3]; //-y z x
	float max[3]; //-y z x
	
	//Header (1)
	xmsh.writelong(825250375);	//12 bytes: GR01MS02....
	xmsh.writelong(842027853);	
	xmsh.writelong(40);

	offsets = xmsh.getwritepos();
	xmsh.writelong(123456789); //dummy - 40 bytes less than next offset
	xmsh.writelong(123456789); //dummy - offset from the beginning of the file to the strange numbers before the mesh data (just after the last C9 00)
	xmsh.writelong(123456789); //dummy - filesize minus previous offset
	
	SYSTEMTIME st;
    FILETIME ft;
	GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);
	xmsh.writechararray((char *) &ft, 8); //write current time as FILETIME

	xmsh.writelong(0);
	xmsh.writelong(0);
	xmsh.writeshort(1);
	xmsh.writelong(16777473);

	xmsh.writeshort(15);
	xmsh.writechararray("eCMeshResource2", 15);

	xmsh.writebyte(1);
	xmsh.writeshort(0);

	xmsh.writever();

	//Header(?) (2)
	xmsh.writever();
	
	offset4 = xmsh.getwritepos();
	xmsh.writelong(123456789); //dummy - offset from here (after this 4 bytes) to the position offset 2 points to

	//Header(?) (3)
	xmsh.writever();
	xmsh.writelong(3);

	xmsh.writeshort(9);
	xmsh.writechararray("SubMeshes", 9);

	xmsh.writeshort(27);
	xmsh.writechararray("bTObjArray<class eCSubMesh>", 27);
	
	xmsh.writemv();

	offset5 = xmsh.getwritepos();
	xmsh.writelong(123456789); //dummy - offset from here (after this 4 bytes) to the two bytes before the string "LightmapName" (right after the list of objects)
	xmsh.writebyte(1);

	xmsh.writelong(matcount); //matcount corresponds to submeshcount

	struct objdummys
	{
		//positions of dummys
		int size;
		int bb;
		int firstindex;
		int indexcount;
		int firstvertex;
		int vertexcount;
		int bb2;

		//positions
		int begin;
		int end;
		int vdbegin;

		//other stuff
		int firstid;
		int vcount;
		int icount;
		float min[3]; //-y z x
		float max[3]; //-y z x
	};

	twodim objds((matcount * (sizeof(objdummys) + sizeof(int))), objcount);
	char *matname;

	for (int i = 0;i < objcount;i++)
	{
		for (int j = 0;j < matcount;j++) 
		{
			objdummys od;

			// (1)
			xmsh.writever();

			od.size = xmsh.getwritepos();
			xmsh.writelong(123456789); //dummy - size of the following data until the end of this loop
	
			// (2)

			od.begin = xmsh.getwritepos();

			xmsh.writever();
			xmsh.writelong(6);

			xmsh.writeshort(12);
			xmsh.writechararray("MaterialName", 12);

			xmsh.writeshort(8);
			xmsh.writechararray("bCString", 8);

			xmsh.writemv();

			matname = &matnames[mats[j]];
			int mnlen = strlen(matname);

			xmsh.writelong(mnlen + 8);
			xmsh.writeshort(mnlen + 6);
			xmsh.writechararray(matname, mnlen);
			xmsh.writechararray("._xmat", 6);

			xmsh.writeshort(7);
			xmsh.writechararray("Extends", 7);

			xmsh.writeshort(5);
			xmsh.writechararray("bCBox", 5);

			xmsh.writemv();
			xmsh.writelong(24);

			od.bb = xmsh.getwritepos();
			xmsh.writechararray("123412341234123412341234", 24); //dummy - bounding box (min and max-vector)

			xmsh.writeshort(10);
			xmsh.writechararray("FirstIndex", 10);

			xmsh.writeshort(4);
			xmsh.writechararray("long", 4);

			xmsh.writemv();
			xmsh.writelong(4);

			od.firstindex = xmsh.getwritepos();
			xmsh.writelong(123456789); //dummy

			xmsh.writeshort(10);
			xmsh.writechararray("IndexCount", 10);

			xmsh.writeshort(4);
			xmsh.writechararray("long", 4);

			xmsh.writemv();
			xmsh.writelong(4);
			od.indexcount = xmsh.getwritepos();
			xmsh.writelong(123456789); //dummy

			xmsh.writeshort(11);
			xmsh.writechararray("FirstVertex", 11);

			xmsh.writeshort(4);
			xmsh.writechararray("long", 4);

			xmsh.writemv();
			xmsh.writelong(4);
			od.firstvertex = xmsh.getwritepos();
			xmsh.writelong(123456789); //dummy

			xmsh.writeshort(11);
			xmsh.writechararray("VertexCount", 11);

			xmsh.writeshort(4);
			xmsh.writechararray("long", 4);

			xmsh.writemv();
			xmsh.writelong(4);
			od.vertexcount = xmsh.getwritepos();
			xmsh.writelong(123456789); //dummy

			//unknown part (seems to be a list of boundary boxes)
			xmsh.writever();
			xmsh.writelong(1); //number of  following "records"

			for (int k = 0;k < 1;k++) //write only one
			{
				od.bb2 = xmsh.getwritepos();
				xmsh.writechararray("123412341234123412341234", 24); //dummy (boundary box of of current object)

				xmsh.writelong(0); //in the first "record" this number seems to be always 0 then it gets rapidely higher
				xmsh.writelong(1); //in the first "record" this number seems to be always the number of "records" then it gets rapidely lower and is mostely a zero.
			}

			od.end = xmsh.getwritepos();

			objds.adddata(i, sizeof(objdummys), ((char *) &od));
		}
	}

	endofobjs = xmsh.getwritepos();

	xmsh.writeshort(12);
	xmsh.writechararray("LightmapName");

	xmsh.writeshort(8);
	xmsh.writechararray("bCString", 8);

	xmsh.writemv();
	xmsh.writelong(2); //lenght of lightmap name + 2
	xmsh.writeshort(0); //lenght of lightmap name
	xmsh.writechararray("", 0); //no lightmap

	xmsh.writeshort(8);
	xmsh.writechararray("Boundary", 8);

	xmsh.writeshort(5);
	xmsh.writechararray("bCBox", 5);

	xmsh.writemv();
	xmsh.writelong(24);

	bbofall = xmsh.getwritepos();
	xmsh.writechararray("123412341234123412341234", 24); //dummy (boundary box of all meshes)

	//another unknown part - a lot of numbers which are the same in every xmsh-file. The following numbers are for objects with no ligtmap. If there's a lightmap in a xmsh-file there are slight changes.
	xmsh.writever();
	
	numbersbeforemd = xmsh.getwritepos();

	xmsh.writelong(0);
	xmsh.writelong(2);

	xmsh.writeshort(0);
	xmsh.writeshort(12);
	xmsh.writeshort(2);
	xmsh.writeshort(3);

	xmsh.writeshort(0);
	xmsh.writeshort(24);
	xmsh.writeshort(2);
	xmsh.writeshort(6);
			
	xmsh.writeshort(0);
	xmsh.writeshort(36);
	xmsh.writeshort(4);
	xmsh.writeshort(10);

	xmsh.writeshort(0);
	xmsh.writeshort(40);
	xmsh.writeshort(4);
	xmsh.writeshort(266);

	xmsh.writeshort(0);
	xmsh.writeshort(44);
	xmsh.writeshort(1);
	xmsh.writeshort(5);

	xmsh.writelong(3);
	xmsh.writelong(67436544);
	xmsh.writelong(255);
	xmsh.writelong(17);
	xmsh.writelong(255);
	xmsh.writelong(17);

	for (int i = 0;i < 14;i++)
	{
		xmsh.writelong(0);
	}

	vdsize = xmsh.getwritepos();
	xmsh.writelong(123456789); //dummy - size of vertex data in bytes

	xmsh.writelong(0);
	xmsh.writelong(0);
	xmsh.writelong(1);
	xmsh.writelong(52); //size of a "vert-record" (60 in files with lighmap)

	for (int i = 0;i < 10;i++)
	{
		xmsh.writelong(0);
	}

	idsize = xmsh.getwritepos();
	xmsh.writelong(123456789); //dummy - size of index data in bytes

	xmsh.writelong(0);
	xmsh.writelong(102);
	xmsh.writelong(1);

	vdbegin = xmsh.getwritepos();
	
	objdummys *od;

	for (int i = 0;i < objcount;i++)
	{
		//cout << endl << "------------------------------------" << endl;
		//cout << "Object " << (i + 1) << endl;
		mesh newmesh(&obj, numverts, numfaces, numtverts, numvnormals, numnovnfaces, breaks, angle);
		//cout << "FFF: " << (newmesh.getface(0))[0] << " " << (newmesh.getface(0))[1] << " " << (newmesh.getface(0))[2] << endl; 

		for (int j = 0;j < matcount;j++)
		{
			int oldnpv = numprintedverts;
			od = ((objdummys *) objds.getdata(i, j));
			od->vdbegin = xmsh.getwritepos();
			od->firstid = allindpos;
			newmesh.output(&xmsh, j, allindices, &allindpos, &numprintedverts, od->min, od->max);
			od->icount = (allindpos - od->firstid);
			od->vcount = (numprintedverts - oldnpv);
			//cout << "MIN: [" << od->min[0] << "," << od->min[1] << "," << od->min[2] << "]" << endl;
			//cout << "MAX: [" << od->max[0] << "," << od->max[1] << "," << od->max[2] << "]" << endl;
		}
	}

	vdend = xmsh.getwritepos();
	idbegin = xmsh.getwritepos();

	for (int i = 0;i < allindpos;i++)
	{
		xmsh.writelong(allindices[i]);
	}

	idend = xmsh.getwritepos();

	xmsh.writebyte(1); //0 if thre's a lightmap

	fileend = xmsh.getwritepos();

	//filling dummies
	od = ((objdummys *) objds.getdata(0,0));

	min[0] = od->min[0];
	min[1] = od->min[1];
	min[2] = od->min[2];
	max[0] = od->max[0];
	max[1] = od->max[1];
	max[2] = od->max[2];

	for (int i = 0;i < objcount;i++)
	{
		int c = objds.getcount(i);
		for (int j = 0;j < c;j++)
		{
			od = ((objdummys *) objds.getdata(i, j));
			if (od->min[0] < min[0])
			{
				min[0] = od->min[0];
			}
			if (od->min[1] < min[1])
			{
				min[1] = od->min[1];
			}
			if (od->min[2] < min[2])
			{
				min[2] = od->min[2];
			}
			if (od->max[0] > max[0])
			{
				max[0] = od->max[0];
			}
			if (od->max[1] > max[1])
			{
				max[1] = od->max[1];
			}
			if (od->max[2] > max[2])
			{
				max[2] = od->max[2];
			}

			xmsh.jump(od->size);
			xmsh.writelong(od->end - od->begin);

			xmsh.jump(od->bb);
			xmsh.writefloat(od->min[0]);
			xmsh.writefloat(od->min[1]);
			xmsh.writefloat(od->min[2]);
			xmsh.writefloat(od->max[0]);
			xmsh.writefloat(od->max[1]);
			xmsh.writefloat(od->max[2]);

			xmsh.jump(od->bb2);
			xmsh.writefloat(od->min[0]);
			xmsh.writefloat(od->min[1]);
			xmsh.writefloat(od->min[2]);
			xmsh.writefloat(od->max[0]);
			xmsh.writefloat(od->max[1]);
			xmsh.writefloat(od->max[2]);

			xmsh.jump(od->firstindex);
			xmsh.writelong(od->firstid);

			xmsh.jump(od->indexcount);
			xmsh.writelong(od->icount);

			xmsh.jump(od->firstvertex);
			xmsh.writelong((od->vdbegin - vdbegin) / 52);

			xmsh.jump(od->vertexcount);
			xmsh.writelong(od->vcount);
		}
	}

	xmsh.jump(offsets);
	xmsh.writelong(numbersbeforemd - 40);
	xmsh.writelong(numbersbeforemd);
	xmsh.writelong(fileend - numbersbeforemd);

	xmsh.jump(offset4);
	xmsh.writelong(numbersbeforemd - (offset4 + 4));

	xmsh.jump(offset5);
	xmsh.writelong(endofobjs - (offset5 + 4));

	xmsh.jump(bbofall);
	xmsh.writefloat(min[0]);
	xmsh.writefloat(min[1]);
	xmsh.writefloat(min[2]);
	xmsh.writefloat(max[0]);
	xmsh.writefloat(max[1]);
	xmsh.writefloat(max[2]);

	xmsh.jump(vdsize);
	xmsh.writelong(vdend - vdbegin);

	xmsh.jump(idsize);
	xmsh.writelong(idend - idbegin);

	//cout << "------------------------------------" << endl << endl;

	//ende

	delete[] allindices;
	output::msg("Umwandlung erfolgreich!", "Conversion succesful!");	

	return 0;

	MSG msg;
	HACCEL hAccelTable;

	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OBJ2XMSH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Anwendungsinitialisierung ausführen:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJ2XMSH));

	// Hauptnachrichtenschleife:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
//  KOMMENTARE:
//
//    Sie müssen die Funktion verwenden,  wenn Sie möchten, dass der Code
//    mit Win32-Systemen kompatibel ist, bevor die RegisterClassEx-Funktion
//    zu Windows 95 hinzugefügt wurde. Der Aufruf der Funktion ist wichtig,
//    damit die kleinen Symbole, die mit der Anwendung verknüpft sind,
//    richtig formatiert werden.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJ2XMSH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OBJ2XMSH);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNKTION: InitInstance(HINSTANCE, int)
//
//   ZWECK: Speichert das Instanzenhandle und erstellt das Hauptfenster.
//
//   KOMMENTARE:
//
//        In dieser Funktion wird das Instanzenhandle in einer globalen Variablen gespeichert, und das
//        Hauptprogrammfenster wird erstellt und angezeigt.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ZWECK:  Verarbeitet Meldungen vom Hauptfenster.
//
//  WM_COMMAND	- Verarbeiten des Anwendungsmenüs
//  WM_PAINT	- Zeichnen des Hauptfensters
//  WM_DESTROY	- Beenden-Meldung anzeigen und zurückgeben
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Menüauswahl bearbeiten:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Hier den Zeichnungscode hinzufügen.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Meldungshandler für Infofeld.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
