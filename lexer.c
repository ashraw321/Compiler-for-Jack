
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include <stdbool.h>
struct Trie{
    bool isend;
    struct Trie* nextchar[26];
};

struct Trie* Reservedword;
struct Trie* Datatype;
int currlinenumber;
// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE
FILE* file;
char* filename;
char* symbols;
bool isasymbol(char ch);
bool peakahead()
{
    char ch=getc(file);
    if(ch!='\xff')
    fseek(file,-1,SEEK_CUR);
    
        if(ch==EOF||ch==' '||ch=='\n'||ch=='\r'||ch=='\t'||ch=='\xff')
        {
            return true;
        }
        else if(isasymbol(ch))
            return true;
    return false;
}
bool checktoken(char ch)
{
    if(isasymbol(ch)||ch==EOF||ch==' '||ch=='\n'||ch=='r'||ch=='t'||ch=='\xff')
        return true;
    else return false;
}
void create(int index,char* str,struct Trie* curr)
{
    if(index==strlen(str))
    {
        curr->isend=true;
        return;
    }
    int i=str[index]-'a';
    if(curr->nextchar[i]==NULL)
    {
        curr->nextchar[i]=malloc(sizeof(struct Trie));
        curr->nextchar[i]->isend=false;
        for(int j=0;j<26;j++)
        {
            curr->nextchar[i]->nextchar[j]=NULL;
        }
        create(index+1,str,curr->nextchar[i]);
    }
    else{
        create(index+1,str,curr->nextchar[i]);
    }
    return ;
    
}
void createtrieforRES(struct Trie *Reservedword)
{
    create(0,"class",Reservedword);
    create(0,"constructor",Reservedword);
    create(0,"function",Reservedword);
    create(0,"method",Reservedword);
    create(0,"field",Reservedword);
    create(0,"static",Reservedword);
    create(0,"var",Reservedword);
    create(0,"int",Reservedword);
    create(0,"char",Reservedword);
    create(0,"boolean",Reservedword);
    create(0,"void",Reservedword);
    create(0,"true",Reservedword);
    create(0,"false",Reservedword);
    create(0,"null",Reservedword);
    create(0,"this",Reservedword);
    create(0,"let",Reservedword);
    create(0,"do",Reservedword);
    create(0,"if",Reservedword);
    create(0,"else",Reservedword);
    create(0,"while",Reservedword);
    create(0,"return",Reservedword);
}
bool isasymbol(char ch)
{
    for(int i=0;i<19;i++)
    {
        if(ch==symbols[i])
            return true;
    }
    return false;
}
struct Trie* locationinTrie(char ch,struct Trie* curr)
{
    if(!(ch>='a'&&ch<='z'))
        return NULL;
    int i=ch-'a';
    return curr->nextchar[i];
}
void clean(struct Trie* R)
{
    if(R==NULL)
        return;
    for(int i=0;i<26;i++)
    {
        clean(R->nextchar[i]);
    }
    return;
}

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
    
    file=fopen(file_name,"r");
    symbols="{}[](),;=.+-*/&|~<>";
    Reservedword= malloc(sizeof(struct Trie));
    for(int i=0;i<27;i++)
        Reservedword->nextchar[i]=NULL;
    Reservedword->isend=false;
    currlinenumber=1;
    if(file==NULL)
    {
        printf("Error! couldnot open the file\n");
        return 0;
    }
    
    createtrieforRES(Reservedword);
    filename=file_name;
    return 1;
}
void insert(char**stringsaved,char ch,int* sizeofstringsaved,int size)
{
    if(size>=(*sizeofstringsaved)-1)
    {
        
        *sizeofstringsaved=2*(*sizeofstringsaved)+3;
        char* temp= malloc((*sizeofstringsaved)*sizeof(char));
        int i=0;
        if(size!=1)
        for(;i<size-1;i++)
        {
            char t=(*stringsaved)[i];
            temp[i]=t;
        }
        temp[i]=ch;
        i++;
        free(*stringsaved);
        temp[i]='\0';
        *stringsaved=temp;
    }
    else
    {
        int len=strlen(*stringsaved);
        (*stringsaved)[len]=ch;
        len++;
        (*stringsaved)[len]='\0';
    }
    return;
}
bool isvalidIDcharacter(char ch)
{
    if(ch=='_'||!(ch<'0'||ch>'9')||!(ch<'a'||ch>'z')||!(ch<'A'||ch>'Z'))
        return true;
    else return false;
}
// Get the next token from the source file
void createtokenforendoffile(Token* t)
{
    t->tp=EOFile;
    char *str="End of File";
    unsigned long int len=strlen(str);
    int i=0;
    for(i=0;i<len;i++)
    {
        t->lx[i]=str[i];
    }
    t->lx[i]='\0';
    t->ec=EofInCom;
    t->ln=currlinenumber;
    len=strlen(filename);
    i=0;
    for(;i<len;i++)
    {
        t->fl[i]=filename[i];
    }
    t->fl[i]='\0';
    return;
}
Token GetNextToken ()
{
    Token t;
    bool type1commentend=false;
    bool isstring=false;
    char* stringsaved=malloc(1*sizeof(char));
    stringsaved[0]='\0';
    int sizeofstringsaved=0;
    char ch;
    ch=getc(file);

    while(ch==' '||ch=='\n'||ch=='\r'||ch=='\t')
    {
        if(ch=='\n')
            currlinenumber++;
        ch=getc(file);
    }
    if(ch==EOF||ch=='\xff')// if end of file is encountered
    {
        createtokenforendoffile(&t);
        return t;
    }
    if(ch=='"')//       if string is starting
    {
        int size=0;
        isstring=true;
        ch=getc(file);
        while(ch!='"'&&ch!=EOF&&ch!='\n')
        {
            size++;
            insert(&stringsaved,ch,&sizeofstringsaved,size);
            ch=getc(file);
        }
        if(ch==EOF) // if inbetween eof is encountered
        {
            t.tp=ERR;
            char *str="Error: unexpected eof in string constant";
            unsigned long int len=strlen(str);
            int i=0;
            for(i=0;i<len;i++)
            {
                t.lx[i]=str[i];
            }
            t.lx[i]='\0';
            t.ec=EofInStr;
            t.ln=currlinenumber;
            len=strlen(filename);
            i=0;
            for(;i<len;i++)
            {
                t.fl[i]=filename[i];
            }
            t.fl[i]='\0';
            return t;
        }
        if(ch=='\n')// if next line is encountered
        {
            t.tp=ERR;
            char *str="Error: new line in string constant";
            unsigned long int len=strlen(str);
            int i=0;
            for(i=0;i<len;i++)
            {
                t.lx[i]=str[i];
            }
            t.lx[i]='\0';
            t.ec=NewLnInStr;
            t.ln=currlinenumber;
            len=strlen(filename);
            i=0;
            for(;i<len;i++)
            {
                t.fl[i]=filename[i];
            }
            t.fl[i]='\0';
            return t;
        }
        t.tp=STRING;//   otherwise a valid string is made
        unsigned long int len=strlen(stringsaved);
        int i=0;
        for(i=0;i<len;i++)
        {
            t.lx[i]=stringsaved[i];
        }
        t.lx[i]='\0';
        t.ec=EofInStr;
        t.ln=currlinenumber;
        len=strlen(filename);
        i=0;
        for(;i<len;i++)
        {
            t.fl[i]=filename[i];
        }
        t.fl[i]='\0';
        sizeofstringsaved=0;
        return t;
    }
    else if(ch=='/')// if comment is encountered
    {
        ch=getc(file);
        if(ch=='*')  // if comment is of type one ie /*
        {
            bool firstchar=false;
            ch=getc(file);
            while(ch!=EOF)
            {
                if(ch=='\n')
                    currlinenumber++;
                if(firstchar&&ch=='/')
                {
                    type1commentend=true;
                    break;
                }
                if(ch=='*')
                {
                    firstchar=true;
                }
                else firstchar=false;
                ch=getc(file);
            }
            if(type1commentend)
            {
                return GetNextToken();
            }
            else if(ch==EOF)// in comment new line of file is encountered
            {
                t.tp=ERR;
                char *str="Error: unexpected eof in comment";
                unsigned long int len=strlen(str);
                int i=0;
                for(i=0;i<len;i++)
                {
                    t.lx[i]=str[i];
                }
                t.lx[i]='\0';
                t.ec=EofInCom;
                t.ln=currlinenumber;
                len=strlen(filename);
                i=0;
                for(;i<len;i++)
                {
                    t.fl[i]=filename[i];
                }
                t.fl[i]='\0';
                return t;
            }

        }
        else if(ch=='/') //if comment is of type 2 ie //
        {
            ch=getc(file);
            while(ch!=EOF&&ch!='\n')
            {
                ch=getc(file);
            }
            if(ch=='\n')
                currlinenumber++;
            return GetNextToken();
        }
        else if(checktoken(ch)||isvalidIDcharacter(ch))
        {
            t.tp=SYMBOL;
            t.lx[0]='/';
            t.lx[1]='\0';
            t.ec=IllSym;
            t.ln=currlinenumber;
            int len=strlen(filename);
            int i=0;
            for(;i<len;i++)
            {
                t.fl[i]=filename[i];
            }
            t.fl[i]='\0';
            fseek(file,-1,SEEK_CUR);
            return t;
        }
        else{
            t.tp=ERR;
            char *str="Error: illegal symbol in source file";
            unsigned long int len=strlen(str);
            int i=0;
            for(i=0;i<len;i++)
            {
                t.lx[i]=str[i];
            }
            t.lx[i]='\0';
            t.ec=IllSym;
            t.ln=currlinenumber;
            len=strlen(filename);
            i=0;
            for(;i<len;i++)
            {
                t.fl[i]=filename[i];
            }
            t.fl[i]='\0';
            return t;
        }
    }
    else if(isasymbol(ch))
    {
        t.tp=SYMBOL;
        t.lx[0]=ch;
        t.lx[1]='\0';
        t.ec=IllSym;
        t.ln=currlinenumber;
        int len=strlen(filename);
        int i=0;
        for(;i<len;i++)
        {
            t.fl[i]=filename[i];
        }
        t.fl[i]='\0';
        return t;
    }
    else
    {
        struct Trie* location=Reservedword;
        int size=0;
        bool isaID=true;
        bool isaINT=true;
        if(!(ch>='0'&&ch<='9'))
            isaINT=false;
        if(isaINT)
            isaID=false;

        while(1)// loop for cheking whether it is a INT RES or ID
        {
            size++;
            insert(&stringsaved,ch,&sizeofstringsaved,size);
            if(location!=NULL)
            {
                location=locationinTrie(ch,location);
            }
            if(isaID)
            {
                isaID=isvalidIDcharacter(ch);
            }
            if(isaINT)
            {
                if(!(ch>='0'&&ch<='9'))
                    isaINT=false;
            }
            if(peakahead())
            {
                break;
            }
            ch=getc(file);
        }
        if(isaINT)// if integer is found
       {
           t.tp=INT;//   otherwise a valid string is made
           unsigned long int len=strlen(stringsaved);
           int i=0;
           for(i=0;i<len;i++)
           {
               t.lx[i]=stringsaved[i];
           }
           t.lx[i]='\0';
           t.ec=EofInStr;
           t.ln=currlinenumber;
           len=strlen(filename);
           i=0;
           for(;i<len;i++)
           {
               t.fl[i]=filename[i];
           }
           t.fl[i]='\0';
           sizeofstringsaved=0;
           return t;
       }
       else if(location!=NULL&&location->isend)//if reserved word is found;
       {
           t.tp=RESWORD;//   otherwise a valid string is made
           unsigned long int len=strlen(stringsaved);
           int i=0;
           for(i=0;i<len;i++)
           {
               t.lx[i]=stringsaved[i];
           }
           t.lx[i]='\0';
           t.ec=EofInStr;
           t.ln=currlinenumber;
           len=strlen(filename);
           i=0;
           for(;i<len;i++)
           {
               t.fl[i]=filename[i];
           }
           t.fl[i]='\0';
           sizeofstringsaved=0;
           return t;
       }
       else if(isaID)//if a Identifier is found
       {
           t.tp=ID;//   otherwise a valid string is made
           unsigned long int len=strlen(stringsaved);
           int i=0;
           for(i=0;i<len;i++)
           {
               t.lx[i]=stringsaved[i];
           }
           t.lx[i]='\0';
           t.ec=EofInStr;
           t.ln=currlinenumber;
           len=strlen(filename);
           i=0;
           for(;i<len;i++)
           {
               t.fl[i]=filename[i];
           }
           t.fl[i]='\0';
           sizeofstringsaved=0;
           return t;
       }
       else{// illegal symbol
           t.tp=ERR;
           char *str="Error: illegal symbol in source file";
           unsigned long int len=strlen(str);
           int i=0;
           for(i=0;i<len;i++)
           {
               t.lx[i]=str[i];
           }
           t.lx[i]='\0';
           t.ec=IllSym;
           t.ln=currlinenumber;
           len=strlen(filename);
           i=0;
           for(;i<len;i++)
           {
               t.fl[i]=filename[i];
           }
           t.fl[i]='\0';
           return t;
       }
    }
    return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
    Token t;
    long unsigned int currentcharacternumber=ftell(file);
    int a=currlinenumber;
    t=GetNextToken();
    currlinenumber=a;
    long unsigned int futurecharacternumber=ftell(file);
    fseek(file,-futurecharacternumber+currentcharacternumber,SEEK_CUR);
    return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
    
    fclose(file);
    clean(Reservedword);
	return 0;
}


