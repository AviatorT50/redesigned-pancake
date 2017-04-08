#include "stdafx.h"
#include "ConfigParser.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

#include <memory>

using std::string;

namespace CFGParser 
{

	//-----------------------------------------------------------------------
	//Additional String Helper Methods
	//-----------------------------------------------------------------------

	std::string trimLeft(const std::string& s, const std::string& cutset){
		// TODO(bill): Make more efficient
		const uint32 slen = s.size(); //len(s);
		const  uint32 clen = cutset.size(); //len(cutset);
		if (s.size() == 0)
			return s;
		int pos = -1;
		bool t = false;

		for (uint32 i = 0; i < slen; i++) {
			for (uint32 j = 0; j < clen; j++) {
				if (s[i] == cutset[j]) {
					t = true;
					pos = i;
				}
			}
			if (!t)
				break;
			t = false;
		}

		if (pos != -1)
			return s.substr(pos + 1, slen); //std::string::substring(s, pos + 1, slen);
		return s;
	}

	std::string trimRight(const std::string& s, const std::string& cutset) {
		// TODO(bill): Make more efficient
		const uint32 slen = s.size(); // len(s);
		const uint32 clen = cutset.size(); //len(cutset);
		if (slen == 0)
			return s;
		int pos = -1;
		bool t = false; //was trimmed

		for (uint32 i = slen - 1; i >= 0; i--) {
			for (uint32 j = 0; j < clen; j++) {
				if (s[i] == cutset[j]) {
					t = true;
					pos = i;
				}
			}
			if (!t)
				break;
			t = false;
		}

		if (pos != -1)
			return s.substr(0, pos);//substring(s, 0, pos);
		return s;
	}

	std::string trim(const std::string& s, const std::string& cutset){
		return trimLeft(trimRight(s, cutset), cutset);
	}

	std::string trimSpace(const std::string& s) { return trim(s, " \t\n\v\f\r"); }

	std::string trimComment(const std::string& s) {
		return trimRight(s, "//");
	}

	std::string removeCommentIfPresent(const std::string& s) {
		bool m_contains = false;
		uint32 rPos = 0;
		for (uint32 i = s.size() - 1; i != 0; i--) {
			if (i > 0 && s[i] == '/' && s[i - 1] == '/') {
				rPos = i - 1;
				m_contains = true;
			}
		}
		if (m_contains) {
			return s.substr(0, rPos);
		}	
		return s;
	}

	bool contains(const std::string& s, char v) {
		bool m_conains = false;
		for (uint32 i = 0; i < s.size(); i++) {
			if (s[i] == v) {
				m_conains = true;
				break;
			}
		}
		return m_conains;
	}

	void splitNew(const std::string& s, std::vector<string>& result, const std::string& delimiters = " ") {
		std::string::size_type a, b = 0;
		for (;;) {
			a = s.find_first_not_of(delimiters, b);
			b = s.find_first_of(delimiters, a);
			if (std::string::npos == b && std::string::npos == a) break;
			result.push_back(s.substr(a, b - a));
		}
	}

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	//Method will check for any empty lines, commented lines
	bool skipLine(std::string line) {
		//Skip any empty lines
		if (line.length() == 0 || line[0] == '\r') {
			return true;
		}

		//Skip comments, it will be also necessery to check if a line.length() is 2 ...
		if (line[0] == '#' || line.length() >= 2 && line[0] == '/' && line[1] == '/') {
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------
	//CFG Document Class Methods
	//-----------------------------------------------------------------------

	CFGDocument::CFGDocument() {}
	
	CFGDocument::~CFGDocument() 
	{
		m_fileCFGSections.clear();
		m_fileNodes.clear();
	}

	CFGParseResult CFGDocument::loadFile(const char* path) 
	{
		std::ifstream file;
		file.open(std::string(path), std::ios::in | std::ios::binary); 
		if (file.good()) 
		{
			std::string line;
			//std::string nodeLine;
			std::string currentSectionName;

			bool sectionStarted = false;
			bool commentedSection = false;

			while (file.good()) 
			{
				getline(file, line);
			
				//Skip any empty lines
				if (line.length() == 0 || line[0] == '\r') {
					continue;
				}

				//Skip comments, it will be also necessery to check if a line.length() is 2 ...
				if (line[0] == '#' || line[0] == '/' && line[1] == '/') {
					continue;
				}

				//This kind of emulates this situation: "attribute=value*/".
				if (line.length() >= 2 && line[line.length() - 3] == '*' && line[line.length() - 2] == '/') {
					commentedSection = false;
					continue;
				}

				// /**/ Comment type
				if (line.length() >= 2 && line[0] == '*' && line[1] == '/') {
					commentedSection = false;
					continue;
				}

				if (line.length() >= 2 && line[0] == '/' && line[1] == '*') {
					commentedSection = true;
					continue;
				}
	
				if (commentedSection == true) {continue;}

				//Check if there is a comment present in the line
				line = removeCommentIfPresent(line);

				//Check for file sections
				if (contains(line, '='))
				{
					CFGNode newNode;
					std::vector<std::string> splitedLine = split(line, '=');
					if (splitedLine.size() > 1) {
						newNode.m_nodeName = trimSpace(splitedLine[0]);
						newNode.m_nodeAttribute = trim(trimSpace(splitedLine[1]), "\"\"");
					}
					else { //node without set attribute
						newNode.m_nodeName = line;
						newNode.m_nodeAttribute = std::to_string(0);
					}
					m_fileNodes.push_back(newNode);
				}

				//Section start/or a new section start
				if (line[0] == '[') 
				{
					//Determine where is ']' located and even if it is there.
					int closeBracketPos = 0;
					bool closeBracket = false;
					for (uint32 l = 0; l < line.size(); l++)
					{
						if (line[l] == ']') {
							closeBracketPos = l;
							closeBracket = true;
						}
					}

					if (!closeBracket) {
						//If there is no ']' / close bracket, then it is incomplete section definition.
						continue;
					}

					if (closeBracketPos != 0) {
						line = line.substr(0, closeBracketPos + 1);
					}

					currentSectionName = trim(trimSpace(line), "[]");
					CFGSection newSection;
					newSection.m_sectionName = currentSectionName;
					//std::cout << currentSectionName << std::endl;

					std::string nodeLine;
					std::streampos previousPos = file.tellg();//Store the position
					while (file.good() && nodeLine[0] != '[') 
					{
						previousPos = file.tellg();
						getline(file, nodeLine);

						//Skip any empty lines
						if (nodeLine.length() == 0 || nodeLine[0] == '\r') {
							continue;
						}
						//Skip comments
						if (nodeLine[0] == '#' || nodeLine[0] == '/' && nodeLine[1] == '/' || nodeLine[0] =='[') {
							continue;
						}

						//This kind of emulates this situation: "attribute=value*/".
						if (nodeLine.length() >= 2 && nodeLine[nodeLine.length() - 3] == '*' && nodeLine[nodeLine.length() - 2] == '/') {
							commentedSection = false;
							continue;
						}

						if (nodeLine.length() >= 2 && nodeLine[0] == '/' && nodeLine[1] == '*' || nodeLine.length() >= 2 && nodeLine[0] == '*' && nodeLine[1] == '/') {
							if (nodeLine[0] == '/' && nodeLine[1] == '*') {
								commentedSection = true;
							}
							if (nodeLine[0] == '*' && nodeLine[1] == '/') {
								commentedSection = false;
							}
							continue;
						}

						if (commentedSection == true) { continue; }	

						//Check if there is a comment present in the line
						nodeLine = removeCommentIfPresent(nodeLine);

						CFGNode newNode;
						if (contains(nodeLine, '=')) { //Node has attribute, need to split the line
							std::vector<std::string> splitedLine = split(nodeLine, '=');
							if (splitedLine.size() > 1) {
								newNode.m_nodeName = trimSpace(splitedLine[0]);
								newNode.m_nodeAttribute = trim(trimSpace(splitedLine[1]), "\"\"");
							}
						}
						else { //node without set attribute
							newNode.m_nodeName = nodeLine;
						}
						newSection.m_sectionNodes.push_back(newNode);
					}
					m_fileCFGSections.push_back(newSection);
					file.seekg(previousPos);//Go back one line so the cycle can start again.							
				}		
			}
			file.close();

			if (m_fileCFGSections.empty()) {
				return CFGParseResult::NO_DOCUMENT_SECTIONS;
			}
			return CFGParseResult::OK;
		}
		else {
			file.close();
			return CFGParseResult::FILE_NOT_FOUND;
		}
	}

	bool CFGDocument::saveFile(const char* path) 
	{
		if (m_fileCFGSections.size() == 0) {
			return false;
		}
		std::ofstream file;
		file.open(std::string(path), std::ios::out | std::ios::binary);
		if (file.good()) {
			//Write file nodes
			for (uint32 i = 0; i < m_fileNodes.size(); i++) {
				if (i != 0) {
					file << "\n";
				}

				file <<  m_fileNodes[i].m_nodeName + "=" + m_fileNodes[i].m_nodeAttribute;

				if (i == m_fileNodes.size() - 1) {
					file << "\n\n";
				}
			}

			//Write file sections
			for (uint32 i = 0; i < m_fileCFGSections.size(); i++) {
				if (i != 0 ) {
					file << "\n\n";
				}
				file << ("[" + m_fileCFGSections[i].m_sectionName + "]");
				for (uint32 j = 0; j < m_fileCFGSections[i].m_sectionNodes.size(); j++) {
					file << "\n" + m_fileCFGSections[i].m_sectionNodes[j].m_nodeName + "=" + m_fileCFGSections[i].m_sectionNodes[j].as_string();
				}		
			}
			file.close();
			return true;
		}
		return false;
	}

	bool CFGDocument::isSectionPresent(std::string sectionName)
	{
		for (uint32 i = 0; i < m_fileCFGSections.size(); i++) {
			if (m_fileCFGSections[i].m_sectionName == sectionName) {
				return true;
			}
		}
		return false;
	}

	void CFGDocument::addSection(CFGSection newSection)
	{
		m_fileCFGSections.push_back(newSection);
	}

	CFGSection& CFGDocument::addSection(std::string sectionName)
	{
		CFGSection* newSection = new CFGSection(sectionName);
		m_fileCFGSections.push_back(*newSection);
		return *newSection;
	}

	CFGSection& CFGDocument::getSection(std::string sectionName) 
	{
		for (uint32 i = 0; i < m_fileCFGSections.size(); i++) {
			if (m_fileCFGSections[i].m_sectionName == sectionName) {
				return m_fileCFGSections[i];
			}
		}
		CFGSection newSection(sectionName);
		m_fileCFGSections.push_back(newSection);
		return m_fileCFGSections[m_fileCFGSections.size() - 1]; //Else returns empty section.
	}

	bool CFGDocument::removeSection(std::string sectionName) 
	{
		for (uint32 i = 0; i < m_fileCFGSections.size(); i++) {
			if (m_fileCFGSections[i].m_sectionName == sectionName) {
				m_fileCFGSections.erase(m_fileCFGSections.begin() + i);
				return true;
			}
		}
		return false;
	}

	//-----------------------------------------------------------------------
	//CFG Attribute Methods
	//-----------------------------------------------------------------------

	CFGNode::CFGNode()
	{
		m_nodeName = "Unnamed";
		m_nodeAttribute = "0";
	}

	CFGNode::CFGNode(std::string sectionNodeName)
	{
		m_nodeName = sectionNodeName;
		m_nodeAttribute = "0";
	}

	CFGNode::CFGNode(std::string sectionNodeName, std::string value)
	{
		m_nodeName = sectionNodeName;
		m_nodeAttribute = value;
	}

	CFGNode::CFGNode(std::string sectionNodeName, int value)
	{
		m_nodeName = sectionNodeName;
		m_nodeAttribute = std::to_string(value);
	}

	CFGNode::CFGNode(std::string sectionNodeName, uint32 value)
	{
		m_nodeName = sectionNodeName;
		m_nodeAttribute = std::to_string(value);
	}

	CFGNode::CFGNode(std::string sectionNodeName, int64 value)
	{
		m_nodeName = sectionNodeName;
		m_nodeAttribute = std::to_string(value);
	}

	CFGNode::CFGNode(std::string sectionNodeName, uint64 value)
	{
		m_nodeName = sectionNodeName;
		m_nodeAttribute = std::to_string(value);
	}

	bool CFGNode::as_bool()
	{
		// only look at first char
		char first = m_nodeAttribute[0];

		// 1*, 1* - 9* , t* (true), T* (True), y* (yes), Y* (YES)
		if (first == '1' || first >= 0x31 && first <= 0x39 || first == 't' || first == 'T' || first == 'y' || first == 'Y')
		{
			return true;
		}
		return false;
		//return static_cast<bool>(strtod(&m_nodeAttribute[0], 0));
	}

	//-----------------------------------------------------------------------
	//CFG Section Methods
	//-----------------------------------------------------------------------

	CFGSection::~CFGSection()
	{
		m_sectionNodes.clear();
	}

	CFGNode & CFGSection::addNode(std::string nodeName)
	{
		CFGNode newNode(nodeName);
		newNode = 0;
		m_sectionNodes.push_back(newNode);
		return m_sectionNodes[m_sectionNodes.size() - 1];
	}
	CFGNode & CFGSection::addNode(CFGNode node)
	{
		m_sectionNodes.push_back(node);
		return m_sectionNodes[m_sectionNodes.size() - 1];
	}
	CFGNode & CFGSection::getNode(std::string nodeName)
	{
		for (uint32 i = 0; i < m_sectionNodes.size(); i++) {
			if (m_sectionNodes[i].m_nodeName == nodeName) {
				return m_sectionNodes[i];
			}
		}
		CFGNode newCFGSectionNode(nodeName);
		m_sectionNodes.push_back(newCFGSectionNode);
		return m_sectionNodes[m_sectionNodes.size() - 1];
	}

	CFGNode & CFGSection::getNode(uint32 ID)
	{
		if (ID < 0 || ID > m_sectionNodes.size() - 1) {
			// 				CFGSectionNode* newCFGSectionNode = new CFGSectionNode;
			// 				sectionNodes.push_back(*newCFGSectionNode);
			CFGNode newCFGSectionNode;
			m_sectionNodes.push_back(newCFGSectionNode);
			return m_sectionNodes[m_sectionNodes.size() - 1];
		}
		else {
			return m_sectionNodes[ID];
		}
	}
	bool CFGSection::removeNode(std::string nodeName)
	{
		for (uint32 i = 0; i < m_sectionNodes.size(); i++) {
			if (m_sectionNodes[i].m_nodeName == nodeName) {
				m_sectionNodes.erase(m_sectionNodes.begin() + i);
				return true;
			}
		}
		return false;
	}
	bool CFGSection::removeNode(uint32 ID)
	{
		if (ID < 0 || ID > m_sectionNodes.size() - 1) {
			return false;
		}
		m_sectionNodes.erase(m_sectionNodes.begin() + ID);
		return true;
	}
}