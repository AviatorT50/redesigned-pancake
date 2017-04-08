//***********************************************************************
//CONFIG PARSER
//
//A friend class in C++ can access the "private" and "protected" members of the class in which it is declared as a friend.
//***********************************************************************

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <vector>
#include <string>

#define CFG_INLINE inline

namespace CFGParser 
{
	typedef int					int32;
	typedef unsigned int		uint32; //Four Byte Unsigned Integer
	typedef long long			int64;  //Eight Byte Unsigned Integer
	typedef unsigned long long	uint64; //Eight Byte Unsigned Integer

	enum class CFGParseResult 
	{
		OK = 0,					// No error
		FILE_NOT_FOUND,			// File was not found during load_file()
		IO_ERROR,				// Error reading from file/stream
		OUT_OF_MEMORY,			// Could not allocate memory
		INTERNAL_ERROR,			// Internal error occurred
		NO_DOCUMENT_SECTIONS	// Parsing resulted in a document without element nodes
	};

	//-----------------------------------------------------------------------
	//CFG Node Class
	//-----------------------------------------------------------------------

	class CFGNode 
	{
		friend class CFGSection;
		friend class CFGDocument;
	public:
		CFGNode();	
		CFGNode(std::string sectionNodeName);
		CFGNode(std::string sectionNodeName, std::string value);		
		CFGNode(std::string sectionNodeName, int value);
		CFGNode(std::string sectionNodeName, uint32 value);
		CFGNode(std::string sectionNodeName, int64 value);
		CFGNode(std::string sectionNodeName, uint64 value);
		~CFGNode(){}

		std::string getNodeName() { return m_nodeName; }

		std::string as_string() {return m_nodeAttribute;}
		int32 as_int() {return static_cast<int32>(strtod(&m_nodeAttribute[0], 0));}
		uint32 as_uint() {return static_cast<uint32>(strtod(&m_nodeAttribute[0], 0));}
		int64 as_int64() {return static_cast<int64>(strtod(&m_nodeAttribute[0], 0));}
		uint64 as_uint64() {return static_cast<uint64>(strtod(&m_nodeAttribute[0], 0));}
		float as_float() {return static_cast<float>(strtod(&m_nodeAttribute[0], 0));}
		double as_double() {return static_cast<double>(strtod(&m_nodeAttribute[0], 0));}
		CFG_INLINE bool as_bool();

		CFGNode& operator=(const char* value) { m_nodeAttribute = value; return *this; }
		CFGNode& operator=(std::string value) { m_nodeAttribute = value; return *this; }
		CFGNode& operator=(int value) { m_nodeAttribute = std::to_string(value); return *this; }
		CFGNode& operator=(uint32 value) { m_nodeAttribute = std::to_string(value); return *this; }
		CFGNode& operator=(int64 value) { m_nodeAttribute = std::to_string(value); return *this; }
		CFGNode& operator=(uint64 value) { m_nodeAttribute = std::to_string(value); return *this; }
		CFGNode& operator=(float value) { m_nodeAttribute = std::to_string(value); return *this; }
		CFGNode& operator=(double value) { m_nodeAttribute = std::to_string(value); return *this; }
		CFGNode& operator=(bool value) { m_nodeAttribute = std::to_string(value); return *this; }
	private:
		std::string m_nodeName;
		std::string m_nodeAttribute;
	};

	//-----------------------------------------------------------------------
	//CFG Section Class
	//-----------------------------------------------------------------------

	class CFGSection 
	{
		friend class CFGDocument;
	public:
		CFGSection() { m_sectionName = "Unnamed"; }
		CFGSection(std::string sectName) { m_sectionName = sectName; }
		~CFGSection();
		
		std::string getSectionName() { return m_sectionName; }
		uint32 getNodeCount() { return (uint32)m_sectionNodes.size(); }

		CFGNode& addNode(std::string nodeName);
		CFGNode& addNode(CFGNode node);

		CFGNode& getNode(std::string nodeName);
		CFGNode& getNode(uint32 ID);

		bool removeNode(std::string nodeName);
		bool removeNode(uint32 ID);
	private:
		std::string m_sectionName;
		std::vector<CFGNode> m_sectionNodes;
	};

	//-----------------------------------------------------------------------
	//CFG Document Class
	//
	//Provide basic functionality for working with CFG type files/documents.
	//-----------------------------------------------------------------------

	class CFGDocument 
	{
		public:
			CFGDocument();
			~CFGDocument();

			CFGParseResult loadFile(const char* path);
			bool saveFile(const char* path);

			bool isSectionPresent(std::string sectionName);
			void addSection(CFGSection newSection);
			CFGSection& addSection(std::string sectionName);
			CFGSection& getSection(std::string sectionName);
			bool removeSection(std::string sectionName);

			std::vector<CFGSection> getFileSections() const { return m_fileCFGSections; }
			std::vector<CFGNode> getFileNodes() const { return m_fileNodes; }
		private:
			CFGSection m_documentSection; //This is the section where all file nodes go that are without defined sections.
			std::vector<CFGSection> m_fileCFGSections;
			std::vector<CFGNode> m_fileNodes;
			//There must be also garbage container
	};
}
#endif //CONFIGPARSER_H
