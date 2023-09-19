#define TEST_PARSER  // uncomment to run selfgrader for the parser

#include "lexer.h"

typedef enum {
    none,                    // no errors
    lexerErr,                // lexer error
    classExpected,            // keyword class expected
    idExpected,                // identifier expected
    openBraceExpected,        // { expected
    closeBraceExpected,        // } expected
    memberDeclarErr,        // class member declaration must begin with static, field, constructor , function , or method
    classVarErr,            // class variables must begin with field or static
    illegalType,            // a type must be int, char, boolean, or identifier
    semicolonExpected,        // ; expected
    subroutineDeclarErr,    // subrouting declaration must begin with constructor, function, or method
    openParenExpected,        // ( expected
    closeParenExpected,        // ) expected
    closeBracketExpected,    // ] expected
    equalExpected,            // = expected
    syntaxError                // any other kind of syntax error
} SyntaxErrors;
typedef enum {
    Square,
    cicular,
    curly
} brackets;
typedef struct{
    int current;
    brackets* element;
    int size;
} stack;
char* operands;

bool isconstructor;
// every parsing function should return this struct
// the struct contains an error type field (er)
// and a token field (tk) that is set to the token at or near which the error was encountered
// if no errors are encountered er should be set to none
typedef struct
{
    SyntaxErrors er;
    Token tk;
} ParserInfo;

int InitParser ()
// initialise the parser to parse source code in file_name
{
    operands="+-*/&|<>=";
    isconstructor=false;
    return 0;
}

ParserInfo variable()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(!strcmp(t.lx,"this")||t.tp==ID||t.tp==STRING||!strcmp(t.lx,"int")||!strcmp(t.lx,"boolean")||!strcmp(t.lx,"char"))
    {
        Token t2=GetNextToken();
        if(t2.tp==ID)
        {
            Token t3=GetNextToken();
            if(!strcmp(t3.lx,";"))
            {
                return P;
            }
            else if(!strcmp(t3.lx,","))
            {
                Token t4=GetNextToken();
                while(t4.tp!=EOFile&&t4.tp==ID)
                {
                    t4=GetNextToken();
                    if(!strcmp(t4.lx,";"))
                    {
                        return P;
                    }
                    if(!(!strcmp(t4.lx,",")))
                    {
                        P.er=semicolonExpected;
                        P.tk=t3;
                        return P;
                    }
                    t4=GetNextToken();
                }
                P.er=idExpected;
                P.tk=t4;
                return P;
                return P;
            }
            P.er=semicolonExpected;
            P.tk=t3;
            return P;
        }
    }
    P.er=illegalType;
    P.tk=t;
    return P;
}
ParserInfo Parameter()
{
    ParserInfo P;
    P.er=none;
    Token t1=GetNextToken();
    if(!strcmp(t1.lx,")"))
    {
        return P;
    }
    if(!(t1.tp==ID||!strcmp(t1.lx,"int")||!strcmp(t1.lx,"bool")||(!strcmp(t1.lx,"char"))))
    {
        P.tk=t1;
        P.er=closeParenExpected;
        return P;
    }
    Token t2=GetNextToken();
       
    if(!(t2.tp==ID))
    {
        P.tk=t2;
        P.er=idExpected;
        return P;
    }
    Token t3=GetNextToken();
    while(!(!strcmp(t3.lx,")"))&&t3.tp!=EOF&&!strcmp(t3.lx,","))
    {
        Token t4=GetNextToken();
        if(!(t4.tp==ID||(!strcmp(t4.lx,"int"))||(!strcmp(t4.lx,"bool"))||(!strcmp(t4.lx,"char"))))
        {
            P.tk=t4;
            P.er=illegalType;
            return P;
        }
        Token t5=GetNextToken();
        if(!(t5.tp==ID))
        {
            P.tk=t5;
            P.er=idExpected;
            return P;
        }
        t3=GetNextToken();
    }
    if(!strcmp(t3.lx,")"))
    {
        return P;
    }
    P.er=closeParenExpected;
    P.tk=t3;
    return P;
}
ParserInfo circularbracketexpression(bool isstart);
ParserInfo squarebraceketexpression(bool isstart);
ParserInfo equaltoexpression(bool isstart);
ParserInfo expression();
ParserInfo subroutinebodyDeclar();
ParserInfo callparameter(bool isstart);
bool isoperand(char ch)
{
    int len=strlen(operands);
    for(int i=0;i<len;i++)
    {
        if(ch==operands[i])
            return true;
    }
    return false;
}
ParserInfo commaexpression(bool isstart)
{
    ParserInfo P;
    P.er=none;
    Token t1=PeekNextToken();
    if(isstart&&(!strcmp(t1.lx,"~")||!strcmp(t1.lx,"-")))
    {
        GetNextToken();
    }
    t1=PeekNextToken();
    if(!strcmp(t1.lx,"this")||t1.tp==STRING||t1.tp==INT||t1.tp==ID||!strcmp(t1.lx,"(")||!strcmp(t1.lx,"[")||!strcmp(t1.lx,")")||!strcmp(t1.lx,"null")||!strcmp(t1.lx,"false")||!strcmp(t1.lx,"true"))
    {
        if(t1.tp==ID)
        {
            GetNextToken();
            Token t5=PeekNextToken();
            if(!strcmp(t5.lx,"."))
            {
                GetNextToken();
                Token t6=GetNextToken();
                if(t6.tp==ID)
                {
                    Token t7=GetNextToken();
                    if(!strcmp(t7.lx,"("))
                    {
                        callparameter(true);
                    }
                    else{
                        P.er=openBraceExpected;
                        P.tk=t7;
                        return P;
                    }
                }
                else{
                    P.er=idExpected;
                    P.tk=t6;
                    return P;
                }
            }
        }
        if(t1.tp==INT||t1.tp==STRING||!strcmp(t1.lx,"this")||!strcmp(t1.lx,"null")||!strcmp(t1.lx,"false")||!strcmp(t1.lx,"true"))
        {
            GetNextToken();
            
        }
        
        Token t2=PeekNextToken();
        while(true)
        {
            t2=PeekNextToken();
            if(!strcmp(t2.lx,"("))
            {
                GetNextToken();
                P=circularbracketexpression(true);
            }
            if(!strcmp(t2.lx,"["))
            {
                GetNextToken();
                P=squarebraceketexpression(true);
            }
            t2=PeekNextToken();
            if(t2.tp==EOFile)
            {
                break;
            }
            if(!strcmp(t2.lx,",")||!strcmp(t2.lx,")"))
            {
                break;
            }
            P=expression();
            if(P.er!=none)
            {
                return P;
            }
        
        }
        if(t2.tp==EOFile)
        {
            P.er=closeParenExpected;
            P.tk=t2;
            return P;
        }
        return P;
    }
    P.er=syntaxError;
    P.tk=t1;
    return P;
}
ParserInfo callparameter(bool isstart)
{
    ParserInfo P;
    P.er=none;
    Token t=PeekNextToken();
    while(t.tp!=EOFile)
    {
        P=commaexpression(true);
        if(P.er!=none)
            return P;
        t=PeekNextToken();
        if(!strcmp(t.lx,")"))
        {
            GetNextToken();
            break;
        }
        else if(!strcmp(t.lx,","))
        {
            GetNextToken();
        }
    }
    return P;
}

ParserInfo expression()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(isoperand(t.lx[0]))
    {
        Token t3=GetNextToken();
        if(!strcmp(t3.lx,"("))
        {
            return circularbracketexpression(true);
        }
        if(t3.tp==INT||t3.tp==STRING||!strcmp(t3.lx,"this")||!strcmp(t3.lx,"null"))
        {
            return P;
        }
        if(t3.tp==ID)
        {
            Token t1=PeekNextToken();
            Token t2=PeekNextTokentwice();
            if(!strcmp(t1.lx,"."))
            {
                GetNextToken();
                GetNextToken();
                if(t2.tp==ID)
                {
                    Token t3=GetNextToken();
                    if(!strcmp(t3.lx,"("))
                    {
                        return callparameter(true);
                    }
                    P.er=openParenExpected;
                    P.tk=t3;
                    return P;
                }
                P.er=idExpected;
                P.tk=t2;
                return P;
            }
            if(!strcmp(t1.lx,"["))
            {
                GetNextToken();
                return squarebraceketexpression(true);
            }
            return P;
        }
    }
    P.er=syntaxError;
    P.tk=t;
    return P;
}
ParserInfo squarebraceketexpression(bool isstart)
{
    ParserInfo P;
    P.er=none;
    Token t1=PeekNextToken();
    if(isstart&&(!strcmp(t1.lx,"~")||!strcmp(t1.lx,"-")))
    {
        GetNextToken();
    }
    t1=PeekNextToken();
    if(t1.tp==STRING||t1.tp==INT||t1.tp==ID||!strcmp(t1.lx,"(")||!strcmp(t1.lx,"["))
    {
        if(t1.tp==ID)
        {
            GetNextToken();
            Token t5=PeekNextToken();
            if(!strcmp(t5.lx,"."))
            {
                GetNextToken();
                Token t6=GetNextToken();
                if(t6.tp==ID)
                {
                    Token t7=GetNextToken();
                    if(!strcmp(t7.lx,"("))
                    {
                        callparameter(true);
                    }
                    else{
                        P.er=openBraceExpected;
                        P.tk=t7;
                        return P;
                    }
                }
                else{
                    P.er=idExpected;
                    P.tk=t6;
                    return P;
                }
            }
        }
        if(t1.tp==INT||t1.tp==STRING)
        {
            GetNextToken();
            
        }
        Token t2=PeekNextToken();
        while(true)
        {
            t2=PeekNextToken();
            if(!strcmp(t2.lx,"("))
            {
                GetNextToken();
                P=circularbracketexpression(true);
            }
            if(!strcmp(t2.lx,"["))
            {
                GetNextToken();
                P=squarebraceketexpression(true);
            }
            t2=PeekNextToken();
            if(t2.tp==EOFile)
            {
                break;
            }
            if(!strcmp(t2.lx,"]"))
            {
                break;
            }
            if(!isoperand(t2.lx[0]))
            {
                P.er=closeBracketExpected;
                P.tk=t2;
                return P;
            }
            P=expression();
            if(P.er!=none)
            {
                return P;
            }
            
            
            
        }
        if(t2.tp==EOFile)
        {
            P.er=closeBracketExpected;
            P.tk=t2;
            return P;
        }
        GetNextToken();
        return P;
    }
    P.er=syntaxError;
    P.tk=t1;
    return P;
}
ParserInfo circularbracketexpression(bool isstart)
{
    ParserInfo P;
    P.er=none;
    Token t1=PeekNextToken();
    if(isstart&&(!strcmp(t1.lx,"~")||!strcmp(t1.lx,"-")))
    {
        GetNextToken();
    }
    t1=PeekNextToken();
    if(t1.tp==STRING||t1.tp==INT||t1.tp==ID||!strcmp(t1.lx,"(")||!strcmp(t1.lx,"[")||!strcmp(t1.lx,"null")||!strcmp(t1.lx,"this"))
    {
        if(t1.tp==ID)
        {
            GetNextToken();
            Token t5=PeekNextToken();
            if(!strcmp(t5.lx,"."))
            {
                GetNextToken();
                Token t6=GetNextToken();
                if(t6.tp==ID)
                {
                    Token t7=GetNextToken();
                    if(!strcmp(t7.lx,"("))
                    {
                        callparameter(true);
                    }
                    else{
                        P.er=openBraceExpected;
                        P.tk=t7;
                        return P;
                    }
                }
                else{
                    P.er=idExpected;
                    P.tk=t6;
                    return P;
                }
            }
        }
        if(t1.tp==INT||t1.tp==STRING||!strcmp(t1.lx,"null")||!strcmp(t1.lx,"this"))
        {
            GetNextToken();
            
        }
        Token t2=PeekNextToken();
        while(true)
        {
            t2=PeekNextToken();
            if(!strcmp(t2.lx,"("))
            {
                GetNextToken();
                P=circularbracketexpression(true);
            }
            if(!strcmp(t2.lx,"["))
            {
                GetNextToken();
                P=squarebraceketexpression(true);
            }
            t2=PeekNextToken();
            if(t2.tp==EOFile)
            {
                break;
            }
            if(!strcmp(t2.lx,")"))
            {
                break;
            }if(!isoperand(t2.lx[0]))
            {
                P.er=closeParenExpected;
                P.tk=t2;
                return P;
            }
            P=expression();
            if(P.er!=none)
            {
                return P;
            }
            
            
        }
        if(t2.tp==EOFile)
        {
            P.er=closeParenExpected;
            P.tk=t2;
            return P;
        }
        GetNextToken();
        return P;
    }
    P.er=syntaxError;
    P.tk=t1;
    return P;
}
ParserInfo equaltoexpression(bool isstart)
{
    ParserInfo P;
    P.er=none;
    Token t1=PeekNextToken();
    if(isstart&&(!strcmp(t1.lx,"~")||!strcmp(t1.lx,"-")))
    {
        GetNextToken();
        
    }
    t1=PeekNextToken();
    if(t1.tp==STRING||t1.tp==INT||t1.tp==ID||!strcmp(t1.lx,"(")||!strcmp(t1.lx,"[")||!strcmp(t1.lx,"this")||!strcmp(t1.lx,"null")||!strcmp(t1.lx,"false")||!strcmp(t1.lx,"true"))
    {
        if(t1.tp==ID)
        {
            GetNextToken();
            Token t5=PeekNextToken();
            if(!strcmp(t5.lx,"."))
            {
                GetNextToken();
                Token t6=GetNextToken();
                if(t6.tp==ID)
                {
                    Token t7=GetNextToken();
                    if(!strcmp(t7.lx,"("))
                    {
                        callparameter(true);
                    }
                    else{
                        P.er=openBraceExpected;
                        P.tk=t7;
                        return P;
                    }
                }
                else{
                    P.er=idExpected;
                    P.tk=t6;
                    return P;
                }
            }
        }
        if(t1.tp==INT||t1.tp==STRING||!strcmp(t1.lx,"this")||!strcmp(t1.lx,"null")||!strcmp(t1.lx,"false")||!strcmp(t1.lx,"true"))
        {
            GetNextToken();
            
        }
        Token t2=PeekNextToken();
        while(true)
        {
            t2=PeekNextToken();
            if(!strcmp(t2.lx,"("))
            {
                GetNextToken();
                P=circularbracketexpression(true);
            }
            if(!strcmp(t2.lx,"["))
            {
                GetNextToken();
                P=squarebraceketexpression(true);
            }
            t2=PeekNextToken();
            if(P.er!=none)
            {
                return P;
            }
            if(!strcmp(t2.lx,";"))
            {
                break;
            }
            if(t2.tp==EOFile)
            {
                break;
            }
            if(!isoperand(t2.lx[0]))
            {
                P.er=semicolonExpected;
                P.tk=t2;
                return P;
            }
            P=expression();
            
            
        }
        if(t2.tp==EOFile)
        {
            P.er=semicolonExpected;
            P.tk=t2;
            return P;
        }
        GetNextToken();
        return P;
    }
    P.er=syntaxError;
    P.tk=t1;
    return P;
}
ParserInfo let()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(t.tp==ID)
    {
        Token t2=GetNextToken();
        if(!strcmp(t2.lx,"["))
        {
            P=squarebraceketexpression(true);
            if(P.er==none)
            {
                Token t3=GetNextToken();
                if(!strcmp(t3.lx,"="))
                {
                    return equaltoexpression(true);
                }
                P.er=equalExpected;
                P.tk=t3;
                return P;
            }
            return P;
        }
        if(!strcmp(t2.lx,"="))
        {
            return equaltoexpression(true);
        }
        P.er=equalExpected;
        P.tk=t2;
        return P;
    }
    P.er=idExpected;
    P.tk=t;
    return P;
}
ParserInfo if_()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(!strcmp(t.lx,"("))
    {
        P=circularbracketexpression(true);
        if(P.er!=none)
            return P;
        Token t1=GetNextToken();
        if(!strcmp(t1.lx,"{"))
        {
            P=subroutinebodyDeclar();
        }
        if(P.er!=none)
            return P;
        Token t2=PeekNextToken();
        if(!strcmp(t2.lx,"else"))
        {
            GetNextToken();
            Token t3=GetNextToken();
            if(!strcmp(t3.lx,"{"))
            {
                return subroutinebodyDeclar();
            }
            if(!strcmp(t3.lx,"if"))
            {
                GetNextToken();
                return if_();
            }
            P.er=openBraceExpected;
            P.tk=t3;
            return P;
        }
        return P;
    }
    P.er=openParenExpected;
    P.tk=t;
    return P;
}
ParserInfo while_()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(!strcmp(t.lx,"("))
    {
        P=circularbracketexpression(true);
        if(P.er!=none)
            return P;
        Token t1=GetNextToken();
        if(!strcmp(t1.lx,"{"))
        {
            return subroutinebodyDeclar();
        }
        P.er=openBraceExpected;
        P.tk=t1;
        return P;
    }
    return P;
}
ParserInfo do_()
{
   
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    Token t2=PeekNextToken();
    if(t.tp==ID&&!strcmp(t2.lx,"("))
    {
                    Token t3=GetNextToken();
                    if(!strcmp(t3.lx,"("))
                    {
                        P= callparameter(true);
                        if(P.er!=none)
                            return P;
                        Token t4=GetNextToken();
                        if(!strcmp(t4.lx,";"))
                        {
                            return P;
                        }
                        P.er=semicolonExpected;
                        P.tk=t4;
                        return P;
                    }
                    P.er=openParenExpected;
                    P.tk=t3;
                    return P;
    }
    if(t.tp==ID)
    {
        Token t1=GetNextToken();
        Token t2=GetNextToken();
        if(!strcmp(t1.lx,"."))
        {
            if(t2.tp==ID)
            {
                Token t3=GetNextToken();
                if(!strcmp(t3.lx,"("))
                {
                    P= callparameter(true);
                    if(P.er!=none)
                        return P;
                    Token t4=GetNextToken();
                    if(!strcmp(t4.lx,";"))
                    {
                        return P;
                    }
                    P.er=semicolonExpected;
                    P.tk=t4;
                    return P;
                }
                P.er=openParenExpected;
                P.tk=t3;
                return P;
            }
            P.er=idExpected;
            P.tk=t2;
            return P;
        }
        P.er=syntaxError;
        P.tk=t1;
        return P;
    }
    P.er =idExpected;
    P.tk=t;
    return P;
}
ParserInfo return_()
{
    ParserInfo P;
    P.er =none;
    Token t=PeekNextToken();
    if(isconstructor)
    {
        if(!(!strcmp(t.lx,"this")))
        {
            P.er=syntaxError;
            P.tk=t;
            return P;
        }
        t=GetNextToken();
    }
    t=PeekNextToken();
    if(!strcmp(t.lx,";"))
    {
        GetNextToken();
        return P;
    }
    Token tn=PeekNextToken();
    if(!strcmp(tn.lx,"}"))
    {
        P.er=semicolonExpected;
        P.tk=tn;
        return P;
    }
    P=equaltoexpression(true);
    return P;
}
ParserInfo subroutinebodyDeclar()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    
    while(t.tp!=EOF)
    {
        if(!(!strcmp(t.lx,"var")||!strcmp(t.lx,"let")||!strcmp(t.lx,"if")||!strcmp(t.lx,"while")||!strcmp(t.lx,"do")||!strcmp(t.lx,"return")||!strcmp(t.lx,"}")))
        {
            P.er=illegalType;
            P.tk=t;
            return P;
        }
        if(!strcmp(t.lx,"var"))
        {
            P=variable();
        }
        if(!strcmp(t.lx,"let"))
        {
            P=let();
        }
        if(!strcmp(t.lx,"if"))
        {
            P=if_();
        }
        if(!strcmp(t.lx,"while"))
        {
            P=while_();
        }
        if(!strcmp(t.lx,"do"))
        {
            P=do_();
        }
        if(!strcmp(t.lx,"return"))
        {
            P=return_();
        }
        if(!strcmp(t.lx,"}"))
        {
            break;
        }
        if(P.er!=none)
        {
            return P;
        }
        t=GetNextToken();
    }
    if(t.tp==EOFile)
    {
        P.er=closeBraceExpected;
        P.tk=t;
        return P;
    }
    
    return P;
}
ParserInfo constructor()
{
    isconstructor=true;
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(t.tp==ID)
    {
        Token t2=GetNextToken();
        if(!strcmp(t2.lx,"new"))
        {
            Token t3=GetNextToken();
            if(!strcmp(t3.lx,"("))
            {
                P= Parameter();
                if(P.er==none)
                {
                    Token t4=GetNextToken();
                    if(!strcmp(t4.lx,"{"))
                    {
                        P= subroutinebodyDeclar();
                    }
                    else{
                        P.er=openBraceExpected;
                        P.tk=t4;
                        return P;
                    }
                }
            }
            else{
                P.er=openParenExpected;
                P.tk=t3;
                return P;
            }
        }
        else{
            P.er=syntaxError;
            P.tk=t2;
            return P;
        }
    }
    else{
        P.er=idExpected;
        P.tk=t;
        return P;
    }
    isconstructor=false;
    return P;
}
ParserInfo method()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    if(t.tp==ID||!strcmp(t.lx,"void")||!strcmp(t.lx,"int")||!strcmp(t.lx,"boolean")||!strcmp(t.lx,"char"))
    {
    
        Token t2=GetNextToken();
        if(t2.tp==ID)
        {
            Token t3=GetNextToken();
            if(!strcmp(t3.lx,"("))
            {
                P= Parameter();
                if(P.er==none)
                {
                    Token t4=GetNextToken();
                    if(!strcmp(t4.lx,"{"))
                    {
                        return subroutinebodyDeclar();
                    }
                }
                else
                    return P;
            }
            P.er=openParenExpected;
            P.tk=t3;
            return P;
        }
        P.er=idExpected;
        P.tk=t2;
        return P;
    }
    
    P.er=illegalType;
    P.tk=t;
    return P;
}
ParserInfo curlybracesforclass()
{
    ParserInfo P;
    P.er=none;
    Token t=GetNextToken();
    while(t.tp!=EOFile)
    {
        t=GetNextToken();
        if(!(!strcmp(t.lx,"static")||!strcmp(t.lx,"field")||!strcmp(t.lx,"constructor")||!strcmp(t.lx,"method")||!strcmp(t.lx,"function")||!strcmp(t.lx,"}")))
        {
            P.er=illegalType;
            P.tk=t;
            return P;
        }
        if(!strcmp(t.lx,"static")||!strcmp(t.lx,"field"))
        {
            P=variable();
        }
        else if(!strcmp(t.lx,"constructor"))
        {
            P=constructor();
        }
        else if(!strcmp(t.lx,"method"))
        {
            P=method();
        }
        else if(!strcmp(t.lx,"function"))
        {
            P=method();
        }
        else if(!strcmp(t.lx,"}"))
        {
            break;
        }
        else{
            P.er=syntaxError;
            P.tk=t;
        }
        if(P.er!=none)
            return P;
    }
    if(t.tp==EOFile)
    {
        
        P.er=closeBraceExpected;
        P.tk=t;
        return P;
    }
    return P;
}
ParserInfo class_()
{
    ParserInfo P;
    P.er=none;
    Token t1=GetNextToken();
    if(!(!strcmp(t1.lx,"class")))
    {
        P.er=classExpected;
        P.tk=t1;
        return P;
    }
    t1=GetNextToken();
    if(t1.tp==ID)
    {
        Token t1=PeekNextToken();
        if(!strcmp(t1.lx,"{"))
        {
            return curlybracesforclass();
        }
        P.er=openBraceExpected;
        P.tk=t1;
        return P;
    }
    else{
        P.er=idExpected;
        P.tk=t1;
        return P;
    }
    return P;
}
ParserInfo check()
{
    ParserInfo P;
    Token t =PeekNextToken();
    if(t.tp==EOFile)
    {
        P.er=none;
        P.tk=t;
        return P;
    }
    return class_();
}
ParserInfo Parse ()
// parse the input file (the one passed to InitParser)
{
    InitParser();
    ParserInfo P;
    P.er=none;
    P.tk.tp=INT;
    while(P.tk.tp!=EOFile){
        P=check();
        if(P.er!=none)
        {
            if(P.tk.tp==ERR)
            {
                P.er=lexerErr;
                return P;
            }
            return P;
        }
    }
    return P;
}
int StopParser ()
// stop the parser and do any necessary clean up
{
    StopLexer();
    return 0;
}
char* ErrorString (SyntaxErrors e);


