#ifndef PARSE_H
#define PARSE_H
// Include this and link to expression parser object files
//

#include "common_parse_includes.h"

// the recursive parse function that does the real work/*{{{*/
extern long double parse( string );

// a function for parsing hex, without the leading 0x
extern unsigned int parse_hex( string );

// a function for parsing binary, without the leading B or b
extern unsigned int parse_bin( string );

// a function for parsing decimal numbers to long doubles
extern long double parse_num( string );

#endif // PARSE_H
