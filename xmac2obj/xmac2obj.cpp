// xmac2obj.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "rmXmacReader.h"
#include "rmObjWriter.h"
#include <direct.h>

void op_smp(string text)
{
	cout << text << endl;
	if ((text[0] == '*') && (text[1] == '*')) {
		cout << endl;
	}
}

int main(int argc, char **argv)
{
	rmOut::registerOutputFunction(op_smp, 9, 17); // for Release: 9 to 17
	rmOut out("rmStuff.cpp");

		if (argc != 2) {
		OUT_MSG("Start conversion by dragging ONE ._xmac file over the exe symbol!")
		cin.ignore(999, '\n');
		cin.get();
		return 0;
	}

	try {
		string xmacFileName(argv[1]);

		rmU32 lastBackSlash = xmacFileName.find_last_of("\\");
		rmI32 lastPeriod = xmacFileName.find_last_of(".");
		if (lastPeriod < 0) {
			lastPeriod = xmacFileName.size();
		}

		string sceneName = xmacFileName.substr((lastBackSlash + 1), (lastPeriod - lastBackSlash - 1));
		string objPathName = xmacFileName.substr(0, (lastBackSlash + 1)) + sceneName + "\\";

		mkdir(objPathName.c_str());

		rmInFileStream xmac(xmacFileName.c_str());
		rmOutFileStream mtl((objPathName + sceneName + string(".mtl")).c_str());
		rmOutFileStream obj;
		rmScene s;
		s.setName(sceneName);

		rmXmacReader::readXmacFileToScene(&xmac, &s);
		if (s.getNumMaterials() != 0) {
			rmObjWriter::writeSceneToMtlFile(&mtl, &s);
		}
		for (rmU32 i = 0; i != s.getNumNodes(); ++i) {
			if (s.getNode(i).getMesh() != 0) {
				obj.open(objPathName + s.getNode(i).getName() + ".obj");
				rmObjWriter::writeSceneToObjFile(&obj, &s, i);
			}
		}
		OUT_MSG("Conversion succesful!")
	}
	catch (rmException & e) {
		OUT_MSG("An error has occured and the conversion couldn't be completed.")
		OUT_MSG("You might notify me via baltram-lielb@web.de or WoP")
	}

	cin.ignore(999, '\n');
	cin.get();
	return 0;
}