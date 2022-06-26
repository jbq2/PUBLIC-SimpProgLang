/* Implementation of PLSL Interpreter
 * parseInt.cpp
 * Programming Assignment 3
 * Spring 2022
*/

#include <vector>
#include <string>
#include "parseInt.h"
#include "val.h"
#include <list>
#include <algorithm>
//#include "lex.cpp"
using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects
std::list<string> declStmtVars;//local list object created to store variable names scanned be declstmt

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

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line, vector<string> &IdList);


bool Prog(istream& in, int& line){
    bool b = true;
    Value v = Value(b);
    cout << v << endl;

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


bool DeclBlock(istream& in, int& line){
    LexItem tok = Parser::GetNextToken(in, line);//First expected token is VAR
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    bool status;
    if(tok.GetToken() != VAR) {//checks if token is VAR
        ParseError(line, "Unrecognized Declaration Block.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is IDENT (because of DeclStmt)
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    while(tok.GetToken() == IDENT){
        declStmtVars.clear();
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
    bool status = false;

    LexItem tok = Parser::GetNextToken(in, line);//next expected token should be an IDENT
    if(tok == IDENT){
        if(defVar.find(tok.GetLexeme()) == defVar.end()){//check if variable is not defined yet
            defVar.insert(pair<string, bool>(tok.GetLexeme(), true));//insert into defVar
            declStmtVars.push_back(tok.GetLexeme());//insert into list
            tok = Parser::GetNextToken(in, line);//next expected token is either a comma or colon
            if(tok.GetToken() == COMMA || tok.GetToken() == COLON){
                Parser::PushBackToken(tok);//pushback token to be scanned by recursive call to declstmt
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
    else if(tok.GetToken() == COMMA){//if comma, then there is a list of vars
        tok = Parser::GetNextToken(in, line);//next expected token is an ident
        if(tok.GetToken() != IDENT){
            ParseError(line, "Identifier does not follow comma");
            return false;
        }
        else{
            Parser::PushBackToken(tok);//pushback token to be scanned by recursive call to declstmt
            status = DeclStmt(in, line);
        }
    }
    else if(tok.GetToken() == COLON){//if colon, variable list has ended
        tok = Parser::GetNextToken(in, line);//next expected token is STRING, INTEGER, or REAL
        if(tok.GetToken() == STRING || tok.GetToken() == INTEGER || tok.GetToken() == REAL){
            if(tok.GetToken() == STRING){
                for(const string& s : declStmtVars){//iterate strings in declstmtvars list to insert into SymTable and set types to STRING
                    SymTable.insert(pair<string,Token>(s, STRING));
                }
            }
            else if(tok.GetToken() == INTEGER){//iterate strings in declstmtvars list to insert into SymTable and set types to INTEGER
                for(const string& s : declStmtVars){
                    SymTable.insert(pair<string,Token>(s, INTEGER));
                }
            }
            else{
                for(const string& s : declStmtVars){//iterate strings in declstmtvars list to insert into SymTable and set types to REAL
                    SymTable.insert(pair<string,Token>(s, REAL));
                }
            }
            return true;
        }
        else{
            ParseError(line, "Incorrect Declaration Type.");
            return false;
        }
    }

    return status;
}


//WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;
    ValQue = new queue<Value>;//ValQue is initialized into a new queue object of Values
    t = Parser::GetNextToken(in, line);//next token is expected to be a LPAREN (WRITELN already read)
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);//run ExprList

    if( !ex ) {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }

    t = Parser::GetNextToken(in, line);//next expected token after ExprList is RPAREN (pushed back by ExprList)
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }


    //Evaluate: print out the list of expressions' values
    while (!(*ValQue).empty())//process items in ValQue as long as it is not empty
    {
        Value nextVal = (*ValQue).front();//process front of queue
        cout << nextVal;//print front of queue
        ValQue->pop();//pop the front item
    }
    cout << endl;

    return ex;
}//End of WriteLnStmt


bool ExprList(istream& in, int& line){
    bool status;
    Value val;//Value object created to be passed into Expr (will be populated when Expr completes its call)

    status = Expr(in, line, val);//call Expr, passing val reference as an arg
    if(!status){
        ParseError(line, "Missing Expression");
        return false;
    }

    //at this point, Expr has finished and val has been populated
    ValQue->push(val);//push val to the queue (to be processed in WriteLnStmt)

    LexItem tok = Parser::GetNextToken(in, line);//next token is either a COMMA or something else (something else is to be processed in WriteLnStmt)
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    if(tok == COMMA){//if comma, then there is more than 1 expr in the list
        status = ExprList(in, line);//make recursive call to ExprList
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    else{//if not COMMA or ERR, then exprlist has run successfully
        Parser::PushBackToken(tok);
        return true;
    }

    return status;
}


bool Stmt(istream& in, int& line){
    bool status;
    //prior to entering, token is pushed back to be scanned in Stmt
    LexItem tok = Parser::GetNextToken(in, line);//next expected token is one of the following

    switch(tok.GetToken()){
        case WRITELN://WRITELN case, enter WriteLnStmt func
            status = WriteLnStmt(in, line);
            break;

        case IF://IF case: enter IFStmt func
            status = IfStmt(in, line);
            break;

        case IDENT://IDENT case: enter assignment statement
            Parser::PushBackToken(tok);//pushback because assignment statement has to process the identifier
            status = AssignStmt(in, line);
            break;

        default://error case
            Parser::PushBackToken(tok);
            return false;
    }
    return status;
}


bool ProgBody(istream& in, int& line){
    bool status;
    //BEGIN checked in DeclBlock, pushed back to be scanned in ProgBody
    LexItem tok = Parser::GetNextToken(in, line);//next expected token is BEGIN
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "<" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    if(tok.GetToken() != BEGIN){
        ParseError(line, "Missing BEGIN");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//net expected token is IDENT, IF, or WRITELN
    std::list<Token> stmtStarters({IDENT, IF, WRITELN});//const list of stmt starters
    while(std::find(stmtStarters.begin(), stmtStarters.end(), tok.GetToken()) != stmtStarters.end()){//process stmt if next read token is a stmt starter
        Parser::PushBackToken(tok);
        status = Stmt(in, line);//run stmt to process stmt
        if(!status){
            ParseError(line, "Syntactic error in Program Body.");
            return false;
        }

        tok = Parser::GetNextToken(in, line);//next expected is semicol after each stmt is read
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }

        if(tok.GetToken() != SEMICOL){
            ParseError(line - 1, "Missing semicolon in Statement");
            return false;
        }

        tok = Parser::GetNextToken(in, line);//next expected is either a stmt starter or END
        //go back to top of while loop--exits if next token is not a statement starter
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
    }

    if(tok.GetToken() != END){//after all statements, program must end with END
        ParseError(line, "Syntactic error in Program Body.");
        return false;
    }

    return status;
}


//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value & retVal) {
    Value val1, val2;//2 value objects created--val1 for lhs of operator, val2 for rhs of operator
    //cout << "in Expr" << endl;
    bool t1 = Term(in, line, val1);//process value of val1 (by running Term--mult and div operators)
    LexItem tok;

    if( !t1 ) {
        return false;
    }
    //at this point, val1 has been fully processed
    retVal = val1;//set retVal to val1

    tok = Parser::GetNextToken(in, line);//next expected token is a PLUS or MINUS
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    //Evaluate: evaluate the expression for addition or subtraction
    while ( tok == PLUS || tok == MINUS )
    {
        t1 = Term(in, line, val2);//process value of val2 (by running Term--mult and div operators)
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }
        //tok here is still either PLUS or MINUS (get next token has not yet been called)
        //at this point val2 has been populated
        if(tok == PLUS)//if PLUS, perform addition of Value objects
        {
            retVal = retVal + val2;//add 2 value objects
            if(retVal.IsErr())//return an error if retVal is an error
            {
                ParseError(line, "Illegal addition operation.");
                //cout << "(" << tok.GetLexeme() << ")" << endl;
                return false;
            }
        }
        else if(tok == MINUS)// if MINUS, perform subtract of Value objects
        {
            retVal = retVal - val2;//subtract 2 Value objects
            if(retVal.IsErr())//return an error if retVal is an error
            {
                ParseError(line, "Illegal subtraction operation.");
                return false;
            }
        }

        tok = Parser::GetNextToken(in, line);//next token should be whatever is after the entire expression (SEMICOL, COMMA, etc.)
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    Parser::PushBackToken(tok);//pushback token to be processed by caller of Expr
    return true;
}//end of Expr


bool AssignStmt(istream& in, int& line){
    //prior to entering assignstmt, scanned token in stmt was pushed back
    LexItem idtok = Parser::GetNextToken(in, line);//next expected token is an IDENT
    if(idtok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << idtok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(idtok.GetToken() != IDENT){
        ParseError(line, "Invalid identifier");
        return false;
    }

    bool status = Var(in, line, idtok);//pass idtok into Var
    if(!status){
        ParseError(line, "Invalid variable in assignment statement");
        return false;
    }

    //create new lexitem for normal scanning
    //this is because idtok token will be required to insert into SymTable later
    LexItem tok = Parser::GetNextToken(in, line);//next expected token is ASSOP
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    if(tok.GetToken() != ASSOP){
        ParseError(line, "Missing assignment operator");
        return false;
    }

    Value val;//creat val object
    status = Expr(in, line, val);//pass val into Expr
    if(!status){
        ParseError(line, "Missing Expression in Assignment Statement");
        return false;
    }

    //at this point, val will be populated
    if((SymTable.at(idtok.GetLexeme()) == STRING && val.GetType() == VSTRING) ||
            (SymTable.at(idtok.GetLexeme()) == INTEGER && val.GetType() == VINT) ||
            (SymTable.at(idtok.GetLexeme()) == REAL && val.GetType() == VREAL)){//check for matching types

        if(TempsResults.find(idtok.GetLexeme()) == TempsResults.end()){//if the variable has not yet been assigned a value
            TempsResults.insert(pair<string, Value>(idtok.GetLexeme(), val));//if so, insert into TempsResults idtok lexeme and the value associated with idtok
        }
        else{//if the variable has already been assigned a value
            TempsResults[idtok.GetLexeme()] = val;
        }
    }
    else if(SymTable.at(idtok.GetLexeme()) == REAL && val.GetType() == VINT){
        float r = float(val.GetInt());
        val = Value(r);

        if(TempsResults.find(idtok.GetLexeme()) == TempsResults.end()){
            TempsResults.insert(pair<string, Value>(idtok.GetLexeme(), val));//if so, insert into TempsResults idtok lexeme and the value associated with idtok
        }
        else{
            TempsResults[idtok.GetLexeme()] = val;
        }
    }
    else if(SymTable.at(idtok.GetLexeme()) == INTEGER && val.GetType() == VREAL){
        int i = int(val.GetReal());
        val = Value(i);

        if(TempsResults.find(idtok.GetLexeme()) == TempsResults.end()){
            TempsResults.insert(pair<string, Value>(idtok.GetLexeme(), val));//if so, insert into TempsResults idtok lexeme and the value associated with idtok
        }
        else{
            TempsResults[idtok.GetLexeme()] = val;
        }
    }
//    if(SymTable.at(idtok.GetLexeme()) == STRING && val.GetType() == VSTRING){
//        TempsResults.insert(pair<string, Value>(idtok.GetLexeme(), val));
//    }
//    else if(SymTable.at(idtok.GetLexeme()) == INTEGER && val.GetType() == VINT){
//        TempsResults.insert(pair<string, Value>(idtok.GetLexeme(), val));
//    }
//    else if(SymTable.at(idtok.GetLexeme()) == REAL && val.GetType() == VREAL){
//        TempsResults.insert(pair<string, Value>(idtok.GetLexeme(), val));
//    }
    else{//in the case of unmatching types, return an error
        ParseError(line, "Illegal Assignment Operation");
        return false;
    }

    return status;
}


bool Var(istream& in, int& line, LexItem& idtok){//passed in idtok is the identifier lex item
    if(idtok.GetToken() == IDENT){//check if idtok is an IDENT (it should be)
        if(defVar.find(idtok.GetLexeme()) == defVar.end()){//check if identifier is undefined
            ParseError(line, "Undefined variable");
            return false;
        }
        return true;
    }
    else{//case that passed in token is not an identifier
        ParseError(line, "Not an identifier");
        return false;
    }
}


bool LogicExpr(istream& in, int& line, Value& retVal){
    Value val1, val2;//logical expressions are binary operations
    bool status = Expr(in, line, val1);//pass val1 into Expr
    if(!status){
        ParseError(line, "Invalid first expression");
    }

    //at this point, val1 will be populated
    retVal = val1;

    LexItem tok = Parser::GetNextToken(in, line);//net expected token is a comparison operator
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() == EQUAL || tok.GetToken() == GTHAN || tok.GetToken() == LTHAN){
        status = Expr(in, line, val2);//pass val2 into Expr
        if(!status) {
            ParseError(line, "Invalid expression after relational operator");
            return false;
        }

        //at this point, val2 will be populated
        if(tok.GetToken() == EQUAL){//equal case
            retVal = retVal == val2;
        }
        if(tok.GetToken() == GTHAN){// > case
            retVal = retVal > val2;
        }
        if(tok.GetToken() == LTHAN){// < case
            retVal = retVal < val2;
        }

        if(retVal.IsErr()){
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
            return false;
        }
    }
    else{
        ParseError(line, "Missing relational operator");
        return false;
    }

    return status;
}


bool IfStmt(istream& in, int& line){
    //previously scanned token (IF) was not pushed back
    LexItem tok = Parser::GetNextToken(in, line);//next token epected is LPAREN

    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
    }

    if(tok.GetToken() != LPAREN){
        ParseError(line, "Missing left parenthesis");
        return false;
    }

    Value logicExprVal;//value object created to get value from LogicExpr
    bool status = LogicExpr(in, line, logicExprVal);//value object passed into LogicExpr
    if(!status){
        ParseError(line, "Missing if statement Logic Expression");
        return false;
    }
    //at this point, logicExprVal has been populated

    tok = Parser::GetNextToken(in, line);//next expected token is a RPAREN
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != RPAREN){
        ParseError(line, "Missing ) parenthesis after expression");
        return false;
    }

    tok = Parser::GetNextToken(in, line);//next expected token is a THEN
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() != THEN){
        ParseError(line, "Missing THEN");
        return false;
    }

    //logicExprVal is of type boolean and should have a boolean associated with it
    //its boolean is the result of the comparison made in the LogicExpr func
    if(logicExprVal.GetBool()){//process next statement (if logic expr val is true)
        status = Stmt(in, line);//process statement within if statement
        if(!status){
            ParseError(line, "Invalid statement in if statement (if)");
            return false;
        }
    }
    else{//skip next statement (logic expr value is false)
        while(tok.GetToken() != ELSE && tok.GetToken() != SEMICOL){//run loop until scanned token is not an ELSE (marks begining of ELSE) or SEMICOL (marks ends of entire IF)
            tok = Parser::GetNextToken(in, line);
            if(tok.GetToken() == ERR){
                ParseError(line, "Unrecognized input patter");
                std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
                return false;
            }
        }
        //pushback token since SEMICOL or ELSE was scanned and needs to be scanned again later on
        Parser::PushBackToken(tok);
    }

    tok = Parser::GetNextToken(in, line);//get next token to scan for ELSE or something else
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }
    if(tok.GetToken() == ELSE){//if else
        if(!logicExprVal.GetBool()){
            status = Stmt(in, line);//process else statement
            if(!status){
                ParseError(line, "Invalid statement in if statement (else)");
                return false;
            }
        }
        else{
            while(tok.GetToken() != SEMICOL){//run loop until scanned token is not an ELSE (marks begining of ELSE) or SEMICOL (marks ends of entire IF)
                tok = Parser::GetNextToken(in, line);
                if(tok.GetToken() == ERR){
                    ParseError(line, "Unrecognized input patter");
                    std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
                    return false;
                }
            }
            //pushback token since SEMICOL or ELSE was scanned and needs to be scanned again later on
            Parser::PushBackToken(tok);
        }
    }
    else{//otherwise it is probably SEMICOL (pushback in this case)
        Parser::PushBackToken(tok);
    }

    return status;
}


bool Term(istream& in, int& line, Value& retVal){
    Value val1, val2;//create 2 value objects
    //val1 is for lhs of operator, val2 is for rhs of operator

    //Functions that call term checked for a token, but do not get next token prior
    bool status = SFactor(in, line, val1);//pass val1 as an arg of SFactor
    if(!status){//checks if SFactor is valid
        return false;
    }

    //at this point val1 is populated from SFactor
    retVal = val1;

    //get next token has been called multiple times in SFactor and the calls inside SFactor
    LexItem tok = Parser::GetNextToken(in, line);//next expected token is MULT or DIV, or something else due to end of Term
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    while(tok.GetToken() == MULT || tok.GetToken() == DIV){
        status = SFactor(in, line, val2);//pass val2 as an arg of SFactor
        if(!status){
            ParseError(line, "Missing operand after operator");
            return false;
        }

        //at this point, val2 will be populated from previous call to SFactor
        //tok has not been assigned the next token, so it is still either MULT or DIV
        if(tok.GetToken() == MULT){
            retVal = retVal * val2;//multiple 2 Value objects if MULT
            if(retVal.IsErr()){//return an error if retVal is an error
                ParseError(line, "Invalid multiplication operation");
                return false;
            }
        }
        else if(tok.GetToken() == DIV){
            if((val2.GetType() == VINT && val2.GetInt() == 0) || (val2.GetType() == VREAL && val2.GetReal() == 0)){
                ParseError(line, "Run-Time Error-Illegal Division by Zero");
                return false;
            }
            retVal = retVal / val2;//divide 2 Value  objects
            if(retVal.IsErr()){//return an error if retVal is an error
                ParseError(line, "Invalid division operation");
                return false;
            }
        }

        tok = Parser::GetNextToken(in, line);//ensure that next token is not an error
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized input pattern");
            std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
            return false;
        }
        //return to top of while loop and re-enter until next token is not a MULT or DIV
    }

    //this token must be pushed back to be checked in an earlier function that leads to Term
    Parser::PushBackToken(tok);//push back token to be scanned again
    return status;
}


bool SFactor(istream& in, int& line, Value & retVal){
    Value val1;//only 1 val object created--SFactor deals with unary PLUS or MINUS operator

    LexItem tok = Parser::GetNextToken(in, line);//next expected token is either PLUS, MINUS, IDENTIFIER, or one of the constants
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    //the function that lead to this does not get next token, so next token is expected
    bool status;
    if(tok.GetToken() == PLUS){//if plus
        status = Factor(in, line, PLUS, val1);//pass val1 into Factor with sign PLUS
    }
    else if(tok.GetToken() == MINUS){//if minus
        status = Factor(in, line, MINUS, val1);//pass val1 into Factor with sign MINUS
    }
    else {//if unsigned (either an identifier or constant)
        Parser::PushBackToken(tok);//pushed back to be scanned in Factor
        //it will either be one of the constants, an IDENT, or LPAREN, anything else will lead to error to be caught
        status = Factor(in, line, -1, val1);//pass val1 into Factor with no sign (-1)
    }

    //at this point, val1 will be populated
    retVal = val1;
    if(retVal.IsErr()){
        return false;
    }

    return status;
}

bool Factor(istream& in, int& line, int sign, Value & retVal){

    LexItem tok = Parser::GetNextToken(in, line);//next expected token is either IDENT, constant, or LPAREN
    //in case of IDENT and LPAREN, those are pushed back prior to this call
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized input pattern");
        std::cout << "(" << tok.GetLexeme() << ")" << std::endl;
        return false;
    }

    bool status;
    if(tok.GetToken() == IDENT){//checks if IDENT
        if(defVar.find(tok.GetLexeme()) == defVar.end()){//TODO: add logic set to retVal to value of identifier
            ParseError(line, "Undefined variable");
            return false;
        }

        if(TempsResults.find(tok.GetLexeme()) == TempsResults.end()){
            ParseError(line, "Unassigned variable");
            return false;
        }

        if(sign != -1){//if factor is signed (sign is not -1)
            if(SymTable.at(tok.GetLexeme()) == REAL || SymTable.at(tok.GetLexeme()) == INTEGER){//apply sign changes to REAL or INTEGER only
                if(sign == MINUS){//MINUS sign case
                    retVal = TempsResults.at(tok.GetLexeme()) * Value(-1); //set retVal to negated value of identifier
                }
                else{
                    retVal = TempsResults.at(tok.GetLexeme()); //set retVal to normal value of identifier
                }
            }
            else{//if not REAL or INTEGER (can only be string)
                ParseError(line, "Illegal sign for identifier of type STRING");
                return false;
            }
        }
        else{//if unsigned (sign is -1)
            retVal = TempsResults.at(tok.GetLexeme());//set retVal to normal value of identifier
            //checking of type is unnecessary
        }

        status = true;
    }
    else if(tok.GetToken() == ICONST){//checks if ICONST
        if(sign == MINUS){//MINUS sign case
            retVal = Value(stoi(tok.GetLexeme())) * Value(-1);//set retVal to negated value of integer
        }
        else{//PLUS or no sign
            retVal = Value(stoi(tok.GetLexeme()));//set retVal to normal value of integer
        }

        if(retVal.IsErr()){//if there was an error with the above operations
            ParseError(line, "Illegal integer constant");
            return false;
        }

        status = true;
    }
    else if(tok.GetToken() == RCONST){//checks if RCONST
        if(sign == MINUS){//MINUS sign case
            retVal = Value(stof(tok.GetLexeme())) * Value(-1.0f);//set retVal to negated value of float
        }
        else{
            retVal = Value(stof(tok.GetLexeme()));//set retVal to normal value of float
        }

        if(retVal.IsErr()){//if there was an error with the above operations
            ParseError(line, "Illegal real constant");
            return false;
        }

        status = true;
    }
    else if(tok.GetToken() == SCONST){//checks if SCONST
        if(sign == -1){//only possibility for sign when the scanned token is SCONST is no sign (sign = -1)
            retVal = Value(tok.GetLexeme());//set retVal to value of string
        }
        else{//signed case (error case)
            ParseError(line, "Illegal Operand Type for Sign Operator");
            return false;
        }

        if(retVal.IsErr()){//checks if above operation lead to error
            ParseError(line, "Illegal string constant");
            return false;
        }

        status = true;
    }
    else if(tok.GetToken() == LPAREN){//in case of Expr enclosed in parentheses
        status = Expr(in, line, retVal);//run Expr again
        if(!status){//checks if Expr is valid
            ParseError(line, "Invalid expression");
            return false;
        }
        //at this point, retVal will be be populated

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