/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
*/

#include "parse.h"
#include <algorithm>
#include <list>


map<string, bool> defVar;
map<string, Token> SymTable;

/*
 * GIVEN
 * */
namespace Parser {
    bool pushed_back = false;
    LexItem	pushed_token;

    static LexItem GetNextToken(istream& in, int& line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem & t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }

}

static int error_count = 0;

/*
 * GIVEN
 * */
int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}


//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
/*
 * GIVEN
 * */
bool Stmt(istream& in, int& line) {
    bool status;
    //cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken()) {

        case WRITELN:
            status = WriteLnStmt(in, line);
            //cout << "After WriteStmet status: " << (status? true:false) <<endl;
            break;

        case IF:
            status = IfStmt(in, line);
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);

            break;

        case FOR:
            status = ForStmt(in, line);

            break;


        default:
            Parser::PushBackToken(t);
            return false;
    }

    return status;
}
//End of Stmt


//WriteStmt:= wi, ExpreList
/*
 * GIVEN
 * */
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    //Evaluate: print out the list of expressions values

    return ex;
}

//ExprList:= Expr {,Expr}
/*
 * GIVEN
 * */
bool ExprList(istream& in, int& line) {
    bool status = false;
    //cout << "in ExprList and before calling Expr" << endl;
    status = Expr(in, line);
    if(!status){
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    if (tok == COMMA) {
        //cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        //cout << "after calling ExprList" << endl;
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}


bool DeclBlock(istream& in, int& line){
    LexItem tok = Parser::GetNextToken(in, line);//First expected token is VAR
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    bool status;
    if(tok.GetToken() != VAR) {//checks if token is VAR
        ParseError(line, "Non-recognizable Declaration Block.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is IDENT (because of DeclStmt)
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    while(tok.GetToken() == IDENT){
        Parser::PushBackToken(tok);//pushback IDENT to be scanned by DeclStmt
        status = DeclStmt(in, line);
        if(!status){//checks if DeclStmt is valid
            ParseError(line, "Invalid Declaration Statement");
            return false;
        }

        tok = Parser::GetNextToken(in, line);//next expected token is SEMICOL
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
        if(tok.GetToken() != SEMICOL){//checks if token is SEMICOL
            ParseError(line, "Missing semicolon");
            return false;
        }

        tok = Parser::GetNextToken(in, line);//get next token to be checked in loop condition
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
    }
    //kicked out if next token is not IDENT
    //then it must be BEGIN or it is invalid
    if(tok.GetToken() != BEGIN){//checks if next token is BEGIN
        ParseError(line, "Missing BEGIN");
        return false;
    }
    Parser::PushBackToken(tok);//push back BEGIN if it is scanned
    //to be scanned in ProgBody

    return status;
}

bool DeclStmt(istream& in, int& line){
    LexItem tok = Parser::GetNextToken(in, line);
    bool status = false;
    if(tok == IDENT){
        if(defVar.find(tok.GetLexeme()) == defVar.end()){
            defVar.insert(pair<string, bool>(tok.GetLexeme(), true));
            tok = Parser::GetNextToken(in, line);
            if(tok.GetToken() == COMMA || tok.GetToken() == COLON){
                Parser::PushBackToken(tok);
                status = DeclStmt(in, line);
            }
            else{
                ParseError(line, "Unrecognized input pattern");
                std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
                ParseError(line, "Incorrect variable in Declaration Statement");
                return false;
            }
        }
        else{
            ParseError(line, "Variable Redefinition");
            ParseError(line, "Incorrect variable in Declaration Statement");
            return false;
        }
    }
    else if(tok.GetToken() == COMMA){
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() != IDENT){
            ParseError(line, "Identifier does not follow comma");
            return false;
        }
        else{
            Parser::PushBackToken(tok);
            status = DeclStmt(in, line);
        }
    }
    else if(tok.GetToken() == COLON){
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == STRING || tok.GetToken() == INTEGER || tok.GetToken() == REAL){
            return true;
        }
        else{
            ParseError(line, "Incorrect Declaration Type.");
            return false;
        }
    }

    return status;
}

bool Prog(istream& in, int& line){
    LexItem tok = Parser::GetNextToken(in, line);//first expected token is PROGRAM
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != PROGRAM){//checks if token is PROGRAM
        ParseError(line, "Missing PROGRAM");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is IDENT
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != IDENT){//checks if token is IDENT
        ParseError(line, "Missing Program Name.");
        return false;
    }
    else{//if it is IDENT
        if(defVar.find(tok.GetLexeme()) == defVar.end()){//if it is not in defVar map yet
            //we add to defVar
            defVar.insert(pair<string, bool>(tok.GetLexeme(), true));
        }
    }

    tok = Parser::GetNextToken(in, line);//next expected token is SEMICOL
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != SEMICOL){//checks if SEMICOL
        ParseError(line, "Missing semicolon");
        return false;
    }

    bool status = DeclBlock(in, line);//run DeclBlock
    if(!status){//checks if DeclBlock is valid
        ParseError(line, "Invalid declaration block");
        return false;
    }

    status = ProgBody(in, line);//run ProgBody
    if(!status){
        ParseError(line, "Incorrect Program Body.");
        return false;
    }

    return status;
}

bool ProgBody(istream& in, int& line){
    bool status;
    LexItem tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    if(tok.GetToken() != BEGIN){//checks if it starts with BEGIN
        ParseError(line, "Missing BEGIN");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//Next expected token should be in stmtStarters list
    std::list<Token> stmtStarters({IDENT, IF, WRITELN, FOR});
    while(std::find(stmtStarters.begin(), stmtStarters.end(), tok.GetToken()) != stmtStarters.end()){
        Parser::PushBackToken(tok);//push back because statement is prefaced with another GetNextToken
        status = Stmt(in, line);
        if(!status){//false status indicates bad statement
            ParseError(line, "Syntactic error in Program Body");
            return false;
        }
        tok = Parser::GetNextToken(in, line);//next expected token after each statement is a semicolon
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
        if(tok.GetToken() != SEMICOL){//checks if next token is a semicolon
            ParseError(line - 1, "Missing semicolon in Statement");
            return false;
        }

        tok = Parser::GetNextToken(in, line);//next expected token can either be one of the stmtStarters or END
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
    }
    //kicked out if not one of the stmtStarters, so next expected is END
    if(tok.GetToken() != END){//checks if next expected is END
        ParseError(line, "Syntactic error in Program Body.");
        return false;
    }

    return status;
}

bool IfStmt(istream& in, int& line){
    //Stmt does not push back token, so next token should not be IF
    LexItem tok = Parser::GetNextToken(in, line);//next expected token is LPAREN
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    if(tok.GetToken() != LPAREN){//checks if token is LPAREN
        ParseError(line, "Missing left parenthesis");
        return false;
    }

    bool status = LogicExpr(in, line);
    if(!status){//checks if logical expr is valid
        ParseError(line, "Invalid logical expression");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is RPAREN
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != RPAREN){//checks if token is RPAREN
        ParseError(line, "Missing ) parenthesis after expression");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is THEN
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != THEN){//checks if next token is THEN
        ParseError(line, "Missing THEN");
        return false;
    }

    status = Stmt(in, line);//Stmt and Stmt inner function calls handle next/pushed back tokens
    if(!status){//checks if statement is valid
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//optional ELSE
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() == ELSE){//checks for optional ELSE
        status = Stmt(in, line);//Stmt and Stmt inner function calls handle next/pushed back tokens
        if(!status){//checks if statement inside ELSE is valid
            ParseError(line, "Invalid statement in if statement (else)");
            return false;
        }
    }
    else{
        //if not ELSE, a valid scanned token will be either SEMICOL or END
        //we push back token for it to be checked in ProgBody
        Parser::PushBackToken(tok);
    }

    return status;
}

bool ForStmt(istream& in, int& line){
    //stmt does not push back, so next scanned token should be acceptable in Var
    bool status = Var(in, line);
    if(!status){//checks if variable is valid
        ParseError(line, "Invalid variable in for statement");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);//next expected token is ASSOP
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != ASSOP){//checks if token is ASSOP
        ParseError(line, "Missing assignment operator");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is an ICONST
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != ICONST){//checks if token is ICONST
        ParseError(line, "Missing integer constant after assignment operator");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is either TO or DOWNTO
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != TO && tok.GetToken() != DOWNTO){//if not TO or DOWNTO, invalid
        ParseError(line, "Missing TO or DOWNTO");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is ICONST
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != ICONST){//checks if token is ICONST
        ParseError(line, "Missing integer constant after DOWNTO/TO");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is DO
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != DO){//checks if token is DO
        ParseError(line, "Missing DO");
        return false;
    }

    status = Stmt(in, line);//status checks for statement validation
    if(!status){//checks if statement is valid
        ParseError(line, "Invalid statement in for statement");
        return false;
    }

    return status;
}

bool AssignStmt(istream& in, int& line){
    //token is pushed back from Stmt, so current token in Var should be an IDENT
    bool status = Var(in, line);
    if(!status){//checks if variable is valid
        ParseError(line,"Invalid variable in assignment statement");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);//next expected token is ASSOP
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != ASSOP){//checks if next token is ASSOP
        ParseError(line, "Missing assignment operator");
        return false;
    }

    status = Expr(in, line);
    if(!status){//checks if expression is valid
        ParseError(line, "Missing Expression in Assignment Statement");
    }

    return status;
}

bool Var(istream& in, int& line){
    //call to Var from AssignStmt pushes back the next token, so get next token returns the expected IDENT
    //call to Var from ForStmt does not push back, so we must get next token anyway
    LexItem tok = Parser::GetNextToken(in, line);//expected token is an IDENT
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() == IDENT){//checks if IDENT
        if(defVar.find(tok.GetLexeme()) == defVar.end()){//if not in defVar, add
            ParseError(line, "Undefined variable");
            return false;
        }
        return true;
    }
    else{//error if not IDENT
        ParseError(line, "Not an identifier");
        return false;
    }
}

bool LogicExpr(istream& in, int& line){
    bool status = Expr(in, line);//expression first expected, so expression called first
    if(!status){
        ParseError(line, "Invalid first expression");
        return false;
    }

    //final function that initial Expr call leads to does not get next token
    LexItem tok = Parser::GetNextToken(in, line);//next token is expected to be a logical op
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() == EQUAL || tok.GetToken() == GTHAN || tok.GetToken() == LTHAN){//checks if next token is logical op
        //we do not get next token from here
        status = Expr(in, line);
        if(!status){//checks if expression is valid
            ParseError(line, "Invalid expression after relational operator");
            return false;
        }
    }
    else{
        ParseError(line, "Missing relational operator");
    }
    return status;
}

bool Expr(istream& in, int& line){
    //This is called from many places, but current token going into this is not the right one
    //in further calls to other functions inside this, we must make sure they end up starting with getting the next token
    bool status = Term(in, line);
    if(!status){//checks if term is valid
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);//next expected might be + or -
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    while(tok.GetToken() == PLUS || tok.GetToken() == MINUS){
        status = Term(in, line);
        if(!status){
            ParseError(line, "Missing operand after operator");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
    }
    //if we get kicked out (next token is not a + nor -, it must be something else
    //we check for this token in another function that calls Expr in a chain
    //push back token so it can be checked
    Parser::PushBackToken(tok);
    return status;
}

bool Term(istream& in, int& line){
    //Functions that call term checked for a token, but do not get next token prior
    bool status = SFactor(in, line);
    if(!status){//checks if SFactor is valid
        return false;
    }

    //function that call to SFactor leads to does not end with get next token, so we must do that
    LexItem tok = Parser::GetNextToken(in, line);//next token is possibly a MULT or DIV
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    while(tok.GetToken() == MULT || tok.GetToken() == DIV){
        status = SFactor(in, line);
        if(!status){
            ParseError(line, "Missing operand after operator");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
    }
    //we get kicked out if checked token is not MULT or DIV
    //this token must be pushed back to be checked in an earlier function that leads to Term
    Parser::PushBackToken(tok);//push back token to be scanned again
    return status;
}

bool SFactor(istream& in, int& line){
    LexItem tok = Parser::GetNextToken(in, line);//satisifies the need to get next token
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    //the function that lead to this does not get next token, so next token is expected
    bool status;
    if(tok.GetToken() == PLUS){//if plus
        status = Factor(in, line, PLUS);
    }
    else if(tok.GetToken() == MINUS){//if minus
        status = Factor(in, line, MINUS);
    }
    else {//if unsigned (anything else will lead to an error in Factor)
        Parser::PushBackToken(tok);//pushback if neither plus nor minus (this will get checked in Factor)
        //it will either be one of the constants, an IDENT, or LPAREN, anything else will lead to error to be caught
        status = Factor(in, line, -1);
    }
    return status;
}

bool Factor(istream& in, int& line, int sign){
    LexItem tok = Parser::GetNextToken(in, line);//satisfies SFactor's requirement of getting next token (if factor was signed)
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    bool status;
    if(tok.GetToken() == IDENT){//checks if IDENT
        if(defVar.find(tok.GetLexeme()) != defVar.end()){
            status = true;
        }
        else{
            ParseError(line, "Undefined variable");
            return false;
        }
    }
    else if(tok.GetToken() == ICONST){//checks if ICONST
        status = true;
    }
    else if(tok.GetToken() == RCONST){//checks if RCONST
        status = true;
    }
    else if(tok.GetToken() == SCONST){//checks if SCONST
        status = true;
    }
    else if(tok.GetToken() == LPAREN){//in case of Expr enclosed in parentheses
        status = Expr(in, line);//run Expr again
        if(!status){//checks if Expr is valid
            ParseError(line, "Invalid expression");
            return false;
        }

        tok = Parser::GetNextToken(in, line);//correct next token should be RPAREN
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
        if(tok.GetToken() != RPAREN){//checks if token is RPAREN
            ParseError(line, "Missing right parenthesis");
            return false;
        }
    }
    else{//if none of the above, token is not valid and invalid pattern
        return false;
    }

    return status;
}



