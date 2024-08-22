/*
    xml_parser.cpp

    Implementation file for XML parsing function
*/

#include "xml_parser.hpp"
#include "refillBuffer.hpp"
#include <iostream>
#include <algorithm>
#include <bitset>
#include <string.h>

using namespace std::literals::string_view_literals;

std::bitset<128> tagNameMask("00000111111111111111111111111110100001111111111111111111111111100000001111111111011000000000000000000000000000000000000000000000");

// trace parsing
#ifdef TRACE
#undef TRACE
#define HEADER(m) std::clog << std::setw(10) << std::left << m <<"\t"
#define FIELD(l, n) l << ":|" << n << "| "
#define TRACE0(m)
#define TRACE1(m, l1, n1) HEADER(m) << FIELD(l1,n1) << '\n';
#define TRACE2(m, l1, n1, l2, n2) HEADER(m) << FIELD(l1,n1) << FIELD(l2,n2) << '\n';
#define TRACE3(m, l1, n1, l2, n2, l3, n3) HEADER(m) << FIELD(l1,n1) << FIELD(l2,n2) << FIELD(l3,n3) << '\n';
#define TRACE4(m, l1, n1, l2, n2, l3, n3, l4, n4) HEADER(m) << FIELD(l1,n1) << FIELD(l2,n2) << FIELD(l3,n3) << FIELD(l4,n4) << '\n';
#define GET_TRACE(_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME
#define TRACE(...) GET_TRACE(__VA_ARGS__, TRACE4, _UNUSED, TRACE3, _UNUSED, TRACE2, _UNUSED, TRACE1, _UNUSED, TRACE0)(__VA_ARGS__)
#else
#define TRACE(...)
#endif

// start tracing on document
void startTracing()
{
    TRACE("START DOCUMENT");
}

// stop tracing on document
void stopTracing()
{
    TRACE("END DOCUMENT");
}

// predicate function determines if inside XML namespace
bool inXMLNS(bool inTag, std::string::const_iterator& cursor)
{
    return (inTag && (strncmp(std::addressof(*cursor), "xmlns", 5) == 0) && (cursor[5] == ':' || cursor[5] == '='));
}

// parse XML namespace
void parseXMLNS(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inTag, int& depth)
{
    std::advance(cursor, 5);
    const auto nameEnd = std::find(cursor, cursorEnd, '=');
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    int prefixSize = 0;
    if (*cursor == ':') {
        std::advance(cursor, 1);
        prefixSize = std::distance(cursor, nameEnd);
    }
    const std::string_view prefix(std::addressof(*cursor), prefixSize);
    cursor = std::next(nameEnd);
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (cursor == cursorEnd) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    const auto delimiter = *cursor;
    if (delimiter != '"' && delimiter != '\'') {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    std::advance(cursor, 1);
    const auto valueEnd = std::find(cursor, cursorEnd, delimiter);
    if (valueEnd == cursorEnd) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    const std::string_view uri(std::addressof(*cursor), std::distance(cursor, valueEnd));
    TRACE("NAMESPACE", "prefix", prefix, "uri", uri);
    cursor = std::next(valueEnd);
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (*cursor == '>') {
        std::advance(cursor, 1);
        inTag = false;
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", inTagPrefix, "qName", inTagQName, "localName", inTagLocalName);
        inTag = false;
    }
}

// predicate function determines if inside attribute
bool inAttribute(bool inTag)
{
    return inTag;
}

// parse attribute
void parseAttribute(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inTag, int& depth, std::string& url, std::string_view startTagLocalName, int& lineCommentCount)
{
    const auto nameEnd = std::find_if_not(cursor, cursorEnd, [] (char c) { return tagNameMask[c]; });
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : Empty attribute name" << '\n';
        exit(1);
    }
    const std::string_view qName(std::addressof(*cursor), std::distance(cursor, nameEnd));
    auto colonPosition = qName.find(':');
    if (colonPosition == 0) {
        std::cerr << "parser error : Invalid attribute name " << qName << '\n';
        exit(1);
    }
    if (colonPosition == std::string::npos)
        colonPosition = 0;
    const std::string_view prefix(std::addressof(*qName.cbegin()), colonPosition);
    if (colonPosition != 0)
        colonPosition += 1;
    const std::string_view localName(std::addressof(*qName.cbegin()) + colonPosition, qName.size() - colonPosition);
    cursor = nameEnd;
    if (isspace(*cursor))
        cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (cursor == cursorEnd) {
        std::cerr << "parser error : attribute " << qName << " incomplete attribute\n";
        exit(1);
    }
    if (*cursor != '=') {
        std::cerr << "parser error : attribute " << qName << " missing =\n";
        exit(1);
    }
    std::advance(cursor, 1);
    if (isspace(*cursor))
        cursor = std::find_if_not(cursor, cursorEnd, isspace);
    const auto delimiter = *cursor;
    if (delimiter != '"' && delimiter != '\'') {
        std::cerr << "parser error : attribute " << qName << " missing delimiter\n";
        exit(1);
    }
    std::advance(cursor, 1);
    auto valueEnd = std::find(cursor, cursorEnd, delimiter);
    if (valueEnd == cursorEnd) {
        std::cerr << "parser error : attribute " << qName << " missing delimiter\n";
        exit(1);
    }
    const std::string_view value(std::addressof(*cursor), std::distance(cursor, valueEnd));
    if (localName == "url"sv)
        url = value;
    TRACE("ATTRIBUTE", "prefix", prefix, "qname", qName, "localName", localName, "value", value);
    if (startTagLocalName == "comment"sv && value == "line"sv) {
        ++lineCommentCount;
    }
    cursor = std::next(valueEnd);
    if (isspace(*cursor))
        cursor = std::find_if_not(std::next(cursor), cursorEnd, isspace);
    if (*cursor == '>') {
        std::advance(cursor, 1);
        inTag = false;
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", inTagPrefix, "qName", inTagQName, "localName", inTagLocalName);
        inTag = false;
    }
}

// predicate function determines if inside XML comment
bool inXMLComment(bool inXMLComment, std::string::const_iterator& cursor)
{
    return (inXMLComment || (cursor[1] == '!' && *cursor == '<' && cursor[2] == '-' && cursor[3] == '-'));
}

// parse XML comment
void parseXMLComment(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inXMLComment)
{
    if (cursor == cursorEnd) {
        std::cerr << "parser error : Unterminated XML comment\n";
        exit(1);
    }
    if (!inXMLComment)
        std::advance(cursor, 4);
    constexpr std::string_view endComment = "-->"sv;
    auto tagEnd = std::search(cursor, cursorEnd, endComment.begin(), endComment.end());
    inXMLComment = tagEnd == cursorEnd;
    const std::string_view comment(std::addressof(*cursor), std::distance(cursor, tagEnd));
    TRACE("COMMENT", "comment", comment);
    if (!inXMLComment)
        cursor = std::next(tagEnd, endComment.size());
    else
        cursor = tagEnd;
}

// predicate function determines if inside CDATA
bool inCDATA(bool inCDATA, std::string::const_iterator& cursor)
{
    return (inCDATA || (cursor[1] == '!' && *cursor == '<' && cursor[2] == '[' && (strncmp(std::addressof(cursor[3]), "CDATA[", 6) == 0)));
}

// parse CDATA
void parseCDATA(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, bool& inCDATA, int& textsize, int& loc)
{
    if (cursor == cursorEnd) {
        std::cerr << "parser error : Unterminated CDATA\n";
        exit(1);
    }
    constexpr std::string_view endCDATA = "]]>"sv;
    if (!inCDATA)
        std::advance(cursor, 9);
    auto tagEnd = std::search(cursor, cursorEnd, endCDATA.begin(), endCDATA.end());
    inCDATA = tagEnd == cursorEnd;
    const std::string_view characters(std::addressof(*cursor), std::distance(cursor, tagEnd));
    TRACE("CDATA", "characters", characters);
    textsize += static_cast<int>(characters.size());
    loc += static_cast<int>(std::count(characters.begin(), characters.end(), '\n'));
    cursor = std::next(tagEnd, endCDATA.size());
    if (!inCDATA)
        cursor = std::next(tagEnd, endCDATA.size());
    else
        cursor = tagEnd;
}

// predicate function determines if inside XML declaration
bool inXMLDeclaration(std::string::const_iterator& cursor)
{
    return (cursor[1] == '?' && *cursor == '<' && (strncmp(std::addressof(*cursor), "<?xml ", 6) == 0));
}

// parse XML declaration
void parseXMLDeclaration(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes)
{
    constexpr std::string_view startXMLDecl = "<?xml";
    constexpr std::string_view endXMLDecl = "?>";
    auto tagEnd = std::find(cursor, cursorEnd, '>');
    if (tagEnd == cursorEnd) {
        auto bytesRead = refillBuffer(cursor, cursorEnd, buffer);
        if (bytesRead < 0) {
            std::cerr << "parser error : File input error\n";
            exit(1);
        }
        totalBytes += bytesRead;
        if ((tagEnd = std::find(cursor, cursorEnd, '>')) == cursorEnd) {
            std::cerr << "parser error: Incomplete XML declaration\n";
            exit(1);
        }
    }
    std::advance(cursor, startXMLDecl.size());
    cursor = std::find_if_not(cursor, tagEnd, isspace);

    // parse required version
    if (cursor == tagEnd) {
        std::cerr << "parser error: Missing space after before version in XML declaration\n";
        exit(1);
    }
    auto nameEnd = std::find(cursor, tagEnd, '=');
    const std::string_view attr(std::addressof(*cursor), std::distance(cursor, nameEnd));
    cursor = std::next(nameEnd);
    const auto delimiter = *cursor;
    if (delimiter != '"' && delimiter != '\'') {
        std::cerr << "parser error: Invalid start delimiter for version in XML declaration\n";
        exit(1);
    }
    std::advance(cursor, 1);
    auto valueEnd = std::find(cursor, tagEnd, delimiter);
    if (valueEnd == tagEnd) {
        std::cerr << "parser error: Invalid end delimiter for version in XML declaration\n";
        exit(1);
    }
    if (attr != "version"sv) {
        std::cerr << "parser error: Missing required first attribute version in XML declaration\n";
        exit(1);
    }
    const std::string_view version(std::addressof(*cursor), std::distance(cursor, valueEnd));
    cursor = std::next(valueEnd);
    cursor = std::find_if_not(cursor, tagEnd, isspace);

    // parse optional encoding and standalone attributes
    std::optional<std::string_view> encoding;
    std::optional<std::string_view> standalone;
    if (cursor != (tagEnd - 1)) {
        nameEnd = std::find(cursor, tagEnd, '=');
        if (nameEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute in XML declaration\n";
            exit(1);
        }
        const std::string_view attr2(std::addressof(*cursor), std::distance(cursor, nameEnd));
        cursor = std::next(nameEnd);
        auto delimiter2 = *cursor;
        if (delimiter2 != '"' && delimiter2 != '\'') {
            std::cerr << "parser error: Invalid end delimiter for attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        std::advance(cursor, 1);
        valueEnd = std::find(cursor, tagEnd, delimiter2);
        if (valueEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        if (attr2 == "encoding"sv) {
            encoding = std::string_view(std::addressof(*cursor), std::distance(cursor, valueEnd));
        } else if (attr2 == "standalone"sv) {
            standalone = std::string_view(std::addressof(*cursor), std::distance(cursor, valueEnd));
        } else {
            std::cerr << "parser error: Invalid attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        cursor = std::next(valueEnd);
        cursor = std::find_if_not(cursor, tagEnd, isspace);
    }
    if (cursor != (tagEnd - endXMLDecl.size() + 1)) {
        nameEnd = std::find(cursor, tagEnd, '=');
        if (nameEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute in XML declaration\n";
            exit(1);
        }
        const std::string_view attr2(std::addressof(*cursor), std::distance(cursor, nameEnd));
        cursor = std::next(nameEnd);
        const auto delimiter2 = *cursor;
        if (delimiter2 != '"' && delimiter2 != '\'') {
            std::cerr << "parser error: Invalid end delimiter for attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        std::advance(cursor, 1);
        valueEnd = std::find(cursor, tagEnd, delimiter2);
        if (valueEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        if (!standalone && attr2 == "standalone"sv) {
            standalone = std::string_view(std::addressof(*cursor), std::distance(cursor, valueEnd));
        } else {
            std::cerr << "parser error: Invalid attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        cursor = std::next(valueEnd);
        cursor = std::find_if_not(cursor, tagEnd, isspace);
    }
    TRACE("XML DECLARATION", "version", version, "encoding", (encoding ? *encoding : ""), "standalone", (standalone ? *standalone : ""));
    std::advance(cursor, endXMLDecl.size());
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
}

// predicate function determines if inside processing instruction
bool inProcessingInstruction(std::string::const_iterator& cursor)
{
    return (cursor[1] == '?' && *cursor == '<');
}

// parse processing instruction
void parseProcessingInstruction(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes)
{
    constexpr std::string_view endPI = "?>";
    auto tagEnd = std::search(cursor, cursorEnd, endPI.begin(), endPI.end());
    if (tagEnd == cursorEnd) {
        auto bytesRead = refillBuffer(cursor, cursorEnd, buffer);
        if (bytesRead < 0) {
            std::cerr << "parser error : File input error\n";
            exit(1);
        }
        totalBytes += bytesRead;
        if ((tagEnd = std::search(cursor, cursorEnd, endPI.begin(), endPI.end())) == cursorEnd) {
            std::cerr << "parser error: Incomplete XML declaration\n";
            exit(1);
        }
    }
    std::advance(cursor, 2);
    auto nameEnd = std::find_if_not(cursor, tagEnd, [] (char c) { return tagNameMask[c]; });
    if (nameEnd == tagEnd) {
        std::cerr << "parser error : Unterminated processing instruction '" << std::string_view(std::addressof(*cursor), std::distance(cursor, nameEnd)) << "'\n";
        exit(1);
    }
    const std::string_view target(std::addressof(*cursor), std::distance(cursor, nameEnd));
    cursor = std::find_if_not(nameEnd, tagEnd, isspace);
    const std::string_view data(std::addressof(*cursor), std::distance(cursor, tagEnd));
    TRACE("PI", "target", target, "data", data);
    cursor = tagEnd;
    std::advance(cursor, 2);
}

// predicate function determines if inside end tag
bool inEndTag(std::string::const_iterator& cursor)
{
    return (cursor[1] == '/' && *cursor == '<');
}

// parse end tag
void parseEndTag(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes, int& depth)
{
    if (std::distance(cursor, cursorEnd) < 100) {
        auto tagEnd = std::find(cursor, cursorEnd, '>');
        if (tagEnd == cursorEnd) {
            auto bytesRead = refillBuffer(cursor, cursorEnd, buffer);
            if (bytesRead < 0) {
                std::cerr << "parser error : File input error\n";
                exit(1);
            }
            totalBytes += bytesRead;
            if ((tagEnd = std::find(cursor, cursorEnd, '>')) == cursorEnd) {
                std::cerr << "parser error: Incomplete element end tag\n";
                exit(1);
            }
        }
    }
    std::advance(cursor, 2);
    if (*cursor == ':') {
        std::cerr << "parser error : Invalid end tag name\n";
        exit(1);
    }
    auto nameEnd = std::find_if_not(cursor, cursorEnd, [] (char c) { return tagNameMask[c]; });
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : Unterminated end tag '" << std::string_view(std::addressof(*cursor), std::distance(cursor, nameEnd)) << "'\n";
        exit(1);
    }
    size_t colonPosition = 0;
    if (*nameEnd == ':') {
        colonPosition = std::distance(cursor, nameEnd);
        nameEnd = std::find_if_not(std::next(nameEnd), cursorEnd, [] (char c) { return tagNameMask[c]; });
    }
    const std::string_view prefix(std::addressof(*cursor), colonPosition);
    const std::string_view qName(std::addressof(*cursor), std::distance(cursor, nameEnd));
    if (qName.empty()) {
        std::cerr << "parser error: EndTag: invalid element name\n";
        exit(1);
    }
    if (colonPosition)
        ++colonPosition;
    const std::string_view localName(std::addressof(*cursor) + colonPosition, std::distance(cursor, nameEnd) - colonPosition);
    cursor = std::next(nameEnd);
    --depth;
    TRACE("END TAG", "prefix", prefix, "qName", qName, "localName", localName);
}

// predicate function determines if inside start tag
bool inStartTag(std::string::const_iterator& cursor)
{
    return (*cursor == '<');
}

// parse start tag
void parseStartTag(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes, int& depth, int& exprCount, int& declCount, int& commentCount, int& functionCount, int& unitCount, int& classCount, int& returnCount, int& literalCount, bool& isArchive, bool& inTag, std::string& inTagQName, std::string_view& inTagPrefix, std::string_view& inTagLocalName)
{
    if (std::distance(cursor, cursorEnd) < 200) {
        auto tagEnd = std::find(cursor, cursorEnd, '>');
        if (tagEnd == cursorEnd) {
            auto bytesRead = refillBuffer(cursor, cursorEnd, buffer);
            if (bytesRead < 0) {
                std::cerr << "parser error : File input error\n";
                exit(1);
            }
            totalBytes += bytesRead;
            if ((tagEnd = std::find(cursor, cursorEnd, '>')) == cursorEnd) {
                std::cerr << "parser error: Incomplete element start tag\n";
                exit(1);
            }
        }
    }
    std::advance(cursor, 1);
    if (*cursor == ':') {
        std::cerr << "parser error : Invalid start tag name\n";
        exit(1);
    }
    auto nameEnd = std::find_if_not(cursor, cursorEnd, [] (char c) { return tagNameMask[c]; });
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : Unterminated start tag '" << std::string_view(std::addressof(*cursor), std::distance(cursor, nameEnd)) << "'\n";
        exit(1);
    }
    size_t colonPosition = 0;
    if (*nameEnd == ':') {
        colonPosition = std::distance(cursor, nameEnd);
        nameEnd = std::find_if_not(std::next(nameEnd), cursorEnd, [] (char c) { return tagNameMask[c]; });
    }
    const std::string_view prefix(std::addressof(*cursor), colonPosition);
    const std::string_view qName(std::addressof(*cursor), std::distance(cursor, nameEnd));
    if (qName.empty()) {
        std::cerr << "parser error: StartTag: invalid element name\n";
        exit(1);
    }
    if (colonPosition)
        ++colonPosition;
    const std::string_view localName(std::addressof(*cursor) + colonPosition, std::distance(cursor, nameEnd) - colonPosition);
    TRACE("START TAG", "prefix", prefix, "qName", qName, "localName", localName);
    if (localName == "expr"sv) {
        ++exprCount;
    } else if (localName == "decl"sv) {
        ++declCount;
    } else if (localName == "comment"sv) {
        ++commentCount;
    } else if (localName == "function"sv) {
        ++functionCount;
    } else if (localName == "unit"sv) {
        ++unitCount;
        if (depth == 1)
            isArchive = true;
    } else if (localName == "class"sv) {
        ++classCount;
    } else if (localName == "return"sv) {
        ++returnCount;
    } else if (localName == "literal"sv) {
        ++literalCount;
    }
    cursor = nameEnd;
    if (*cursor != '>')
        cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (*cursor == '>') {
        std::advance(cursor, 1);
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", prefix, "qName", qName, "localName", localName);
    } else {
        inTagQName = qName;
        inTagPrefix = std::string_view(inTagQName.data(), prefix.size());
        inTagLocalName = std::string_view(inTagQName.data() + prefix.size());
        inTag = true;
    }
}

// parse character entity references
void parseCharEntityRefs(std::string::const_iterator& cursor, int& textsize)
{
    std::string_view characters;
    if (cursor[1] == 'l' && cursor[2] == 't' && cursor[3] == ';') {
        characters = "<";
        std::advance(cursor, 4);
    } else if (cursor[1] == 'g' && cursor[2] == 't' && cursor[3] == ';') {
        characters = ">";
        std::advance(cursor, 4);
    } else if (cursor[1] == 'a' && cursor[2] == 'm' && cursor[3] == 'p' && cursor[4] == ';') {
        characters = "&";
        std::advance(cursor, 5);
    } else {
        characters = "&";
        std::advance(cursor, 1);
    }
    TRACE("ENTITYREF", "characters", characters);
    ++textsize;
}

// parse non-character entity references
void parseNonCER(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, int& loc, int& textsize)
{
    const auto tagEnd = std::find_if(cursor, cursorEnd, [] (char c) { return c == '<' || c == '&'; });
    const std::string_view characters(std::addressof(*cursor), std::distance(cursor, tagEnd));
    TRACE("CHARACTERS", "characters", characters);
    loc += static_cast<int>(std::count(characters.cbegin(), characters.cend(), '\n'));
    textsize += static_cast<int>(characters.size());
    std::advance(cursor, characters.size());
}

// predicate function checks the length of our buffer for refill
bool isShort(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd)
{
    return (std::distance(cursor, cursorEnd) < 5);
}

// refill buffer and adjust iterator
void refillAndAdjust(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer, long& totalBytes)
{
    auto bytesRead = refillBuffer(cursor, cursorEnd, buffer);
    if (bytesRead < 0) {
        std::cerr << "parser error : File input error\n";
        exit(1);
    }
    totalBytes += bytesRead;
}

// test for end of code
bool isEndOfCode(bool& inXMLComment, bool& inCDATA, std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd)
{
    return (!inXMLComment && !inCDATA && cursor == cursorEnd);
}

// parse characters before or after XML
void parseBeforeOrAfter(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd)
{
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
}

// check for Char Entity Refs
bool isCharEntityRef(std::string::const_iterator& cursor)
{
    return (*cursor == '&');
}

// check if before or after XML
bool isBeforeOrAfter(int depth)
{
    return (depth == 0);
}
