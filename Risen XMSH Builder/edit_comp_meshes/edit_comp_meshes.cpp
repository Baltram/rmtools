// edit_comp_meshes.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h"
#include "edit_comp_meshes.h"
#include "..\Allgemein\file.h"
#include "..\Allgemein\output.h"

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE hInst;								// Aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];					// Titelleistentext
TCHAR szWindowClass[MAX_LOADSTRING];			// Klassenname des Hauptfensters

char *newpart;
//char newpart[2000];//
int nppos;

// Vorwärtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void print(void *ptr, int size);
void pmv(void);
void pver(void);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Hier Code einfügen.

	char args[2000];
	char *xmshfile = &args[1];
	char *binfile;
	char *language;
	char *replace;
	int instring = 0;
	int argcount = 1;
	nppos = 0;

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
				binfile = &args[i + 2];
			}
			if (argcount == 3)
			{
				replace = &args[i + 1];
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

	char time[8];
	int offset3;

	file *xmsh = new file(xmshfile, 1);
	xmsh->jump(20);
	offset3 = xmsh->readlong();
	xmsh->read(time, 8);
	xmsh->jump(127);
	int numsubmeshes = xmsh->readlong();
	newpart = new char [152 * numsubmeshes + 1000];

	short int *lightmap;
	short int *filename;
	short int **matnames = new short int * [numsubmeshes];
	char *stringcontainer = new char [1000 + (numsubmeshes * 40)];
	char **strings = new char * [numsubmeshes + 2];
	int strcpos = 0;

	int fnbegin = 0;
	int fplen = strlen(xmshfile);
	for (int i = 0;i < fplen;i++)
	{
		if (xmshfile[i] == 92) //backslash
		{
			fnbegin = (i + 1);
		}
	}

	char *fname = &xmshfile[fnbegin];
	int fnlen = strlen(fname) - 6; //without ._xmsh extension
	memcpy(&stringcontainer[strcpos], fname, fnlen);
	strings[0] = &stringcontainer[strcpos];
	strcpos += (fnlen + 1);
	stringcontainer[strcpos - 1] = 0;

	print("MS02", 4);
	char tmp001[11] = {1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0};
	print(tmp001, 11);
	pver();
	pver();
	int tmp002 = (152 * numsubmeshes) + 69; //eigentlich statt 152: 120 + 32 * [entspr. bblist].count
	print(&tmp002, 4);
	pver();
	char tmp003[8] = {3, 0, 0, 0, 1, 0, 2, 0};
	print(tmp003, 8);
	pmv();
	tmp002 -= 64;
	print(&tmp002, 4);
	char tmp004 = 1;
	print(&tmp004, 1);
	print(&numsubmeshes, 4);

	//print submeshes
	for (int i = 0;i < numsubmeshes;i++)
	{
		xmsh->searchbytes("MaterialName", 12);
		xmsh->skip(28);
		int mnlen = xmsh->readshort() + 1;
		char *matname = xmsh->readascii(mnlen - 1);
		memcpy(&stringcontainer[strcpos], matname, mnlen);
		strings[i + 1] = &stringcontainer[strcpos];
		strcpos += mnlen;

		pver();
		int tmp100 = 146; //114 + 32 * [corresp. bblist].count
		print(&tmp100, 4);

		pver();
		char tmp101[8] = {6, 0, 0, 0, 3, 0, 4, 0}; // {3, 0} = "MaterialName" {4, 0} = "bCString"
		print(&tmp101, 8);
		pmv();
		int tmp102 = 2;
		print(&tmp102, 4);
		matnames[i] = (short int *) &newpart[nppos];
		pver(); //dummy (gets overwritten later by a short that points to the material name)
		char tmp103[4] = {6, 0, 7, 0};  // {6, 0} = "extends" {7, 0} = "bCBox"
		print(&tmp103, 4);
		
		xmsh->skip(16);
		xmsh->read(&newpart[nppos], 30); //contains mv, size and bCBox
		nppos += 30;

		char tmp104[4] = {8, 0, 9, 0};  // {8, 0} = "FirstIndex" {9, 0} = "long"
		print(&tmp104, 4);
		xmsh->skip(18);
		xmsh->read(&newpart[nppos], 10); //contains mv, size and firstindex
		nppos += 10;

		char tmp105[4] = {10, 0, 9, 0};  // {10, 0} = "IndexCount" {9, 0} = "long"
		print(&tmp105, 4);
		xmsh->skip(18);
		xmsh->read(&newpart[nppos], 10); //contains mv, size and indexcount
		nppos += 10;

		char tmp106[4] = {11, 0, 9, 0};  // {11, 0} = "FirstVertex" {9, 0} = "long"
		print(&tmp106, 4);
		xmsh->skip(19);
		xmsh->read(&newpart[nppos], 10); //contains mv, size and firstvertex
		nppos += 10;

		char tmp107[4] = {12, 0, 9, 0};  // {12, 0} = "VertexCount" {9, 0} = "long"
		print(&tmp107, 4);
		xmsh->skip(19);
		xmsh->read(&newpart[nppos], 10); //contains mv, size and vertexcount
		nppos += 10;

		pver();
		int tmp008 = 1; //count of bblist
		print(&tmp008, 4);
		xmsh->skip(6);

		xmsh->read(&newpart[nppos], 24); //contains bCBox of first entry of bblist
		nppos += 24;
		tmp008 = 0;
		print(&tmp008, 4);	
		tmp008 = 1;
		print(&tmp008, 4);	
	}

	char tmp005[4] = {13, 0, 4, 0};
	print(&tmp005, 4);
	pmv();
	int tmp006 = 2;
	print(&tmp006, 4);
	lightmap = (short int *) &newpart[nppos];
	//short int tmp007 = 14; //points to a void bCString (no lightmap)
	//print(&tmp007, 2);
	pver(); //dummy (gets overwritten later by lightmap name)
	char tmp008[4] = {15, 0, 7, 0};
	print(&tmp008, 4);
	pmv();
	tmp006 = 24; //sizeof(bCBox)
	print(&tmp006, 4);
	xmsh->searchbytes("LightmapName", 12);
	xmsh->skip(28);
	int lmlen = xmsh->readshort() + 1;
	memcpy(&stringcontainer[strcpos], xmsh->readascii(lmlen - 1), lmlen);
	strings[numsubmeshes + 1] = &stringcontainer[strcpos];
	strcpos += lmlen;
	xmsh->skip(23);
	xmsh->read(&newpart[nppos], 24); //contains bCBox
	nppos += 24;
	pver();
	filename = ((short int *) &newpart[nppos]);
	pver(); //dummy (gets overwritten later by filename)
	print(&offset3, 4);
	print(time, 8);
	print(time, 8); //print this a seconde time cause i have no idea where the second time must come from (wordgame^^)

	delete xmsh;

	file bin(binfile, 1);
	char nbname[200];
	nbname[0] = 0;
	strcat_s(nbname, 200, binfile);
	strcat_s(nbname, 200, ".new");
	file newbin(nbname, 2);

	bin.jump(10);
	int stringsection = bin.readlong();
	bin.jump(18);
	int numentries = bin.readlong();
	int *filenames = new int [numentries];
	int *entries = new int [numentries]; 

	bin.skip(19); //is now at size of first entry
	for (int i = 0;i < numentries;i++)
	{
		entries[i] = bin.getreadpos() - 19;
		bin.skip(bin.readlong()); //is now at bCString : Filename
		filenames[i] = bin.readshort();
		bin.skip(39); //is now at size of next entry
	}
	bin.skip(-19);
	int insertnp = bin.getreadpos();

	bin.jump(stringsection + 5);
	int numstrings = bin.readlong();
	int lmfound = -1;
	int fnfound = -1;
	int *mnfound = new int [numsubmeshes];
	for (int i = 0;i < numsubmeshes;i++)
	{
		mnfound[i] = -1;
	}
	int strl = 0;
	char *str;
	int tmp666 = numsubmeshes + 1;
	for (int i = 0;i < numstrings;i++)
	{
		strl = bin.readshort();
		str = bin.readascii(strl);
		if (strcmp(str, strings[0]) == 0)
		{
			fnfound = i;
		}
		if (strcmp(str, strings[tmp666]) == 0)
		{
			lmfound = i;
		}
		for (int j = 1;j <= numsubmeshes;j++)
		{
			if (strcmp(str, strings[j]) == 0)
			{
				mnfound[j - 1] = i;
			}
		}
	}

	for (int i = 0;i < numsubmeshes;i++)
	{
		if (mnfound[i] > -1)
		{
			matnames[i][0] = mnfound[i];
		}
		else
		{
			//mnfound[i] = numstrings;
			matnames[i][0] = numstrings;
			for (int j = (i + 1);j < numsubmeshes;j++)
			{
				if (strcmp(strings[i + 1], strings[j + 1]) == 0)
				{
					mnfound[j] = numstrings;
					matnames[j][0] = numstrings;
				}
			}
			numstrings++;
		}
	}
	if (lmfound > -1)
	{
		lightmap[0] = lmfound;
	}
	else
	{
		lightmap[0] = numstrings;
		numstrings++;
	}
	if (fnfound > -1)
	{
		for (int i = 0;i < numentries;i++)
		{
			if (filenames[i] == fnfound)
			{
				insertnp = entries[i];
			}
		}
		filename[0] = fnfound;
	}
	else
	{
		filename[0] = numstrings;
		numstrings++;
	}

	//newbin.writechararray(newpart, nppos);
	//newbin.writechararray(
	//start to write

	char *buf = new char [insertnp];
	bin.jump(0);
	bin.read(buf, insertnp);
	newbin.directwrite(buf, insertnp);
	delete[] buf;

	int oldentrysize = 0;
	if (fnfound > -1)
	{
		bin.skip(19);
		oldentrysize = 45 + bin.readlong();
		bin.skip(oldentrysize - 23);
	}

	newbin.directwrite(newpart, nppos);

	int remain = bin.getfilesize() - insertnp - oldentrysize;
	buf = new char [remain];
	bin.read(buf, remain);
	newbin.directwrite(buf, remain);
	delete[] buf;

	//print new strings
	for (int i = 0;i < numsubmeshes;i++)
	{
		if (mnfound[i] == (-1))
		{
			int len = strlen(strings[i + 1]);
			newbin.writeshort(len);
			newbin.writechararray(strings[i + 1], len);
		}
	}
	if (lmfound == (-1))
	{
		int len = strlen(strings[numsubmeshes + 1]);
		newbin.writeshort(len);
		newbin.writechararray(strings[numsubmeshes + 1], len);
	}
	if (fnfound == (-1))
	{
		int len = strlen(strings[0]);
		newbin.writeshort(len);
		newbin.writechararray(strings[0], len);
	} 

	newbin.jump(10);
	int newstringsec = (stringsection + nppos - oldentrysize);
	newbin.writelong(newstringsec);
	if (fnfound == (-1))
	{
		newbin.jump(18);
		newbin.writelong(numentries + 1);
	}
	newbin.jump(newstringsec + 5);
	newbin.writelong(numstrings);

	delete[] matnames;
	delete[] stringcontainer;
	delete[] strings;
	delete[] mnfound;
	delete[] filenames;
	delete[] entries;
	delete[] newpart;

	bin.close();
	newbin.close();
	if (replace[0] == 'Y')
	{
		if (remove(binfile) != 0)
		{
			output::error("Couldn't delete bin file", "Couldn't delete bin file");
		}
		if (rename(nbname, binfile) != 0)
		{
			output::error("Couldn't rename new bin file", "Couldn't rename new bin file");
		}
	}

	output::msg("BIN-Datei wurde erfolgreich erstellt", "BIN file has been created succesfully");

	return 0;

	MSG msg;
	HACCEL hAccelTable;

	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EDIT_COMP_MESHES, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Anwendungsinitialisierung ausführen:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDIT_COMP_MESHES));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDIT_COMP_MESHES));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EDIT_COMP_MESHES);
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

void print(void *ptr, int size)
{
	memcpy(&newpart[nppos], ptr, size);
	nppos += size;
}

void pmv(void)
{
	char ltmp[2] = {30, 0};
	print(ltmp, 2);
}

void pver(void)
{
	char ltmp[2] = {201, 0};
	print(ltmp, 2);
}