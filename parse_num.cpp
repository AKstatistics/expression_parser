#include "common_parse_includes.h"

class num_symbol: public exception
{

	virtual const char * what() const throw()
	{
		return "Error: bad num symbol ";
	}
} num_symbol;

class sci_syntax: public exception
{

	virtual const char * what() const throw()
	{
		return "Error: bad scientific notation ";
	}
} sci_syntax;

class num_empty: public exception
{

	virtual const char * what() const throw()
	{
		return "Error: empty number ";
	}
} num_empty;

long double parse_num( string expr )/*{{{*/
{
	int len = expr.length();
	long double magnitude = 1;

	int sciNotation = expr.find("e");
	
	if(sciNotation > -1){
		string afterE = expr.substr(sciNotation+1);
		if( sciNotation > 0	   &&
		    afterE.length() > 0	   &&
		    afterE.find("e") == -1 &&
		    afterE.find(".") == -1 ){
			int pow = parse_num( afterE );
			if( pow > 0 ){
				for( int i = 0; i < pow; i++ ){
					magnitude *= 10;
				}
			} else{
				pow *= -1;
				for( int i = 0; i < pow; i++ ){
					magnitude *= .10;
				}
			}
			len = sciNotation;
		} else{
			throw sci_syntax;
		}
	}

	// check for negative
	bool isNeg = false;
	if('-' == expr[0]){
		if( len > 1 ){
			isNeg = true;
		} else{
			throw num_empty;
		}
	}

	// set magnitude
	if('.' == expr[isNeg]){
		if( len > 1){
			magnitude *= .1;
		} else{
			throw num_empty;
		}
	} else{
		for(int i = isNeg; i < len; i++){
			if(i != len - 1 && expr[i+1] != '.'){
				magnitude *= 10;
			} else{
				break;
			}
		}
	}

	// parse number
	bool hasDec = false;
	long double num = 0;
	for(int i = isNeg; i < len; i++){
		const char sym = expr[i];
		if(isdigit(sym)){
			num += (sym - '0') * magnitude;
			magnitude *= .1;
		} else if('.' == sym && !hasDec){
			hasDec = true;
		} else{
			throw num_symbol;
		}
	}

	// apply negative
	if(isNeg){
		num *= -1;
	}

	return num;
}/*}}}*/
