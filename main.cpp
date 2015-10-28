/*	Math Expression Parser
 *	Adam Levy
 *	Oct 15, 2015
 *	Designed for Orion Lawlor's CS 301 Assembly class project
 *
 *	This program features a recursive function, 
 *	long double recursive_parse(string),
 *	which takes a mathematical expression as a string and 
 *	computes the result according to standard order of operations.
 *
 *	Parse accepts + - * / ^ ( ), decimal numbers, juxtaposed ()
 *
 *	The main runs through test cases, prints their output, and their result
 *
 *	See #define VERBOSE to silence the parse function
 */


#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <stdlib.h>

//#define VERBOSE		  // if defined, parse(string) will print out every string it 
			  // receives and the result it computes for that string

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::exception;
using std::fabs;

long double parse( string );
//long double recursive_parse( string );		// recursive parser
bool isValid(const char);	// checks that a char within input string is allowed
bool isZero(long double);	// used for checking zero of floating points


// test generation
enum OPP{ SUM, SUB, MUL, DIV, POW };
string arbitrary_expr( int numOps, OPP o );	// generates an expression 

int main()
{
	// test cases
	vector<string> tests; 
	tests.push_back("("+arbitrary_expr(10,SUM)+")*"+ arbitrary_expr(10,MUL));
	//tests.push_back("-(4+3)(.5)(3+2)");
	//tests.push_back("((43))");
	//tests.push_back("((43)-123.2)/(145*1-1)");
	//tests.push_back("(43)^(1+2)");
	//tests.push_back("2^.0000000000000000000000000000000001");
	//tests.push_back("2^0");
	//tests.push_back("0^0");
	//tests.push_back("0^-10");
	//tests.push_back(".1^2^3^4^-1");
	//tests.push_back("--(1234)");
	//tests.push_back("45--(1234)");
	//tests.push_back("45-(1234)");
	//tests.push_back("81./34*1+2*(3-5.234543)^2^(8.1234-2.19238)");
	//tests.push_back("81./34*1+2*2^(8.1234-2.19238)");
	//tests.push_back("(5(.25^3)*(-8.34)^7)/3");
	//tests.push_back("-(-5(.25^3)(4-8.23^.5)(-8.34)^7)/-3");
	//tests.push_back("-(-5(.25^3)(4+8.23^.5)(-8.34)^7)/-3");
	//tests.push_back("-(-(-(4*3+8(12(2(3))))))");
	//tests.push_back("--(-(-(4*3+8(12(2(3))))))");
	//tests.push_back("-----------------------45--------234----1");
	//tests.push_back("(3-5.234543)^2^(8.1234-2.19238)");
	//tests.push_back("2^(8.1234-2.19238)");
	//tests.push_back("(3-5.234543)^61.012");
	//tests.push_back("(6-5.234543)^61.012");
	//tests.push_back("(5-5.234543)^61.012");
	//tests.push_back("(-5.234543)^61.012");
	//tests.push_back("-2^61.012");
	//tests.push_back("(-2)^-(-2)");

	long double result;

	for( int i = 0; i < tests.size(); i++ ){
		bool error = false;
		string expression = tests[i];
		try{
		result = parse(expression);
		}
		catch ( exception & e ){
			cout << e.what() << ": " << expression << endl << endl;
			error = true;
		}
		if( !error ){
			cout << expression << " = " << result << endl << endl;
		}
	}

	return 0;
}

// exceptions
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
		return "Error: syntax";
	}
} syntax;

class divByZero: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: Divide by zero";
	}
} divByZero;

class badOp: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: Op not in enum OPP";
	}
} badOp;

/* each char is:
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
 */
long double parse(string expr){
	static int count = 0;
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

	int numMinus = 0;

	// parse terms separated by '+' or '-'
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
				    ( isdigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.' OR
				      ')' == expr[i-1]	    )   &&	//			')' OR
				    ( isdigit(expr[i+1]) || 		// is followed by [ a digit OR
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
			       '^' != expr[i-1]	 ))){   //     ignore '^-'    ] ]
				if((0 == i || // is first
				    // or is preceded by
				    ( isdigit(expr[i-1]) ||	  //    is preceded by [ a digit OR
				      '-' == expr[i-1]   ||	  //     			  '-'
				      '.' == expr[i-1]   ||	  //   			  '.'
				      ')' == expr[i-1]     )) &&  //   			  ')'	] AND
				// and is followed by
				   ( isdigit(expr[i+1]) ||		// is followed by [ a digit OR
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
	if( paren_depth != 0 ){
		#ifdef VERBOSE
		cout << "from below add/sub, depth: " << paren_depth << endl;
		#endif
		throw mismatch;
	}
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
	}

	// parse terms separated by '*' or '/'
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
				    ( isdigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.'
				      ')' == expr[i-1]      )	&& 	// 			')' 	] AND
				    ( isdigit(expr[i+1]) || 		// is followed by [ a digit OR
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
			}
			if( '(' == sym 		&&
			    0 != i		&&	// ignore leading '('
			    '-' != expr[i-1]    &&	// ignore '-('
			    '/' != expr[i-1]	&&	// ignore '/('
			    '*' != expr[i-1]	&&  	// ignore '*('
			    '^' != expr[i-1]	  ){	// ignore '^('
				if( 0 != i && len - 1 != i 	&& 	// don't walk off the edge	  AND
				    ( isdigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.'
				      ')' == expr[i-1]      )	&& 	// 			')' 	] AND
				    ( isdigit(expr[i+1]) || 		// is followed by [ a digit OR
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

			}
			if( '/' == sym 	){
				if( 0 != i && len - 1 != i	&&	// don't walk off the end	  AND
				    ( isdigit(expr[i-1]) ||		// is preceded by [ a digit OR
				      '.' == expr[i-1]	 ||		//			'.'
				      ')' == expr[i-1]      )	&&	//			')'	] AND
				    ( isdigit(expr[i+1]) ||		// is followed by [ a digit OR
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
	}

	// parse terms separated by '^'
	vector<long double> powers;
	if( !hasAdd && !hasMul ){

		for(int i = 0; i < len; i++){
			char sym = expr[i];

			// track paren depth
			if( '(' == expr[i-1] ){
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
					    ( isdigit(expr[i-1]) ||		// is preceded by [ a digit OR
					      '.' == expr[i-1]	 ||		//			'.'
					      ')' == expr[i-1]      )	&& 	// 			')' 	] AND
					    ( isdigit(expr[i+1]) || 		// is followed by [ a digit OR
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
		}
		#ifdef VERBOSE
		cout << count << ": " << expo << endl;
		#endif
		return expo;
	}

	// remove outer parens
	if( '(' == expr.front() && ')' == expr.back() ){
		result = parse(expr.substr(1,len-2));
		return result;
	}

	// check for empty parens
	if( len == 0 ){
		throw empty;
	}

	// base case, parse numbers
	// check for too many '.'
	int dots = 0;
	for( int i = 0; i < len; i++ ){
		const char sym = expr[i];
		if( '.' == expr[i] ){
			dots++;
		}
		if( !isValid(sym) ){
			cout << "from bad sym: " << sym << endl;
			throw syntax;
		}
	}
	if( dots > 1 ){
		cout << "from dots" << endl;
		throw syntax;
	}
	result = strtold(expr.c_str(), NULL);
	#ifdef VERBOSE
	cout << count << ": " << result << endl;
	#endif
	return result;
}

bool isValid( const char sym ){
	if( !isdigit(sym) &&
	    '.' != sym	  &&
	    '+' != sym	  &&
	    '-' != sym	  &&
	    '*' != sym	  &&
	    '/' != sym	  &&
	    '(' != sym	  &&
	    ')' != sym	  &&
	    '^' != sym	     ){
		return false;
	}
	return true;
}

bool isZero( long double val ){
	if( fabs(val) < 0.00000000000000001 ){
		return true;
	}
	return false;
}

string arbitrary_expr( int numOps, OPP o )
{
	string expr = "1";
	char op;
	switch(char(o)){
		case SUM:
			op = '+';
			break;
		case SUB:
			op = '-';
			break;
		case MUL:
			op = '*';
			break;
		case DIV:
			op = '/';
			break;
		case POW:
			op = '^';
			break;
		default:
			throw badOp;
	}

	for( int i = 2; i <= numOps; i++ ){
		expr += op;
		expr += std::to_string(i);
	}
			
	return expr;
}
