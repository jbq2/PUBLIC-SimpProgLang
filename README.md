# Simple Programming Language
<span style="font-size:1.25em">**CS280 Term Project.  PLSL - Pascal Like Simple Language.** <br>
This project has **3 parts**: <br>  </span>

1) Lexical Analyzer <span style="color:green">**(COMPLETE)**</span>
2) Parser <span style="color:red">(INCOMPLETE)</span>
3) Interpreter <span style="color:red">(INCOMPLETE)</span>

<span style="font-size:1.5em">**<---Rules of the Language--->**</span>
<br>
**Identifiers**: can start with a letter or underscore followed by letters, digits, or underscores <br>
**Integer Constants**: 1 or more digits <br>
**Real Constants**: 0 or more digits followed by a '.' followed by 1 or more digits <br>
**String Literals**: characters enclosed in single quotes.  New line characters are not permitted in
string literals. <br>
**List of Operators**: <br>
1. \+ (plus)
2. \- (minus)
3. \* (multiplication)
4. \/ (division)
5. := (assignment)
6. ( (open parenthesis)
7. ) (close parenthesis)
8. = (equality)
9. < (less than)
10. \> (greater than)<br><br>
List of Reserved Words: program, end, begin, writeln, if, integer, real,
string, for, to, downto, else, var <br>
<br>


<span style="font-size:1.5em">**<---Simple Programming Language Phase 1: Lexical Analyzer--->**</span>

**<span style="font-size:1.15em">
This project has 3 parts, the first of which being the lexical analyzer. <br>
Essentially, a lexical analyzer is just a pattern matcher. <br>
</span>**
<br>
<br>
Lexical analyzers are front end programs for syntax analyzers, which are
usually for larger constructs like expressions, statements, and code blocks.
Lexical analyzers are for smaller constructs like lexemes and symbols.
<br>
<br>
Each lexeme in the programming language is associated with some sort of token.  For
example, the '+' character has the token PLUS in my implementation of a lexical analyzer.
Another example is the lexeme '20.22' has the token REAL.
<br>
<br>
The lexical analyzer goes through each character, returning a lexical item which
consists of a line number, lexeme, and token.  These lexical item structures
are returned to the syntax analyzer for larger construct analysis.  In the case of this
project, lexical items are passed to the **Parser** portion of the project.  The lexical
analyzer portion of this project consists of three main files:
<br>
<br>

1. <span style="font-size:1.1em">**lex.cpp**</span>
    1. <span style="font-size:1.1em">Implements id_or_kw method, which differentiates identifier and keyword</span>
    2. <span style="font-size:1.1em">Accepts characters from the input file</span>
    3. <span style="font-size:1.1em">Returns a LexItem when a lexeme matches a pattern</span>
    4. <span style="font-size:1.1em">Utilizes a state diagram to transition between analysis states</span>
2. <span style="font-size:1.1em">**lex.h**</span>
    1. <span style="font-size:1.1em">Includes a list of tokens for the PLSL</span>
    2. <span style="font-size:1.1em">Defines LexItem object that is returned by the lexical analyzer</span>
    3. <span style="font-size:1.1em">Defines abstract methods</span>
3. <span style="font-size:1.1em">**main.cpp**</span>
    1. <span style="font-size:1.1em">Applies lex.cpp</span>
    2. <span style="font-size:1.1em">Implements overridden << operator to print LexItem objects properly</span>
    3. <span style="font-size:1.1em">Reads command line arguments that consist of a flag and input file</span>
    4. <span style="font-size:1.1em">Outputs to terminal based on given flags and file</span>
       <br>
       <br>

<span style="font-size:1.5em">**Concerns of my Implementation for Phase 1**</span>
<br>

1. <span style="font-size:1.1em">**Use of if, else if, and else statements over switch statements**</span>
    1. Switch statements seems to be more efficient than if statements (Switch essentially creates a jump table)
    2. My program does not utilize any switch statements--only if statements
    3. This may be an area to optimize in the future
    4. Readability of my code may also drastically increase upon using switch statements
2. <span style="font-size:1.1em">**No use of regex**</span>
    1. I used std::string.find() method in the C++ string library
    2. This is potentially less efficient than regex
    3. Readability may also increase upon using regex
