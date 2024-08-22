/*
    srcFacts.cpp

    Produces a report with various measures of source code.
    Supports C++, C, Java, and C#.

    Input is an XML file in the srcML format.

    Output is a markdown table with the measures.

    Output performance statistics to stderr.

    Code includes an embedded XML parser:
    * No checking for well-formedness
    * No DTD declarations
*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <string_view>
#include <algorithm>

#include "XMLParser.hpp"

using namespace std::literals::string_view_literals;

int main() {
    const auto start = std::chrono::steady_clock::now();
    std::string url;
    int textsize = 0;
    int loc = 0;
    int exprCount = 0;
    int functionCount = 0;
    int classCount = 0;
    int unitCount = 0;
    int declCount = 0;
    int commentCount = 0;
    int lineCommentCount = 0;
    int returnCount = 0;
    int literalCount = 0;
    bool isArchive = false;

    XMLParser parser([&](int depth, std::string_view qName, std::string_view prefix, std::string_view localName) {

        // update counts for srcFacts report
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
                isArchive == true;
        } else if (localName == "class"sv) {
            ++classCount;
        } else if (localName == "return"sv) {
            ++returnCount;
        } else if (localName == "literal"sv) {
            ++literalCount;
        }
    },
    [&url, &lineCommentCount](int depth, std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value) {

        // check url and update line comment counter
        if (localName == "url"sv) {
        url = value;
        }  
        if (value == "line"sv) {
        ++lineCommentCount;
        }
    },
    [&textsize, &loc](int depth, std::string_view characters) {

        // update textsize and loc
        textsize += static_cast<int>(characters.size());
        loc += static_cast<int>(std::count(characters.begin(), characters.end(), '\n'));
    },
    [&textsize, &loc](int depth, std::string_view characters) {

        // update textsize and loc
        textsize += static_cast<int>(characters.size());
        loc += static_cast<int>(std::count(characters.begin(), characters.end(), '\n'));
    },
    [&textsize](int depth, std::string_view characters) {

        // increment textsize
        ++textsize;
    },
    [](int depth, std::string_view prefix, std::string_view uri) {
        
        // Nothing done with these in srcFacts
        return;
    },
    [](int depth, std::string_view comment) {

        // Nothing done with this in srcFacts
        return;
    },
    [](int depth, std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone) {

        // Nothing done with these in srcFacts
        return;
    },
    [](int depth, std::string_view target, std::string_view data) {

        // Nothing done with these in srcFacts
        return;
    },
    [](int depth, std::string_view prefix, std::string_view qName, std::string_view localName) {

        // Nothing done with these in srcFacts
        return;
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

    const auto finish = std::chrono::steady_clock::now();
    const auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double> >(finish - start).count();
    const double mlocPerSec = loc / elapsed_seconds / 1000000;
    auto files = unitCount;
    if (isArchive)
        --files;

    // output report
    std::cout.imbue(std::locale{""});
    int valueWidth = std::max(5, static_cast<int>(log10(parser.getTotalBytes()) * 1.3 + 1));
    std::cout << "# srcFacts: " << url << '\n';
    std::cout << "| Measure      | " << std::setw(valueWidth + 3) << "Value |\n";
    std::cout << "|:-------------|-" << std::setw(valueWidth + 3) << std::setfill('-') << ":|\n" << std::setfill(' ');
    std::cout << "| srcML bytes  | " << std::setw(valueWidth) << parser.getTotalBytes()   << " |\n";
    std::cout << "| Characters   | " << std::setw(valueWidth) << textsize          << " |\n";
    std::cout << "| Files        | " << std::setw(valueWidth) << files             << " |\n";
    std::cout << "| LOC          | " << std::setw(valueWidth) << loc               << " |\n";
    std::cout << "| Classes      | " << std::setw(valueWidth) << classCount        << " |\n";
    std::cout << "| Functions    | " << std::setw(valueWidth) << functionCount     << " |\n";
    std::cout << "| Declarations | " << std::setw(valueWidth) << declCount         << " |\n";
    std::cout << "| Expressions  | " << std::setw(valueWidth) << exprCount         << " |\n";
    std::cout << "| Comments     | " << std::setw(valueWidth) << commentCount      << " |\n";
    std::cout << "| Line Comments| " << std::setw(valueWidth) << lineCommentCount  << " |\n";
    std::cout << "| Returns      | " << std::setw(valueWidth) << returnCount       << " |\n";
    std::cout << "| Literals     | " << std::setw(valueWidth) << literalCount      << " |\n";
    std::clog << '\n';
    std::clog << std::setprecision(3) << elapsed_seconds << " sec\n";
    std::clog << std::setprecision(3) << mlocPerSec << " MLOC/sec\n";
    std::cout << "\n";
    return 0;
}
