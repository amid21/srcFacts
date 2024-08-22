/*
    identity.cpp

    An identity transformation of XML. The input is XML and the
    output is the equivalent XML.

    Limitation:
    * CDATA is not complete
*/

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include "XMLParser.hpp"

using namespace std::literals::string_view_literals;

int main() {

    std::fstream demoCopy;
    
    XMLParser parser(
    [&](int depth, std::string_view qName, std::string_view prefix, std::string_view localName) {
        
        demoCopy << "<" << qName << ">";
    },
    [&](int depth, std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value) {
        
        demoCopy.seekp(-1, std::ios_base::cur);
        demoCopy << " " << qName << "=\"" << value << "\">";
    },
    [&](int depth, std::string_view characters) {
        
        for (auto i = characters.begin(); i != characters.end(); i++) {
            if (*i == '<') {
                demoCopy << "&lt;";
            } else if (*i == '>') {
                demoCopy << "&gt;";
            } else if (*i == '&') {
                demoCopy << "&amp;";
            } else {
                demoCopy << *i;
            }
        }
    },
    [&](int depth, std::string_view characters) {
        
        for (auto i = characters.begin(); i != characters.end(); i++) {
            if (*i == '<') {
                demoCopy << "&lt;";
            } else if (*i == '>') {
                demoCopy << "&gt;";
            } else if (*i == '&') {
                demoCopy << "&amp;";
            } else {
                demoCopy << *i;
            }
        }
    },
    [&](int depth, std::string_view characters) {

        for (auto i = characters.begin(); i != characters.end(); i++) {
            if (*i == '<') {
                demoCopy << "&lt;";
            } else if (*i == '>') {
                demoCopy << "&gt;";
            } else if (*i == '&') {
                demoCopy << "&amp;";
            } else {
                demoCopy << *i;
            }
        }
    },
    [&](int depth, std::string_view prefix, std::string_view uri) {
        
        demoCopy.seekp(-1, std::ios_base::cur);
        demoCopy << " xmlns"; 
        if (prefix.empty())
            demoCopy << "=\"" << uri << "\">";
        else
            demoCopy << ":" << prefix << "=\"" << uri << "\">";
    },
    [&](int depth, std::string_view comment) {
        demoCopy << comment;
    },
    [&](int depth, std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone) {
        
        demoCopy << "<?xml version=\"" << version << "\" encoding=\"" << encoding.value() << "\" standalone=\"" << standalone.value() << "\"?>\n";
    },
    [&](int depth, std::string_view target, std::string_view data) {
        
        demoCopy.seekp(-1, std::ios_base::cur);
        demoCopy << " " << target << "=\"" << data << "\">\n";
    },
    [&](int depth, std::string_view prefix, std::string_view qName, std::string_view localName) {
        demoCopy << "</" << qName << ">";
    },
    [&](int depth) {

        demoCopy.open("democopy.xml");
        return;
    },
    [&](int depth) {

        demoCopy << "\n";
        demoCopy.close();
        return;
    });

    parser.parse();

    return 0;
}
