# Srcfacts

C++ (Spring 2022)

In Object Oriented Programming, a project we've worked on all semester worked
with an XML parser changing the design so that it had more applications, was easier
to extend and read. 
The latest version here includes extension point handlers, and multiple
programs that use the parser class.

### Files

ClassDiagram.txt - formatted for a website called yuml.me. It produces
		   a class diagram I wrote of the XMLParser.

CMakeLists.txt - Provided by my professor, builds this program in a linux
		 distribution.

demo.xml.zip - Provided by my professor, demo code to run the program with

identity.cpp - Written by me, registers handlers for XMLParser to make a copy
	       of the parsed code

refillBuffer.cpp - Extracted a function from the original srcFacts.cpp
		   That fills a buffer  with xml to parse.

refillBuffer.hpp - includes for refillBuffer() function

SequenceDiagram.svg - Sequence diagram for flow of control between srcFacts.cpp
		      and the XMLParser.cpp

srcFacts.cpp - The main program that we have been extracting from and redesigning.
	       It reports data from an XML file.

xml_parser.cpp - free functions extracted from srcFacts

xml_parser.hpp - includes for free functions

XMLParser.cpp - Classed version of the XMLParser

XMLParser.hpp - includes for XMLParser

xmlStats.cpp - program that uses my XMLParser to count different parts of XML 
	       it comes across.

srcFacts(original).txt - contains the original srcFacts program that we have
			 been redesigning.

### Below is a description of the srcFacts.cpp program written by my professor.

-----

# srcFacts

Calculates various counts on a source-code project, including files, functions,
comments, etc.

Input is a srcML form of the project source code. An example srcML file for libxml2
is included.

The srcReport main program includes code to directly parse XML interleaved with code
to produce the report.

Notes:
* The integrated XML parser handles start tags, end tags, empty elements, attributes,
characters, namespaces, (XML) comments, and CDATA.
* Program should be fast. Run on 3 GB srcML of the linux kernel takes under 20 seconds
on an SSD Macbook Pro Mid 2015 2.2 GHz Intel Core i7. Takes very little RAM.

-----