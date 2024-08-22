/*
    XMLParser.hpp

    Include file for XML parsing class
*/

#ifndef INCLUDED_XMLPARSER_HPP
#define INCLUDED_XMLPARSER_HPP

#include <string>
#include <iterator>
#include <functional>
#include <string_view>
#include <optional>

class XMLParser
{

private:
    std::string buffer;
    std::string::const_iterator cursor;
    std::string::const_iterator cursorEnd;
    int depth;
    bool inTag;
    std::string inTagQName;
    std::string_view inTagPrefix;
    std::string_view inTagLocalName;
    bool isInCDATA;
    bool isInXMLComment;
    long totalBytes;

    std::function<void(int depth, std::string_view qName, std::string_view prefix, std::string_view localName)> handleStartTag;
    std::function<void(int depth, std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value)> handleAttribute;
    std::function<void(int depth, std::string_view characters)> handleNonCER;
    std::function<void(int depth, std::string_view characters)> handleCDATA;
    std::function<void(int depth, std::string_view characters)> handleCER;
    std::function<void(int depth, std::string_view prefix, std::string_view uri)> handleNamespace;
    std::function<void(int depth, std::string_view comment)> handleComment;
    std::function<void(int depth, std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone)> handleDeclaration;
    std::function<void(int depth, std::string_view target, std::string_view data)> handlePI;
    std::function<void(int depth, std::string_view prefix, std::string_view qName, std::string_view localName)> handleEndTag;
    std::function<void(int depth)> handleStart;
    std::function<void(int depth)> handleEnd;

public:
    // parameterized XMLParser constructor
    XMLParser(
        std::function<void(int depth, std::string_view qName, std::string_view prefix, std::string_view localName)> handleStartTag, 
        std::function<void(int depth, std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value)> handleAttribute,
        std::function<void(int depth, std::string_view characters)> handleNonCER,
        std::function<void(int depth, std::string_view characters)> handleCDATA,
        std::function<void(int depth, std::string_view characters)> handleCER,
        std::function<void(int depth, std::string_view prefix, std::string_view uri)> handleNamespace,
        std::function<void(int depth, std::string_view comment)> handleComment,
        std::function<void(int depth, std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone)> handleDeclaration,
        std::function<void(int depth, std::string_view target, std::string_view data)> handlePI,
        std::function<void(int depth, std::string_view prefix, std::string_view qName, std::string_view localName)> handleEndTag,
        std::function<void(int depth)> handleStart,
        std::function<void(int depth)> handleEnd
    );

private:
    // predicate function determines if inside XML namespace
    bool inXMLNS();

    // parse XML namespace
    void parseXMLNS();

    // predicate function determines if inside attribute
    bool inAttribute();

    // parse attribute
    void parseAttribute();

    // predicate function determines if inside XML comment
    bool inXMLComment();

    // parse XML comment
    void parseXMLComment();

    // predicate function determines if inside CDATA
    bool inCDATA();

    // parse CDATA
    void parseCDATA();

    // predicate function determines if inside XML declaration
    bool inXMLDeclaration();

    // parse XML declaration
    void parseXMLDeclaration();

    // predicate function determines if inside processing instruction
    bool inProcessingInstruction();

    // parse processing instruction
    void parseProcessingInstruction();

    // predicate function determines if inside end tag
    bool inEndTag();

    // parse end tag
    void parseEndTag();

    // predicate function determines if inside start tag
    bool inStartTag();

    // parse start tag
    void parseStartTag();

    // parse character entity references
    void parseCharEntityRefs();

    // parse non-character entity references
    void parseNonCER();

    // predicate function checks the length of our buffer for refill
    bool isShort();

    // refill buffer and adjust iterator
    void refillAndAdjust();

    // test for end of code
    bool isEndOfCode();

    // parse characters before or after XML
    void parseBeforeOrAfter();

    // check for Char Entity Refs
    bool isCharEntityRef();

    // check if before or after XML
    bool isBeforeOrAfter();

    // start trace macro on document
    void startTracing();

    // end trace macro on document
    void stopTracing();

public:
    // Parsing loop with nested if's
    void parse();

    // Get method for total bytes
    long getTotalBytes();
};

#endif
