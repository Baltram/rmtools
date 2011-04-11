// max2xmsh.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h"
#include "max2xmsh.h"
#include "..\Allgemein\file.h"
#include "..\Allgemein\output.h"
#include "..\Allgemein\mesh.h"
#include "..\Allgemein\twodim.h"
#include <math.h>
#include "windows.h"
#include <iostream>

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
	char *maxfile = &args[1];
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

	char *matname;
	char num[4];
	char matstart[22];
	file::writeasunicode(":::start:::", matstart, 11);
	char matend[18];
	file::writeasunicode(":::end:::", matend, 9);
	//char matnames[20000];
	int a = 0;
	int b = 0;

	float kennung[3] = {6.2947698,2.5928173,9.1820545};
	int exphelper;
	int objcount;
	int newobjcount = 0;
	int allindcount = 0;
	char *objinf = 0;
	int allindpos = 0;
	int numprintedverts = 0;
	int *allindices = 0;
	file gmax(maxfile, 4);
	file xmsh(xmshfile, 2);
	
	//start

	exphelper = gmax.searchbytes((char *) kennung, 12);
	if (exphelper == (-1))
	{
		output::error("Quelldatei ungültig. Verwenden Sie in 3ds Max / gmax 'Risen Tools' > 'Scene vorbereiten und speichern'", "Invalid source file. In 3ds Max / gmax, use 'Risen Tools' > 'Prepare and save scene'");
	}
	//cout << "Bytes from exporthelper to end of file: " << (gmax.getfilesize() - exphelper) << endl;
	gmax.skip(12);
	objcount = file::fakeintasint(gmax.readfloat());
	//cout << "Num objects: " << objcount << endl;
	gmax.skip(8);
	objinf = new char [(252 * objcount)];
	gmax.read(objinf,(252 * objcount));

	//mats
	twodim mats(20000, objcount);
	gmax.jump(0);
	while (1 == 1)
	{
		a = gmax.searchbytes(matstart, 22);
		b = gmax.searchbytes(matend, 18);
		if (a < 0)
		{
			output::error("Problems when trying to read material names", "Problems when trying to read material names");
		}
		gmax.jump(a + 22);
		if ((b - a - 22) == 26)
		{
			char tmpmn[14];
			memcpy(tmpmn, gmax.readunicode(13), 13);
			tmpmn[13] = 0;
			int hh = strcmp(":::lastmat:::", tmpmn);
			if (!strcmp(":::lastmat:::", tmpmn))
			{
				break;
			}
		}
		memcpy(num, gmax.readunicode(4), 4);
		int n = atoi(num);
		//cout << "num: " << n << endl;
		gmax.skip(8);
		mats.addstring((n - 1), ((b - a - 22 - 16) / 2), gmax.readunicode((b - a - 22 - 16) / 2));
	}

	//cout << endl;
	for (int i = 0;i < objcount;i++)
	{
		//cout << "Obj " << (i + 1) << ":" << endl;
		int mc = mats.getcount(i);
		for (int j = 0; j < mc;j++)
		{
			//cout << (j + 1) << ". " << mats.getstring(i, j) << endl;
		}
		//cout << endl;
	}
	//end mats

	for (int i = 0;i < objcount;i++)
	{
		allindcount += (3 * file::fakeintasint(((float *) &objinf[252 * i + 4])[0]));
		newobjcount += mats.getcount(i);
	}
	allindices = new int [allindcount];
	//cout << "allfaces: " << allindcount << endl;
	
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

	xmsh.writelong(newobjcount);

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

	twodim objds((newobjcount * (sizeof(objdummys) + sizeof(int))), objcount);

	for (int i = 0;i < objcount;i++)
	{
		int c = mats.getcount(i);
		for (int j = 0;j < c;j++) 
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

			matname = mats.getstring(i, j);
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
	//xmsh.writeshort(19); //lenght of lightmap name
	//xmsh.writechararray("Levelmesh_DonCamp_0", 19); //no lightmap
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
		mesh newmesh(&gmax, &objinf[252 * i], mats.getcount(i), angle);
		//cout << "FFF: " << (newmesh.getface(0))[0] << " " << (newmesh.getface(0))[1] << " " << (newmesh.getface(0))[2] << endl; 

		int c = mats.getcount(i);
		for (int j = 0;j < c;j++)
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

	delete[] objinf;
	delete[] allindices;
	output::msg("Umwandlung erfolgreich!", "Conversion succesful!");	
	return 0;

	MSG msg;
	HACCEL hAccelTable;

	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAX2XMSH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Anwendungsinitialisierung ausführen:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAX2XMSH));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAX2XMSH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MAX2XMSH);
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

  // ShowWindow(hWnd, nCmdShow);
  // UpdateWindow(hWnd);

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
