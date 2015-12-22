/*	Math Expression Parser
 *	FINAL SUBMISSION
 *	Adam Levy
 *	Nov 4, 2015
 *	Designed for Orion Lawlor's CS 301 Assembly class project
 *
 *	This program features a recursive function, 
 *		long double parse( string );
 *	which takes a mathematical expression as a string and 
 *	computes the result according to standard order of operations.
 *
 *	Parse accepts + - * / ^ ( ), decimal numbers, hex, binary, juxtaposed () instead of *
 *
 *	See #define VERBOSE to silence the parse function
 */


#include <iostream>/*{{{*/
#include <iomanip>
#include <string>
#include <stdlib.h>

#include "parser.h"

//#define VERBOSE		  // if defined, parse(string) will print out every string it 
			  // receives and the result it computes for that string

using std::string;
using std::cin;
using std::cout;
using std::endl;

// a preprocessor which removes whitespace and make all char to lower and interprets 'ans'
extern void clean_input( string & , long double ans );

void clearAll();

int main()/*{{{*/
{
	clearAll();
	cout << "Welcome to Adam Levy's calculator. " << endl << endl;
	cout << "You can enter math expressions using: +, -, *, /, ^, () " << endl;
	cout << "The expression will be evaluated by the standard PEMDAS order of operations. " << endl;
	cout << "You can enter numbers in decimal, hex or binary format. " << endl;
	cout << "Prepend a hex number with 0x, e.g. 0xFF, 0xfa, 0xFa123aff " << endl;
	cout << "Prepend a binary number with B or b, e.g. b101, B001 " << endl;
	cout << "All spaces are removed, so B 1111 1111 1111 is allowed. " << endl;
	cout << "Arbitrarily long strings of - will be properly evaluated, e.g. --4 = 4, ---4 = -4 " << endl;
	cout << endl << "Enter QUIT, quit, Q or q to exit the program. " << endl << endl;
	
    double prev_answer = std::numeric_limits<double>::quiet_NaN();

	for(;;){
		string input;
		bool error = false;
		cout << "CALC:   ";
		getline(cin, input);
        try{
		    clean_input(input,prev_answer);
        }
        catch ( exception & e ){
            cout << e.what() << endl << endl;
            error = true;
        }

		if("quit" == input || "q" == input){
			break;
		}
		if("clear" == input || "c" == input){
			clearAll();
			continue;
		}

        if( !error ){
		    long double result = 0;
		    try{
		    	result = parse(input);
		    }
		    catch ( exception & e ){
		    	cout << e.what() << endl << endl;
		    	error = true;
		    }
		    if( !error ){
		    	cout << " = " << result << endl << endl;
                prev_answer = result;
		    }
        }
	}

	cout << "GOODBYE" << endl;
	return 0;
}/*}}}*/

void clearAll()/*{{{*/
{
	cout << string( 100, '\n' );
}/*}}}*/

