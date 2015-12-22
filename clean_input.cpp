#include "common_parse_includes.h"

#include <vector>
using std::vector;

class noHistory: public exception
{
	virtual const char * what() const throw()
	{
		return "Error: No previous history. Cannot parse ANS";
	}
} noHistory;

using std::to_string;
void clean_input( string & input, long double ans )/*{{{*/
{
	int len = input.length();

	vector<int> toErase;
    vector<int> toAns;
    vector<int> toPi;

	for(int i = 0; i < len; i++){
		char sym = input[i];

        // make A-Z lowercase
		if( 'A' <= sym && sym <= 'Z' ){
			input[i] += 'a' - 'A';
		}

        // save all whitespace positions for removal
		if( '\n' == sym || '\r' == sym || '\t' == sym || ' ' == sym ){
			toErase.push_back(i);
        }
	}
    // erase whitespace
	for(int i = toErase.size() - 1; i >= 0; i--){
		input.erase(toErase[i],1);
        len--;
	}

	for(int i = 0; i < len-2; i++){
        // save positions of 'ans'
        if( 'a' == input[i] && 'n' == input[i+1] && 's' == input[i+2] ){
            toAns.push_back(i);
            i+=2; // skip over the "ns" in "ans"
        }
	}
    // replace 'ans' with value ans
    if(toAns.size() > 0){
        if(ans==ans){       // checks for NaN
            string ans_str = '(' + to_string(ans) + ')';
	        for(int i = toAns.size() - 1; i >= 0; i--){
	        	input.erase(toAns[i],3);
                len-=3;
                input.insert(toAns[i],ans_str);
                len+=ans_str.length();
	        }
        } else{
            throw noHistory;
        }
    }

	for(int i = 0; i < len-1; i++){
        if( 'p' == input[i] && 'i' == input[i+1] ){
            toPi.push_back(i);
            i++; // skip over the 'i' in "pi"
        }
	}
    if(toPi.size() > 0){
        string pi_str = "(3.141592653589793)";
        for(int i = toPi.size() - 1; i >= 0; i--){
            input.erase(toPi[i],2);
            len-=2;
            input.insert(toPi[i],pi_str);
            len+=pi_str.length();
        }
    }

}/*}}}*/
