/*
    refillBuffer.hpp

    Include file for refillBuffer function
*/

#ifndef INCLUDED_REFILLBUFFER_HPP
#define INCLUDED_REFILLBUFFER_HPP

#include <string>

int refillBuffer(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer);

#endif
