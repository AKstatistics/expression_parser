#include "common_parse_includes.h"

class hex_symbol: public exception
{

	virtual const char * what() const throw()
	{
		return "Error: bad hex symbol ";
	}
} hex_symbol;
unsigned int parse_hex( string expr )/*{{{*/
{
	unsigned long long num = 0;
	int len = expr.length();
	for(int i = 0; i < len; i++){
		const char sym = expr[i];
		unsigned int digit;
		if( isdigit(sym) ){
			digit = sym - '0';
		} else if( 'a' <= sym && sym <= 'f' ){
			digit = sym - 'a' + 10;
		} else if( 'A' <= sym && sym <= 'F' ){
			digit = sym - 'A' + 10;
		} else{
			throw hex_symbol;
		}

		num = num | (digit << ( (len - i - 1) * 4 ));
	}

	return num;
}/*}}}*/
