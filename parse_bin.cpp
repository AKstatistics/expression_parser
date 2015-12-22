#include "common_parse_includes.h"

class bin_symbol: public exception
{

	virtual const char * what() const throw()
	{
		return "Error: bad bin symbol ";
	}
} bin_symbol;


unsigned int parse_bin( string expr )/*{{{*/
{
	unsigned long long num = 0;
	int len = expr.length();
	for(int i = 0; i < len; i++){
		const char sym = expr[i];
		unsigned int digit;
		if('0' == sym){
			digit = 0;
		} else if('1' == sym){
			digit = 1;
		} else{
			throw bin_symbol;
		}

		num = num | (digit << (len - i - 1));
	}

	return num;
}/*}}}*/
