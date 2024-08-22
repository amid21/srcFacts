/*
    xml_parser.hpp

    Include file for XML parsing function
*/

#ifndef INCLUDED_XML_PARSER_HPP
#define INCLUDED_XML_PARSER_HPPs

#include <string>

// general tracing functions

// start trace macro on document
void startTracing();

// end trace macro on document
void stopTracing();

// predicate function determines if inside XML namespace
bool inXMLNS(bool inTag, std::string::const_iterator& cursor);

// parse XML namespace
void parseXMLNS(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inTag, int& depth);

// predicate function determines if inside attribute
bool inAttribute(bool inTag);

// parse attribute
void parseAttribute(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inTag, int& depth, std::string& url, std::string_view startTagLocalName, int& lineCommentCount);

// predicate function determines if inside XML comment
bool inXMLComment(bool inXMLComment, std::string::const_iterator& cursor);

// parse XML comment
void parseXMLComment(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inXMLComment);

// predicate function determines if inside CDATA
bool inCDATA(bool inCDATA, std::string::const_iterator& cursor);

// parse CDATA 
void parseCDATA(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inCDATA, int& textsize, int& loc);

// predicate function determines if inside XML declaration
bool inXMLDeclaration(std::string::const_iterator& cursor);

// parse XML declaration
void parseXMLDeclaration(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes);

// predicate function determines if inside processing instruction
bool inProcessingInstruction(std::string::const_iterator& cursor);

// parse processing instruction
void parseProcessingInstruction(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes);

// predicate function determines if inside end tag
bool inEndTag(std::string::const_iterator& cursor);

// parse end tag
void parseEndTag(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes, int& depth);

// predicate function determines if inside start tag
bool inStartTag(std::string::const_iterator& cursor);

// parse start tag
void parseStartTag(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes, int& depth, int& exprCount, int& declCount, int& commentCount, int& functionCount, int& unitCount, int& classCount, int& returnCount, int& literalCount, bool& isArchive, bool& inTag, std::string& inTagQName, std::string_view& inTagPrefix, std::string_view& inTagLocalName);

// parse character entity references
void parseCharEntityRefs(std::string::const_iterator& cursor, int& textsize);

// parse non-character entity references
void parseNonCER(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, int& loc, int& textsize);

// predicate function checks the length of our buffer for refill
bool isShort(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd);

// refill buffer and adjust iterator
void refillAndAdjust(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes);

// test for end of code
bool isEndOfCode(bool& inXMLComment, bool& inCDATA, std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd);

// parse characters before or after XML
void parseBeforeOrAfter(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd);

// check for Char Entity Refs
bool isCharEntityRef(std::string::const_iterator& cursor);

// check if before or after XML
bool isBeforeOrAfter(int depth);

#endif
