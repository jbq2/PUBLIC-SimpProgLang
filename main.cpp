#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include "lex.h"


ostream& operator<<(ostream& out, const LexItem& tok) {

    if (tok.GetToken() == PROGRAM) {
        return std::cout << "PROGRAM";
    }
    else if (tok.GetToken() == WRITELN) {
        return std::cout << "WRITELN";
    }
    else if (tok.GetToken() == INTEGER) {
        return std::cout << "INTEGER";
    }
    else if (tok.GetToken() == BEGIN) {
        return std::cout << "BEGIN";
    }
    else if (tok.GetToken() == END) {
        return std::cout << "END";
    }
    else if (tok.GetToken() == IF) {
        return std::cout << "IF";
    }
    else if (tok.GetToken() == REAL) {
        return std::cout << "REAL";
    }
    else if (tok.GetToken() == STRING) {
        return std::cout << "STRING";
    }
    else if (tok.GetToken() == VAR) {
        return std::cout << "VAR";
    }
    else if (tok.GetToken() == ELSE) {
        return std::cout << "ELSE";
    }
    else if (tok.GetToken() == FOR) {
        return std::cout << "FOR";
    }
    else if (tok.GetToken() == THEN) {
        return std::cout << "THEN";
    }
    else if (tok.GetToken() == DO) {
        return std::cout << "DO";
    }
    else if (tok.GetToken() == TO) {
        return std::cout << "TO";
    }
    else if (tok.GetToken() == DOWNTO) {
        return std::cout << "DOWNTO";
    }
    else if (tok.GetToken() == IDENT) {
        std::string lexemeCaps;
        int i = 0;
        char c;
        while(tok.GetLexeme()[i]){
            c = tok.GetLexeme()[i];
            if(tok.GetLexeme()[i] != '_') {
                c = toupper(c);
            }
            lexemeCaps += c;
            i++;
        }
        return std::cout << "IDENT" << "(" << lexemeCaps << ")";
    }
    else if (tok.GetToken() == ICONST) {
        return std::cout << "ICONST" << "(" << tok.GetLexeme() << ")";
    }
    else if (tok.GetToken() == RCONST) {
        return std::cout << "RCONST" << "(" << tok.GetLexeme() << ")";
    }
    else if (tok.GetToken() == SCONST) {
        std::string noquotes = tok.GetLexeme();
        noquotes.erase(remove(noquotes.begin(), noquotes.end(), '\''), noquotes.end());
        return std::cout << "SCONST" << "(" << noquotes << ")";
    }
    else if (tok.GetToken() == PLUS) {
        return std::cout << "PLUS";
    }
    else if (tok.GetToken() == MINUS) {
        return std::cout << "MINUS";
    }
    else if (tok.GetToken() == MULT) {
        return std::cout << "MULT";
    }
    else if (tok.GetToken() == DIV) {
        return std::cout << "DIV";
    }
    else if (tok.GetToken() == ASSOP) {
        return std::cout << "ASSOP";
    }
    else if (tok.GetToken() == LPAREN) {
        return std::cout << "LPAREN";
    }
    else if (tok.GetToken() == RPAREN) {
        return std::cout << "RPAREN";
    }
    else if (tok.GetToken() == COMMA) {
        return std::cout << "COMMA";
    }
    else if (tok.GetToken() == EQUAL) {
        return std::cout << "EQUAL";
    }
    else if (tok.GetToken() == GTHAN) {
        return std::cout << "GTHAN";
    }
    else if (tok.GetToken() == LTHAN) {
        return std::cout << "LTHAN";
    }
    else if (tok.GetToken() == SEMICOL) {
        return std::cout << "SEMICOL";
    }
    else if (tok.GetToken() == COLON) {
        return std::cout << "COLON";
    }
    else if (tok.GetToken() == ERR) {
        return std::cout << "ERR";
    }
    else if (tok.GetToken() == DONE) {
        return std::cout << "DONE";
    }
    else{
        return std::cout << "<<token was not found>>";
    }
}

int main(int argc, char *argv[]) {
    if(argc > 1) {
        ifstream file;

        bool v = false;
        bool iconsts = false;
        bool rconsts = false;
        bool sconsts = false;
        bool ident = false;
        int infcount = 0;

        for(int i = 1; i < argc; i++){
            std::string e = argv[i];
            if(e[0] == '-') {
                if (e == "-v") {
                    v = true;
                }
                else if (e == "-iconst") {
                    iconsts = true;
                }
                else if (e == "-rconst") {
                    rconsts = true;
                }
                else if (e == "-sconst") {
                    sconsts = true;
                }
                else if (e == "-ident") {
                    ident = true;
                }
                else{
                    std::cout << "UNRECOGNIZED FLAG " << e << std::endl;
                    return 0;
                }
            }
            else{
                infcount += 1;
                if(infcount == 1) {
                    file.open(e);
                    if (!file.is_open()) {
                        std::cout << "CANNOT OPEN the File " << e << std::endl;
                        return 0;
                    }
                }
                else{
                    std::cout << "ONLY ONE FILE NAME ALLOWED." << std::endl;
                    return 0;
                }
            }
        }

        if(infcount == 0){
            std::cout << "NO SPECIFIED INPUT FILE NAME." << std::endl;
            return 0;
        }

        int tokCount = 0;
        int ln = 0;
        LexItem tok;

        multimap<std::string, LexItem> resultMap;
        map<std::string, LexItem> identMap;
        map<int, LexItem> vmap;
        map<int, LexItem> intMap;
        multimap<float, LexItem> fltMap;

        int i = 0;
        while ((tok = getNextToken(file, ln)) != DONE && tok != ERR) {
            tokCount += 1;
            if (v) {
                //TODO
                vmap.insert(pair<int, LexItem>(i, tok));
                i++;
            }
            if (iconsts) {
                //TODO

                if (tok.GetToken() == ICONST) {
                    intMap.insert(pair<int, LexItem>(std::stoi(tok.GetLexeme()), tok));
                }
            }
            if (rconsts) {
                //TODO

                if (tok.GetToken() == RCONST) {
                    fltMap.insert(pair<float, LexItem>(std::stof(tok.GetLexeme()), tok));
                }
            }
            if (sconsts) {
                //TODO

                if (tok.GetToken() == SCONST) {
                    resultMap.insert(pair<std::string, LexItem>(tok.GetLexeme(), tok));
                }
            }
            if (ident) {
                //TODO
                if (tok.GetToken() == IDENT && id_or_kw(tok.GetLexeme(), tok.GetLinenum()) == IDENT) {
                    std::string lexemeCaps;
                    int i = 0;
                    char c;
                    while(tok.GetLexeme()[i]){
                        c = tok.GetLexeme()[i];
                        if(tok.GetLexeme()[i] != '_') {
                            c = toupper(c);
                        }
                        lexemeCaps += c;
                        i++;
                    }
                    identMap.insert(pair<std::string, LexItem>(lexemeCaps, tok));
                }
            }
        }

        if(tok.GetToken() == ERR){
            if(v == true){
                vmap.insert(pair<int, LexItem>(i, tok));
            }
            else {
                std::cout << "Error in line " << ln + 1 << " (" << tok.GetLexeme() << ")" << std::endl;
                return 0;
            }
        }
        if(ln == 0){
            std::cout << "Lines: 0" << std::endl;
        }
        else {
            if (v) {
                //TODO

                std::map<int, LexItem>::iterator it;
                for (it = vmap.begin(); it != vmap.end(); it++) {
                    if (it->second.GetToken() == ERR && it->second.GetLexeme() != "CommentEOFException") {
                        std::cout << "Error in line " << it->second.GetLinenum() + 1 << " ("
                                  << it->second.GetLexeme() << ")" << std::endl;
                        return 0;
                    }
                    else if(it->second.GetLexeme() == "CommentEOFException") {
                        std::cout << std::endl;
                        std::cout << "Missing a comment end delimiters \'*)\' at line " << vmap.at(i).GetLinenum()
                                  << std::endl;
                    }
                    else if(it->second.GetToken() != ERR){
                        std::cout << it->second << std::endl;
                    }
                }
            }


            std::cout << "Lines: " << ln << std::endl;
            std::cout << "Tokens: " << tokCount << std::endl;

            if (sconsts) {
                //TODO
                std::cout << "STRINGS:" << std::endl;

                map<std::string, LexItem>::iterator it;
                for (it = resultMap.begin(); it != resultMap.end(); it++) {
                    std::cout << it->second.GetLexeme() << std::endl;
                }
            }
            if (iconsts) {
                //TODO
                std::cout << "INTEGERS:" << std::endl;

                map<int, LexItem>::iterator it;
                for (it = intMap.begin(); it != intMap.end(); it++) {
                    std::cout << it->second.GetLexeme() << std::endl;
                }
            }
            if (rconsts) {
                std::cout << "REALS:" << std::endl;

                map<float, LexItem>::iterator it;
                for (it = fltMap.begin(); it != fltMap.end(); it++) {
                    std::cout << it->first << std::endl;
                }
            }
            if (ident) {
                std::cout << "IDENTIFIERS:" << std::endl;

                map<std::string, LexItem>::iterator it;
                int j = 0;
                for (it = identMap.begin(); j != identMap.size() - 1; it++) {
                    std::cout << it->first << ", ";
                    j++;
                }
                std::cout << it->first<< std::endl;
            }
        }
    }
    else{
        std::cout << "NO SPECIFIED INPUT FILE NAME." << std::endl;
        return 0;
    }

    return 0;
}