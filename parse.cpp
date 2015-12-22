#include "parser.h"

#include <vector>
#include <cmath>

//#define VERBOSE		  // if defined, parse(string) will print out every string it 
			  // receives and the result it computes for that string

using std::vector;

// exceptions/*{{{*/
class empty: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: Empty parentheses";
	}
} empty;

class mismatch: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: Mismatched parentheses";
	}
} mismatch;

class syntax: public exception
{

	virtual const char * what() const throw()
	{
		return "Error: syntax ";
	}
} syntax;

class divByZero: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: Divide by zero";
	}
} divByZero;

class undefined: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: Undefined";
	}
} undefined;

/*}}}*/

using std::fabs;
bool isZero( long double val )/*{{{*/
{
	if( fabs(val) < 0.00000000000000001 ){
		return true;
	}
	return false;
}/*}}}*/

bool isDigit(const char & sym)/*{{{*/
{
	if( isdigit(sym) || 
	    'b' == sym || 'B' == sym ||
	    ('a' <= sym && sym <= 'f') ||
	    ('A' <= sym && sym <= 'A') )
	{
		return true;
	}
	return false;
}/*}}}*/

long double parse(string expr)/*{{{*/
{
	static int count = 0;/*{{{*/
	count++;
	int len = expr.length();
	#ifdef VERBOSE
	cout << count << ": \"" << expr << "\"" << endl;
	#endif

	int start = 0;
	int end = 0;
	int paren_depth = 0;

	bool hasAdd = false;
	bool hasMul = false;
	bool hasPow = false;

	long double result = 0;	// additive identity

	bool prevWasSub = false;

	int numMinus = 0;/*}}}*/

	// parse terms separated by '+' or '-'/*{{{*/
	for( int i = 0; i < len; i++ ){
		char sym = expr[i];

		// track paren depth
		if( '(' == sym ){
			++paren_depth;
		}
		if( ')' == sym ){
			--paren_depth;
		}
		if( paren_depth < 0 ){
			#ifdef VERBOSE
			cout << "from add, depth: " << paren_depth << endl;
			#endif
			throw mismatch;
		}

		if( 0 == paren_depth ){		// make sure you are outside of parens
			if( '+' == sym ){
				if( 0 != i && len - 1 != i 	&& 	// don't walk off the edge	  AND
				    ( isDigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.' OR
				      ')' == expr[i-1]	    )   &&	//			')' OR
				    ( isDigit(expr[i+1]) || 		// is followed by [ a digit OR
				      '-' == expr[i+1]   ||		//			'-' OR
				      '.' == expr[i+1] 	 ||      	// 			'.' OR
				      '(' == expr[i+1]      )	  ){	//			'('	]

					// add previous term to result
					end = i;
					long double value = parse(expr.substr(start, end - start));
					if(prevWasSub){
						value = -value;
					}
					result += value;
					start = i+1; 	// start next term right after the plus
					hasAdd = true;
					prevWasSub = false;
				} else{
					#ifdef VERBOSE
					cout << "from plus: " << i << endl;
					#endif
					throw syntax;
				}
			} else
			if( '-' == sym 		&&	// if minus AND
			    (0 == i 		 ||	// [ is first OR
			      ('+' != expr[i-1]   &&	//   [ ignore '+-' AND
			       '*' != expr[i-1]   &&	//     ignore '*-' AND
			       '/' != expr[i-1]   &&    //     ignore '/-' AND
			       '^' != expr[i-1]	  &&    //     ignore '^-'    ] ]
			       'e' != expr[i-1]	 ))){   //     ignore '^-'    ] ]
				if((0 == i || // is first
				    // or is preceded by
				    ( isDigit(expr[i-1]) ||	  //    is preceded by [ a digit OR
				      '-' == expr[i-1]   ||	  //     			  '-'
				      '.' == expr[i-1]   ||	  //   			  '.'
				      ')' == expr[i-1]     )) &&  //   			  ')'	] AND
				// and is followed by
				   ( isDigit(expr[i+1]) ||		// is followed by [ a digit OR
				     '-' == expr[i+1]	||	   	//			'-' OR
				     '.' == expr[i+1]	||	   	//			'.' OR
				     '(' == expr[i+1]	    )){    	//			'('	]

					numMinus++; // now includes the current '-'
					if(1 == numMinus && 0 != i){ // this is the first '-' but not the very first sym
						end = i;
						long double value = parse(expr.substr(start, end - start));
						if(prevWasSub){
							value = -value;
						}
						result += value;
					}
					start = i+1;
					hasAdd = true;
				} else{
					#ifdef VERBOSE
					cout << "from sub: " << i << endl;
					#endif
					throw syntax;
				}
			} else
			if( numMinus > 0 ){
				prevWasSub = numMinus % 2;
				numMinus = 0;
			}
		}
	}
	// check for paren mismatch
	if( paren_depth != 0 ){
		#ifdef VERBOSE
		cout << "from below add/sub, depth: " << paren_depth << endl;
		#endif
		throw mismatch;
	}

	// add the final term
	if( hasAdd ){
		long double value = parse(expr.substr(start, len - start));
		if(prevWasSub){
			value = -value;
		}
		result += value; // add the last term
		#ifdef VERBOSE
		cout << count << ": " << result << endl;
		#endif
		return result;
	}/*}}}*/

	// parse terms separated by '*' or '/'/*{{{*/
	bool prevWasDiv = false;
	result = 1;	// multiplicative identity
	start = 0;
	for( int i = 0; i < len; i++ ){
		char sym = expr[i];

		// track paren depth
		if( i > 0 && '(' == expr[i-1] ){
			++paren_depth;
		}
		if( ')' == sym ){
			--paren_depth;
		}
		if( paren_depth < 0 ){
			#ifdef VERBOSE
			cout << "from mul, depth: " << paren_depth << endl;
			#endif
			throw mismatch;
		}

		if( 0 == paren_depth ){		// make sure you are outside of or on a parenthesis
			if( '*' == sym ){
				if( 0 != i && len - 1 != i 	&& 	// don't walk off the edge	  AND
				    ( isDigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.'
				      ')' == expr[i-1]      )	&& 	// 			')' 	] AND
				    ( isDigit(expr[i+1]) || 		// is followed by [ a digit OR
				      '-' == expr[i+1]   ||		//			'-' OR
				      '.' == expr[i+1] 	 ||      	// 			'.' OR
				      '(' == expr[i+1]      )	  ){	//			'('	]

					// mul previous term to result
					end = i;
					long double value = parse(expr.substr(start, end - start));
					if( prevWasDiv && std::numeric_limits<long double>::infinity() == 1./value ){
						#ifdef VERBOSE
						cout << "at " << i << endl;
						#endif
						throw divByZero;
					} else if( prevWasDiv ){
						result /= value;
					} else{
						result *= value;
					}
					start = i+1; 	// start next term right after the '*'
					hasMul = true;
					prevWasDiv = false;
				} else{
					#ifdef VERBOSE
					cout << "from mul: " << i << endl;
					#endif
					throw syntax;
				}
			} else
			if( '(' == sym 		&&
			    0 != i		&&	// ignore leading '('
			    '-' != expr[i-1]    &&	// ignore '-('
			    '/' != expr[i-1]	&&	// ignore '/('
			    '*' != expr[i-1]	&&  	// ignore '*('
			    '^' != expr[i-1]	  ){	// ignore '^('
				if( 0 != i && len - 1 != i 	&& 	// don't walk off the edge	  AND
				    ( isDigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.'
				      ')' == expr[i-1]      )	&& 	// 			')' 	] AND
				    ( isDigit(expr[i+1]) || 		// is followed by [ a digit OR
				      '-' == expr[i+1]   ||		//			'-' OR
				      '.' == expr[i+1] 	 ||      	// 			'.' OR
				      '(' == expr[i+1]      )	  ){	//			'('	]

					// mul previous term to result
					end = i;
					long double value = parse(expr.substr(start, end - start));
					if( prevWasDiv && std::numeric_limits<long double>::infinity() == 1./value ){
						#ifdef VERBOSE
						cout << "at " << i << endl;
						#endif
						throw divByZero;
					} else if( prevWasDiv ){
						result /= value;
					} else{
						result *= value;
					}
					start = i; 	// start next term on the '(' to avoid mismatch
					hasMul = true;
					prevWasDiv = false;
				} else{
					#ifdef VERBOSE
					cout << "from parmul: " << i << endl;
					#endif
					throw syntax;
				}

			} else
			if( '/' == sym 	){
				if( 0 != i && len - 1 != i	&&	// don't walk off the end	  AND
				    ( isDigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.'
				      ')' == expr[i-1]      )	&&	//			')'	] AND
				    ( isDigit(expr[i+1]) ||		// is followed by [ a digit OR
				      '-' == expr[i+1]	 ||		//			'-' OR
				      '.' == expr[i+1]	 ||		//			'.' OR
				      '(' == expr[i+1]	    )	   ){	//			'('	]

					// div previous term to result
					end = i;
					long double value = parse(expr.substr(start, end - start));
					if( prevWasDiv && std::numeric_limits<long double>::infinity() == 1./value ){
						#ifdef VERBOSE
						cout << "at " << i << endl;
						#endif
						throw divByZero;
					} else if( prevWasDiv ){
						result /= value;
					} else{
						result *= value;
					}
					start = i+1;
					hasMul = true;
					prevWasDiv = true;
				} else{
					#ifdef VERBOSE
					cout << "from div: " << i << endl;
					#endif
					throw syntax;
				}
			} else if( 0 != i && 
				   ')' == expr[i-1] && 
				   isDigit(sym)
				   ){ // catches (5)6 form of multiplication
					end = i;
					long double value = parse(expr.substr(start, end - start));
					if( prevWasDiv && std::numeric_limits<long double>::infinity() == 1./value ){
						#ifdef VERBOSE
						cout << "at " << i << endl;
						#endif
						throw divByZero;
					} else if( prevWasDiv ){
						result /= value;
					} else{
						result *= value;
					}
					start = i; 	// start next term on the current sym
					hasMul = true;
					prevWasDiv = false;
			}
		}
	}
	if( hasMul ){
		long double value = parse(expr.substr(start, len - start));
		if( prevWasDiv && std::numeric_limits<long double>::infinity() == 1./value ){
			#ifdef VERBOSE
			cout << "after mul " << endl;
			#endif
			throw divByZero;
		} else if( prevWasDiv ){
			result /= value;
		} else{
			result *= value;
		}
		#ifdef VERBOSE
		cout << count << ": " << result << endl;
		#endif
		return result;
	}/*}}}*/

	// parse terms separated by '^'/*{{{*/
	vector<long double> powers;
	if( !hasAdd && !hasMul ){
		for(int i = 0; i < len; i++){
			char sym = expr[i];

			// track paren depth
			if( '(' == sym ){
				++paren_depth;
			}
			if( ')' == sym ){
				--paren_depth;
			}
			if( paren_depth < 0 ){
				#ifdef VERBOSE
				cout << "from pow, depth: " << paren_depth << endl;
				#endif
				throw mismatch;
			}

			if( 0 == paren_depth ){		// make sure you are outside of parens
				if( '^' == sym ){
					if( 0 != i && len - 1 != i 	&& 	// don't walk off the edge	  AND
					    ( isDigit(expr[i-1]) ||		// is preceded by [ a digit OR
					      '.' == expr[i-1]	 ||		//			'.'
					      ')' == expr[i-1]      )	&& 	// 			')' 	] AND
					    ( isDigit(expr[i+1]) || 		// is followed by [ a digit OR
					      '-' == expr[i+1]   ||		//			'-' OR
					      '.' == expr[i+1] 	 ||      	// 			'.' OR
					      '(' == expr[i+1]      )	  ){	//			'('	]
						// parse and save previous term to result
						end = i;
						powers.push_back(parse(expr.substr(start, end - start)));
						start = i+1; 	// start next term right after the '^'
						hasPow = true;
					} else{
						#ifdef VERBOSE
						cout << "from pow: " << i << endl;
						#endif
						throw syntax;
					}
				}
			}
		}
	}
	if( hasPow ){
		powers.push_back(parse(expr.substr(start, len - start))); // mul the last term
		long double expo = powers.back();
		for(int i = powers.size() - 2; i >= 0; i--){
			long double base = powers[i];
			if( isZero(base) && expo < 0 ){
				#ifdef VERBOSE
				cout << "from pow: " << i << endl;
				#endif
				throw divByZero;
			} else if( !isZero(expo) ){
				expo = pow(base, expo);
			} else{
				expo = 1;
			}
			if(!(expo == expo)){
				throw undefined;
			}
		}
		#ifdef VERBOSE
		cout << count << ": " << expo << endl;
		#endif
		return expo;
	}/*}}}*/

	// remove outer parens/*{{{*/
	if( '(' == expr.front() && ')' == expr.back() ){
		result = parse(expr.substr(1,len-2));
		return result;
	} 
	// check for empty parens
	if( len == 0 ){
		throw empty;
	}/*}}}*/

	// base case, parse numbers/*{{{*/
	bool isHex = false;
	bool isBin = false;
	if( 'B' == expr[0] || 'b' == expr[0] ){
		isBin = true;
	} else
	if( '0' == expr[0] && 'x' == expr[1] ){
		isHex = true;
	}

	if(isHex){
		result = parse_hex(expr.substr(2, len-2));
	} else if(isBin){
		result = parse_bin(expr.substr(1,len-1));
	} else{
		result = parse_num(expr);
	}

	#ifdef VERBOSE
	cout << count << ": " << result << endl;
	#endif
	return result;/*}}}*/
}
/*}}}*/

/* each char is: {{{
 * 		- a digit 0123456789, possibly belonging to a longer number 
 *		- a binary op, + - * / ^
 *		- () 

 * an expression can only start with:  d ( -

   last | allowed to follow
  ======|====================================================================
     d  |   d + - * / ^ ( )
     )	|   d + - * / ^ ( )
     +  |   d ( -
     *  |   d ( -
     /  |   d ( -
     ^  |   d ( -
     (	|   d ( -
     -  |   d (

 */

/* syntax checking notes:
 * 	any binary op must:
 * 		be preceded by a digit '.' or ')'
 * 		followed by a digit '-' '.' or '('
 *	paren depth must never be negative and must return to zero
 *	remove all spaces
 *
 * Recursive parse:
 * 	1. if there are add/sub outside parentheses,
 * 	   split all terms separated by add outside parentheses,  (validate op)
 * 	   call parse on each term, 	(check for mismatch)
 * 	   return sum of results
 *
 * 	2. if no add/sub, 
 * 	   split all terms separated by mul/div, 	(validate op)
 * 	   call parse on each term, 
 * 	   apply ^-1 on each term following division
 * 	   return product of results
 *
 * 	3. if no add/sub, mul/div exist,
 * 	   split all terms separated by ^,	(validate op)
 * 	   call parse on each term,
 * 	   return power of each term evaluated with the two rightmost powers first
 *
 * 	4. if no add/sub, mul/div or powers exist
 * 	   and if there are outer parens
 * 	   remove them
 *
 * 	5. if no add/sub, mul/div or powers exist
 * 	   return atof on input string
 *//*}}}*/
