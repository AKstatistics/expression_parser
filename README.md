# Math Expression Parser, with a simple CLI
#### Fall 2015

## About

This started as a class project for Orion Lawlor's CS301 Assembly programming class in Fall 2015. 
The goal was to correctly parse math expressions involving plus, minus, multiplication, division, and powers (+-\*/^). 
I wrote a recursive parse function. 
It's a bit of a beast of a function chaulk full of complex flow control, but the expression parsing does work reliably. 
The main function has a CLI that is a little lacking.

## How it works
The principal behind the recursive parser is simple:
- The base case parses individual numbers.
- Each higher level of recusion parses the next highest precedence operation. 

For example, say we parse "4 + 5 * (1 + 2)"

1. Find all terms separated by + or - and call parse on each term, return value of sum.

        "4", "5 * (1 + 2)"
2. If no + or -, find all terms separated by * or / and call parse on each term, return value or product.

             "5", "1 + 2"
3. If no +, -, \*, or /, find all terms separated by ^ and call parse on each term, return value of powers.

4. If none of the above conditions are met, parse the individual numbers and return their values.

        "4" = 4, "5" = 5, etc.

When first researching this project I read [this page](http://www.strchr.com/expression_evaluator) which explains this recursive algorithm in greater detail.

## CLI

- You can use "ans" in place of the previously returned answer.
- You can use "pi" for the value of pi.

## Known bugs

The CLI for this has numerous bugs. It was not the focus of this project. 

Some annoying things: 
- You can't use the arrow keys to navigate what you typed, you have to backspace
- You can't use the up arrow to grab previous entries
- Command + D makes the CLI go crazy.

## Use of this code
I publish this code in the hopes that anyone else interested in parsing expressions or tty programming or make, might learn from this code and share their successes and improvements with me. I simply ask that proper credit is attributed for use or modification of this code, and that the aspects that are original or modified are clearly represented as such in the sources. I will try to improve the code, comments, and documentation as I have time. If you happen to use it, I welcome a message about it. Thanks!

## Build

Navigate to the containing folder. Run *make calc*. Now you can run *./calc* and launch the calculator.

If you only run *make* you will instead build *./calc_test_build* which is identical to the other. 
I just have two targets so that I can change the code and build it without messing up the working binary of the program.
This should be the other way around but I'm not actively maintaining this code anyway.
