/*
    xmlstats.cpp

    Markdown report with the number of each part of XML.
    E.g., the number of start tags, end tags, attributes,
    character sections, etc.
*/

#include <iostream>
#include <iomanip>
#include "XMLParser.hpp"

int main() {

    int XMLNSCount = 0;
    int attributeCount = 0;
    int XMLCommentCount = 0;
    int CDATACount = 0;
    int XMLDeclarationCount = 0;
    int PICount = 0;
    int endTagCount = 0;
    int startTagCount = 0;
    int beforeOrAfterCount = 0;
    int CERCount = 0;
    int nonCERCount = 0;

    XMLParser parser(
    [&startTagCount](int depth, std::string_view qName, std::string_view prefix, std::string_view localName) {
        ++startTagCount;
    },
    [&attributeCount](int depth, std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value) {
        ++attributeCount;
    },
    [&nonCERCount](int depth, std::string_view characters) {
        ++nonCERCount;
    },
    [&CDATACount](int depth, std::string_view characters) {
        ++CDATACount;
    },
    [&CERCount](int depth, std::string_view characters) {
        ++CERCount;
    },
    [&XMLNSCount](int depth, std::string_view prefix, std::string_view uri) {
        ++XMLNSCount;
    },
    [&XMLCommentCount](int depth, std::string_view comment) {
        ++XMLCommentCount;
    },
    [&XMLDeclarationCount](int depth, std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone) {
        ++XMLDeclarationCount;
    },
    [&PICount](int depth, std::string_view target, std::string_view data) {
        ++PICount;
    },
    [&endTagCount](int depth, std::string_view prefix, std::string_view qName, std::string_view localName) {
        ++endTagCount;
    },
    [](int depth) {

        // Nothing done here
        return;
    },
    [](int depth) {

        // Nothing done here
        return;
    });

    parser.parse();

    // output xml stats
    std::cout << "\n\n";
    int valueWidth = 6;
    std::cout << "# XMLStats:\n";
    std::cout << "| Measure        | " << std::setw(valueWidth + 3) << "Value |\n";
    std::cout << "|:---------------|-" << std::setw(valueWidth + 3) << std::setfill('-') << ":|\n" << std::setfill(' ');
    std::cout << "| XML Namespaces | " << std::setw(valueWidth) << XMLNSCount          << " |\n";
    std::cout << "| attributes     | " << std::setw(valueWidth) << attributeCount      << " |\n";
    std::cout << "| Comments       | " << std::setw(valueWidth) << XMLCommentCount     << " |\n";
    std::cout << "| CDATA          | " << std::setw(valueWidth) << CDATACount          << " |\n";
    std::cout << "| Declarations   | " << std::setw(valueWidth) << XMLDeclarationCount << " |\n";
    std::cout << "| PI's           | " << std::setw(valueWidth) << PICount             << " |\n";
    std::cout << "| End Tags       | " << std::setw(valueWidth) << endTagCount         << " |\n";
    std::cout << "| Start Tags     | " << std::setw(valueWidth) << startTagCount       << " |\n";
    std::cout << "| Before or After| " << std::setw(valueWidth) << beforeOrAfterCount  << " |\n";
    std::cout << "| CER's          | " << std::setw(valueWidth) << CERCount            << " |\n";
    std::cout << "| Non CER's      | " << std::setw(valueWidth) << nonCERCount         << " |\n";
    std::cout << "\n\n";
   
    return 0;
}
