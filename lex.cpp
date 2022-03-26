#include <iostream>
#include <string>
#include "lex.h"
#include <map>
#include <algorithm>

LexItem id_or_kw(const string& lexeme, int linenum){
    LexItem l;
    std::string upperLexeme = lexeme;
    std::transform(upperLexeme.begin(), upperLexeme.end(), upperLexeme.begin(), ::toupper);

    //following map only holds tokens that are KEYWORDS
    std::map<std::string, Token> d = {
            {"PROGRAM", PROGRAM},
            {"WRITELN", WRITELN},
            {"INTEGER", INTEGER},
            {"BEGIN", BEGIN},
            {"END", END},
            {"IF", IF},
            {"REAL", REAL},
            {"STRING", STRING},
            {"VAR", VAR},
            {"ELSE", ELSE},
            {"FOR", FOR},
            {"THEN", THEN},
            {"DO", DO},
            {"TO", TO},
            {"DOWNTO", DOWNTO}
    };

    if(d.count(upperLexeme) > 0){
        return LexItem(d.at(upperLexeme), lexeme, linenum);
    }
    else{
        return LexItem(IDENT, upperLexeme, linenum);
    }
}

LexItem getNextToken(istream& in, int& linenum){
    enum TokState {START, INID, INSTRING, ININT, INREAL, INCOMMENT};

    std::map<char, Token> opsMap = { //map for operators, parens, punctuation, col/semicol
            {'+', PLUS},
            {'-', MINUS},
            {'*', MULT},
            {'/', DIV},
            {'(', LPAREN},
            {')', RPAREN},
            {',', COMMA},
            {'=', EQUAL},
            {'>', GTHAN},
            {'<', LTHAN},
            {';', SEMICOL},
            {':', COLON},
            {'.', DOT},
    };

    TokState lexstate = START;
    LexItem l;
    std::string line;
    std::string lexeme;
    char c;

    std::string numsValid = "0123456789";
    std::string identValid = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_";
    std::string newlines = "\n\r";
    std::string whitespaces = " \t";

    while(in.get(c)) {
        if (lexstate == START) { //LEXSTATE START HANDLING
            if (newlines.find(c) != std::string::npos) { //handling newline char
                if(c == '\r' && in.peek() == '\n'){
                    linenum++;
                    in.get(c);
                }
                else{
                    linenum++;
                }
                continue;
            }
            if (whitespaces.find(c) != std::string::npos) { //handling whitespaces
                continue;
            }
            if(c == '\"'){
                return LexItem(ERR, "Double quotes ERR", linenum);
            }
            if (identValid.find(c) != std::string::npos){ //handling identifier starter
                lexstate = INID;
                lexeme += c;
            }
            else if (numsValid.find(c) != std::string::npos){ //handling iconst starter
                lexstate = ININT;
                lexeme += c;
            }
            else if (c == ':' && in.peek() == '='){ //handling assop
                in.get(c);
                return LexItem(ASSOP, ":=", linenum);
            }
            else if(c == '.' && numsValid.find(in.peek()) != std::string::npos){ //handling rconst
                lexstate = INREAL;
                lexeme = "0.";
            }
            else if (c == '\''){ //handling sconst
                lexstate = INSTRING;
                lexeme += c;
            }
            else if(c == '(' && in.peek() == '*'){ //handling comment starter
                lexstate = INCOMMENT;
                in.get(c);
            }
            else if(opsMap.count(c) >= 1){
                std::string op;
                op += c;
                return LexItem(opsMap.at(c), op, linenum);
            }
            else{
                lexeme += c;
                return LexItem(ERR, lexeme, linenum);
            }//END OF LEXSTATE START HANDLING
        }

            //OTHER LEXSTATE STATES
        else if(lexstate == INID){
            if(numsValid.find(c) != std::string::npos || identValid.find(c) != std::string::npos){
                //add if c is a valid identifier char
                lexeme += c;
            }
            else if(opsMap.count(c) >= 1 || whitespaces.find(c) != std::string::npos){
                in.putback(c);
                LexItem idCheck = id_or_kw(lexeme, linenum);
                if(idCheck.GetToken() != IDENT){
                    return LexItem(idCheck.GetToken(), lexeme, linenum);
                }
                return LexItem(IDENT, lexeme,linenum);
            }
            else if(newlines.find(c) != std::string::npos){
                in.putback(c);
                LexItem idCheck = id_or_kw(lexeme, linenum);
                if(idCheck.GetToken() != IDENT){
                    return LexItem(idCheck.GetToken(), lexeme, linenum);
                }
                return LexItem(IDENT, lexeme,linenum);
            }
            else{
                in.putback(c);
                LexItem idCheck = id_or_kw(lexeme, linenum);
                if(idCheck.GetToken() != IDENT){
                    return LexItem(idCheck.GetToken(), lexeme, linenum);
                }
                return LexItem(IDENT, lexeme,linenum);
            }
        }
        else if(lexstate == INSTRING){
            if(c == '\''){
                lexeme += c;
                return LexItem(SCONST, lexeme, linenum);
            }
            else if(c == '\"'){
                //ERR if end in '\"' or if c is newline char
                lexeme += c;
                return LexItem(ERR, lexeme, linenum);
            }
            else if(newlines.find(c) != std::string::npos){
                return LexItem(ERR, lexeme, linenum);
            }
            lexeme += c;
        }
        else if(lexstate == ININT){
            if(c == '.'){
                if(numsValid.find(in.peek()) == std::string::npos){
                    //ERR if '.' followed by non-number
                    lexeme += c;
                    return LexItem(ERR, lexeme, linenum);
                }
                else{
                    //otherwise, switch state to INREAL
                    lexstate = INREAL;
                    lexeme += c;
                }
            }
            else if(numsValid.find(c) != std::string::npos){
                //add to lexeme if c is a number
                lexeme += c;
            }
            else if(opsMap.count(c) >= 1 || whitespaces.find(c) != std::string::npos){
                //return LexItem if c is operator, whitespace, newline, or eof
                in.putback(c);
                return LexItem(ICONST, lexeme, linenum);
            }
            else if(newlines.find(c) != std::string::npos){
                in.putback(c);
                return LexItem(ICONST, lexeme, linenum);
            }
            else{
                in.putback(c);
                return LexItem(ICONST, lexeme, linenum);
            }
        }
        else if(lexstate == INREAL){
            if(c == '.') {
                //ERR f encounter second '.'
                lexeme += c;
                return LexItem(ERR, lexeme, linenum);
            }
            else if (numsValid.find(c) != std::string::npos){
                //add to lexeme if c is a number
                lexeme += c;
            }
            else if (opsMap.count(c) >= 1 || whitespaces.find(c) != std::string::npos){
                //return LexItem if c is operator, whitespace, newline, or eof
                in.putback(c);
                return LexItem(RCONST, lexeme, linenum);
            }
            else if(newlines.find(c) != std::string::npos){
                in.putback(c);
                return LexItem(RCONST, lexeme, linenum);
            }
            else{
                //ERR if none of the above
                lexeme += c;
                return LexItem(ERR, lexeme, linenum);
            }
        }
        else { //for comments
            if (newlines.find(c) != std::string::npos) { //handling newline char
                if (c == '\r' && in.peek() == '\n') {
                    linenum++;
                    in.get(c);
                } else {
                    linenum++;
                }
                continue;
            }
            if (c == '*' && in.peek() == ')') {
                in.get(c);
                lexstate = START;
            }
        }
    }

    if(lexstate == INCOMMENT){ //end of file reached without closing comment
        return LexItem(ERR, "CommentEOFException", linenum);
    }
    else if(lexstate == INSTRING){ //end of file reached without closing string
        return LexItem(ERR, "StringEOFException", linenum);
    }

    return LexItem(DONE, "DONE", linenum);
}

