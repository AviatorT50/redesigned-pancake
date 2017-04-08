#include "stdafx.h"
#include <iostream>
#include <string>
#include "ConfigParser.h"

using namespace std;
using namespace CFGParser;

int main() {
	cout << "CFG Parser" << endl;
	cout << "------------------------------------------\n";
	CFGDocument newCFGFile;
	CFGParseResult result = newCFGFile.loadFile("C:\\Users\\akaln\\Documents\\Visual Studio 2015\\Projects\\ConfigFileParser\\sample.cfg"); //sample
	if (result == CFGParseResult::OK) {
		for (uint32 i = 0; i < newCFGFile.getFileSections().size(); i++) {
			if (i != 0) {
				cout << "------------------------------------------\n";
			}	
			cout << "Section Name: " << newCFGFile.getFileSections()[i].getSectionName() << endl;
			for (uint32 j = 0; j < newCFGFile.getFileSections()[i].getNodeCount(); j++) {
				cout << "\tNode: " << newCFGFile.getFileSections()[i].getNode(j).getNodeName()
					<< "\tAttribute: " << newCFGFile.getFileSections()[i].getNode(j).as_string() << endl;
			}
			/*
			for (int j = 0; j < newCFGFile.m_fileCFGSections[i].sectionNodes.size(); j++) {
				cout << "\tNode: " << newCFGFile.m_fileCFGSections[i].sectionNodes[j].nodeName
					<< "\tAttribute: " << newCFGFile.m_fileCFGSections[i].sectionNodes[j].as_string() << endl;
			}
			*/
		}
	}
	cout << "------------------------------------------\n";
	


	newCFGFile.removeSection("Area.001");


	//Two different ways of adding new sections and nodes to the cfg file.
	newCFGFile.getSection("Area.X").getNode("Test1") = 15.0f;
	newCFGFile.getSection("Area.X").getNode("Test2") = 25.0f;
	
	newCFGFile.getSection("Area.0021").getNode("Layer1") = 10;
	newCFGFile.getSection("Area.0021").getNode("Layer2") = 0;


	CFGSection section002 = newCFGFile.getSection("Area.002");
	newCFGFile.getSection("Area.002").addNode("SuperNode") = 100;
	newCFGFile.getSection("Area.002").addNode("SuperNode2") = 10.15f;
	newCFGFile.getSection("Area.002").addNode("SuperNodeText") = std::string("EMPTY");
	
	bool saved = newCFGFile.saveFile("C:\\Users\\akaln\\Documents\\Visual Studio 2015\\Projects\\ConfigFileParser\\scenery2.cfg");
	if (saved)
		std::cout << "File has been saved.\n";
	_gettch();
    return 0;
}
