//### This file created by BYACC 1.8(/Java extension  1.15)
//### Java capabilities added 7 Jan 97, Bob Jamison
//### Updated : 27 Nov 97  -- Bob Jamison, Joe Nieten
//###           01 Jan 98  -- Bob Jamison -- fixed generic semantic constructor
//###           01 Jun 99  -- Bob Jamison -- added Runnable support
//###           06 Aug 00  -- Bob Jamison -- made state variables class-global
//###           03 Jan 01  -- Bob Jamison -- improved flags, tracing
//###           16 May 01  -- Bob Jamison -- added custom stack sizing
//###           04 Mar 02  -- Yuval Oren  -- improved java performance, added options
//###           14 Mar 02  -- Tomas Hurka -- -d support, static initializer workaround
//### Please send bug reports to tom@hukatronic.cz
//### static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";






//#line 2 "Parser.y"

package dbrocks.frontend;

import java.util.ArrayList;
import dbrocks.ast.*;
import dbrocks.util.BiName;

//#line 25 "Parser.java"




public class Parser
             extends ParserBase
{

boolean yydebug;        //do I want debug output?
int yynerrs;            //number of errors so far
int yyerrflag;          //was there an error?
int yychar;             //the current working character

//########## MESSAGES ##########
//###############################################################
// method: debug
//###############################################################
void debug(String msg)
{
  if (yydebug)
    System.out.println(msg);
}

//########## STATE STACK ##########
final static int YYSTACKSIZE = 500;  //maximum stack size
int statestk[] = new int[YYSTACKSIZE]; //state stack
int stateptr;
int stateptrmax;                     //highest index of stackptr
int statemax;                        //state when highest index reached
//###############################################################
// methods: state stack push,pop,drop,peek
//###############################################################
final void state_push(int state)
{
  try {
		stateptr++;
		statestk[stateptr]=state;
	 }
	 catch (ArrayIndexOutOfBoundsException e) {
     int oldsize = statestk.length;
     int newsize = oldsize * 2;
     int[] newstack = new int[newsize];
     System.arraycopy(statestk,0,newstack,0,oldsize);
     statestk = newstack;
     statestk[stateptr]=state;
  }
}
final int state_pop()
{
  return statestk[stateptr--];
}
final void state_drop(int cnt)
{
  stateptr -= cnt; 
}
final int state_peek(int relative)
{
  return statestk[stateptr-relative];
}
//###############################################################
// method: init_stacks : allocate and prepare stacks
//###############################################################
final boolean init_stacks()
{
  stateptr = -1;
  val_init();
  return true;
}
//###############################################################
// method: dump_stacks : show n levels of the stacks
//###############################################################
void dump_stacks(int count)
{
int i;
  System.out.println("=index==state====value=     s:"+stateptr+"  v:"+valptr);
  for (i=0;i<count;i++)
    System.out.println(" "+i+"    "+statestk[i]+"      "+valstk[i]);
  System.out.println("======================");
}


//########## SEMANTIC VALUES ##########
//## **user defined:SemanticValue
String   yytext;//user variable to return contextual strings
SemanticValue yyval; //used to return semantic vals from action routines
SemanticValue yylval;//the 'lval' (result) I got from yylex()
SemanticValue valstk[] = new SemanticValue[YYSTACKSIZE];
int valptr;
//###############################################################
// methods: value stack push,pop,drop,peek.
//###############################################################
final void val_init()
{
  yyval=new SemanticValue();
  yylval=new SemanticValue();
  valptr=-1;
}
final void val_push(SemanticValue val)
{
  try {
    valptr++;
    valstk[valptr]=val;
  }
  catch (ArrayIndexOutOfBoundsException e) {
    int oldsize = valstk.length;
    int newsize = oldsize*2;
    SemanticValue[] newstack = new SemanticValue[newsize];
    System.arraycopy(valstk,0,newstack,0,oldsize);
    valstk = newstack;
    valstk[valptr]=val;
  }
}
final SemanticValue val_pop()
{
  return valstk[valptr--];
}
final void val_drop(int cnt)
{
  valptr -= cnt;
}
final SemanticValue val_peek(int relative)
{
  return valstk[valptr-relative];
}
final SemanticValue dup_yyval(SemanticValue val)
{
  return val;
}
//#### end semantic value section ####
public final static short SELECT=257;
public final static short FROM=258;
public final static short WHERE=259;
public final static short UPDATE=260;
public final static short SET=261;
public final static short CREATE=262;
public final static short TABLE=263;
public final static short COLUMN=264;
public final static short AS=265;
public final static short DROP=266;
public final static short DELETE=267;
public final static short INT=268;
public final static short DOUBLE=269;
public final static short CHAR=270;
public final static short VARCHAR=271;
public final static short BOOLEAN=272;
public final static short LOGIC_AND=273;
public final static short LOGIC_OR=274;
public final static short STR_CONCAT=275;
public final static short NOT=276;
public final static short IDENTIFIER=277;
public final static short CONST_VALUE=278;
public final static short LESS_EQUAL=279;
public final static short GREATER_EQUAL=280;
public final static short NOT_EQUAL=281;
public final static short EQUAL=282;
public final static short UMINUS=283;
public final static short YYERRCODE=256;
final static short yylhs[] = {                           -1,
    0,    0,    1,    1,    1,    1,    1,    1,    2,    7,
    7,    8,    8,   10,   10,    9,    9,    3,   13,   12,
   12,    4,    5,    6,    6,   11,   11,   11,   11,   11,
   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,
   11,   11,   11,   14,   14,
};
final static short yylen[] = {                            2,
    2,    0,    1,    1,    1,    1,    1,    2,    5,    1,
    1,    3,    1,    3,    1,    2,    0,    5,    3,    3,
    1,    3,    3,    6,    5,    1,    1,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    2,    2,    4,    3,    1,
};
final static short yydefred[] = {                         2,
    0,    0,    0,    0,    0,    0,    0,    3,    4,    5,
    6,    7,    0,   11,    0,    0,   13,    0,    0,    0,
    0,    0,    8,    0,    0,    0,    0,   22,   23,    0,
    0,   14,    0,   12,    0,    0,   21,    0,    0,    0,
    9,    0,    0,   18,    0,   25,    0,    0,   27,    0,
    0,    0,    0,   20,    0,   42,    0,   41,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   28,    0,    0,    0,    0,    0,    0,
   31,   32,   33,    0,    0,    0,    0,   43,    0,
};
final static short yydgoto[] = {                          1,
    7,    8,    9,   10,   11,   12,   15,   16,   41,   17,
   52,   36,   37,   73,
};
final static short yysindex[] = {                         0,
 -241,  -28, -275, -259, -243,  -39,  -32,    0,    0,    0,
    0,    0, -236,    0, -223,  -22,    0, -225, -226, -221,
 -222, -206,    0, -219, -213, -213, -211,    0,    0, -204,
 -188,    0,  -35,    0,   15,  -26,    0, -180, -196,   -6,
    0,   -6, -211,    0,   -6,    0,   -6,   49,    0,   -6,
   -6,  102,  102,    0,  102,    0,   -6,    0,  -30,   -6,
   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,
   -6,  102,   -4,    0,   81,   35,   12,  -37,  -14,  -14,
    0,    0,    0,  -37,  -37,  -37,   -6,    0,  102,
};
final static short yyrindex[] = {                         0,
    0,    0,    0,    0,    0,    0,  253,    0,    0,    0,
    0,    0,  206,    0,    0, -171,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  226,    0,    0,  226,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    1,    0,    0,
    0,  234,  218,    0,  242,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    9,    0,    0,   93,  195,   70,  112,   24,   47,
    0,    0,    0,  133,  155,  176,    0,    0,   30,
};
final static short yygindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,   69,   60,   72,
  476,    0,   56,    0,
};
final static int YYTABLESIZE=563;
static short yytable[];
static { yytable();}
static void yytable(){
yytable = new short[]{                         68,
   26,   18,   22,   19,   66,   64,   68,   65,   26,   67,
   74,   66,   64,   14,   65,    2,   67,   43,    3,   20,
    4,   26,   68,   29,    5,    6,   23,   66,   24,   71,
   69,   70,   67,   51,   25,   27,   88,   26,   50,   87,
   30,   26,   26,   26,   26,   26,   30,   26,   68,   45,
   28,   31,   45,   66,   64,   29,   65,   32,   67,   26,
   26,   26,   26,   13,   29,   35,   29,   29,   29,   34,
   44,   68,   38,   44,   39,   42,   66,   64,   45,   65,
   46,   67,   29,   29,   29,   29,   10,   30,   57,   30,
   30,   30,   35,   33,   71,   44,   70,   34,   54,    0,
    0,    0,    0,    0,    0,   30,   30,   30,   30,    0,
   34,   40,    0,   34,    0,    0,    0,   68,    0,    0,
    0,    0,   66,   64,    0,   65,    0,   67,   34,   34,
   34,   34,   39,   35,    0,    0,   35,    0,   68,    0,
   71,    0,   70,   66,   64,    0,   65,    0,   67,    0,
    0,   35,   40,   35,   38,   40,    0,    0,    0,    0,
    0,   71,   69,   70,    0,    0,    0,    0,    0,    0,
   40,    0,   40,   39,    0,   37,   39,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   39,    0,   39,   36,   38,    0,    0,   38,    0,
    0,    0,    0,    0,    0,   15,    0,    0,    0,    0,
    0,    0,    0,   38,    0,   38,   37,   19,   21,   37,
    0,    0,    0,   40,    0,   17,    0,    0,    0,    0,
    0,    0,   40,   16,   37,   36,   37,   62,   36,    0,
    0,   24,   60,   61,   62,    0,    0,    0,   13,   15,
   63,    0,    1,   36,    0,   36,    0,   26,    0,   26,
   26,   19,   26,    0,   15,    0,   26,   26,    0,   47,
   48,   49,    0,   26,   26,   26,   19,    0,    0,    0,
   29,   26,   29,   29,   17,   29,    0,    0,    0,   29,
   29,    0,   16,    0,    0,    0,   29,   29,   29,    0,
   24,    0,    0,   30,   29,   30,   30,   60,   30,   62,
    0,    0,   30,   30,    0,   63,    0,    0,    0,   30,
   30,   30,    0,    0,    0,    0,   34,   30,   34,   34,
    0,   34,    0,    0,    0,   34,   34,    0,    0,    0,
    0,    0,   34,   34,   34,    0,    0,    0,    0,   35,
   34,   35,   35,    0,   35,   62,    0,    0,   35,   35,
    0,   63,    0,    0,    0,   35,   35,    0,   40,    0,
   40,   40,    0,   40,   60,   61,   62,   40,   40,    0,
    0,    0,   63,    0,   40,   40,    0,    0,    0,   39,
    0,   39,   39,    0,   39,    0,    0,    0,   39,   39,
    0,    0,    0,    0,    0,   39,   39,    0,    0,    0,
    0,   38,    0,   38,   38,    0,   38,    0,    0,    0,
   38,   38,    0,    0,    0,    0,    0,   38,   38,    0,
    0,    0,   37,    0,   37,   37,    0,   37,    0,    0,
    0,   37,   37,    0,    0,    0,    0,    0,   37,   37,
    0,   36,    0,   36,   36,    0,   36,    0,    0,    0,
   36,   36,   15,   15,   15,   15,    0,   15,   36,    0,
    0,   15,   15,    0,   19,    0,   19,   19,    0,   19,
    0,    0,   17,   19,   19,   17,    0,   17,    0,    0,
   16,   17,   17,   16,    0,   16,    0,    0,   24,   16,
   16,   24,    0,   24,    0,    0,    0,   24,   24,    1,
    0,    0,    1,    0,    1,    0,    0,   53,    1,    1,
   55,    0,   56,    0,    0,   58,   59,    0,    0,    0,
    0,    0,   72,    0,    0,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   89,
};
}
static short yycheck[];
static { yycheck(); }
static void yycheck() {
yycheck = new short[] {                         37,
    0,  277,   42,  263,   42,   43,   37,   45,   44,   47,
   41,   42,   43,   42,   45,  257,   47,   44,  260,  263,
  262,   44,   37,    0,  266,  267,   59,   42,  265,   60,
   61,   62,   47,   40,  258,  261,   41,   37,   45,   44,
  263,   41,   42,   43,   44,   45,    0,   47,   37,   41,
  277,  258,   44,   42,   43,  277,   45,  277,   47,   59,
   60,   61,   62,  277,   41,  277,   43,   44,   45,    0,
   41,   37,  277,   44,  263,   61,   42,   43,  259,   45,
  277,   47,   59,   60,   61,   62,  258,   41,   40,   43,
   44,   45,    0,   25,   60,   36,   62,   26,   43,   -1,
   -1,   -1,   -1,   -1,   -1,   59,   60,   61,   62,   -1,
   41,    0,   -1,   44,   -1,   -1,   -1,   37,   -1,   -1,
   -1,   -1,   42,   43,   -1,   45,   -1,   47,   59,   60,
   61,   62,    0,   41,   -1,   -1,   44,   -1,   37,   -1,
   60,   -1,   62,   42,   43,   -1,   45,   -1,   47,   -1,
   -1,   59,   41,   61,    0,   44,   -1,   -1,   -1,   -1,
   -1,   60,   61,   62,   -1,   -1,   -1,   -1,   -1,   -1,
   59,   -1,   61,   41,   -1,    0,   44,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   59,   -1,   61,    0,   41,   -1,   -1,   44,   -1,
   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   59,   -1,   61,   41,    0,  258,   44,
   -1,   -1,   -1,  259,   -1,    0,   -1,   -1,   -1,   -1,
   -1,   -1,  259,    0,   59,   41,   61,  275,   44,   -1,
   -1,    0,  273,  274,  275,   -1,   -1,   -1,  277,   44,
  281,   -1,    0,   59,   -1,   61,   -1,  257,   -1,  259,
  260,   44,  262,   -1,   59,   -1,  266,  267,   -1,  276,
  277,  278,   -1,  273,  274,  275,   59,   -1,   -1,   -1,
  257,  281,  259,  260,   59,  262,   -1,   -1,   -1,  266,
  267,   -1,   59,   -1,   -1,   -1,  273,  274,  275,   -1,
   59,   -1,   -1,  257,  281,  259,  260,  273,  262,  275,
   -1,   -1,  266,  267,   -1,  281,   -1,   -1,   -1,  273,
  274,  275,   -1,   -1,   -1,   -1,  257,  281,  259,  260,
   -1,  262,   -1,   -1,   -1,  266,  267,   -1,   -1,   -1,
   -1,   -1,  273,  274,  275,   -1,   -1,   -1,   -1,  257,
  281,  259,  260,   -1,  262,  275,   -1,   -1,  266,  267,
   -1,  281,   -1,   -1,   -1,  273,  274,   -1,  257,   -1,
  259,  260,   -1,  262,  273,  274,  275,  266,  267,   -1,
   -1,   -1,  281,   -1,  273,  274,   -1,   -1,   -1,  257,
   -1,  259,  260,   -1,  262,   -1,   -1,   -1,  266,  267,
   -1,   -1,   -1,   -1,   -1,  273,  274,   -1,   -1,   -1,
   -1,  257,   -1,  259,  260,   -1,  262,   -1,   -1,   -1,
  266,  267,   -1,   -1,   -1,   -1,   -1,  273,  274,   -1,
   -1,   -1,  257,   -1,  259,  260,   -1,  262,   -1,   -1,
   -1,  266,  267,   -1,   -1,   -1,   -1,   -1,  273,  274,
   -1,  257,   -1,  259,  260,   -1,  262,   -1,   -1,   -1,
  266,  267,  257,  258,  259,  260,   -1,  262,  274,   -1,
   -1,  266,  267,   -1,  257,   -1,  259,  260,   -1,  262,
   -1,   -1,  257,  266,  267,  260,   -1,  262,   -1,   -1,
  257,  266,  267,  260,   -1,  262,   -1,   -1,  257,  266,
  267,  260,   -1,  262,   -1,   -1,   -1,  266,  267,  257,
   -1,   -1,  260,   -1,  262,   -1,   -1,   42,  266,  267,
   45,   -1,   47,   -1,   -1,   50,   51,   -1,   -1,   -1,
   -1,   -1,   57,   -1,   -1,   60,   61,   62,   63,   64,
   65,   66,   67,   68,   69,   70,   71,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   87,
};
}
final static short YYFINAL=1;
final static short YYMAXTOKEN=283;
final static String yyname[] = {
"end-of-file",null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,"'%'",null,null,"'('","')'","'*'","'+'",
"','","'-'",null,"'/'",null,null,null,null,null,null,null,null,null,null,null,
"';'","'<'","'='","'>'",null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,"SELECT","FROM","WHERE","UPDATE","SET",
"CREATE","TABLE","COLUMN","AS","DROP","DELETE","INT","DOUBLE","CHAR","VARCHAR",
"BOOLEAN","LOGIC_AND","LOGIC_OR","STR_CONCAT","NOT","IDENTIFIER","CONST_VALUE",
"LESS_EQUAL","GREATER_EQUAL","NOT_EQUAL","EQUAL","UMINUS",
};
final static String yyrule[] = {
"$accept : StatementGroup",
"StatementGroup : StatementGroup Statement",
"StatementGroup :",
"Statement : SelectStatement",
"Statement : UpdateStatement",
"Statement : CreateStatement",
"Statement : DropStatement",
"Statement : DeleteStatement",
"Statement : Statement ';'",
"SelectStatement : SELECT SelectList FROM RenamableIdentifierList WhereOpt",
"SelectList : RenamableIdentifierList",
"SelectList : '*'",
"RenamableIdentifierList : RenamableIdentifierList ',' RenamableIdentifier",
"RenamableIdentifierList : RenamableIdentifier",
"RenamableIdentifier : IDENTIFIER AS IDENTIFIER",
"RenamableIdentifier : IDENTIFIER",
"WhereOpt : WHERE Expr",
"WhereOpt :",
"UpdateStatement : UPDATE IDENTIFIER SET AssignmentList WhereOpt",
"Assignment : IDENTIFIER '=' Expr",
"AssignmentList : AssignmentList ',' Assignment",
"AssignmentList : Assignment",
"CreateStatement : CREATE TABLE IDENTIFIER",
"DropStatement : DROP TABLE IDENTIFIER",
"DeleteStatement : DELETE FROM TABLE IDENTIFIER WHERE Expr",
"DeleteStatement : DELETE '*' FROM TABLE IDENTIFIER",
"Expr : IDENTIFIER",
"Expr : CONST_VALUE",
"Expr : '(' Expr ')'",
"Expr : Expr '+' Expr",
"Expr : Expr '-' Expr",
"Expr : Expr '*' Expr",
"Expr : Expr '/' Expr",
"Expr : Expr '%' Expr",
"Expr : Expr STR_CONCAT Expr",
"Expr : Expr LOGIC_AND Expr",
"Expr : Expr LOGIC_OR Expr",
"Expr : Expr '<' Expr",
"Expr : Expr '>' Expr",
"Expr : Expr '=' Expr",
"Expr : Expr NOT_EQUAL Expr",
"Expr : '-' Expr",
"Expr : NOT Expr",
"Expr : IDENTIFIER '(' ArgList ')'",
"ArgList : ArgList ',' Expr",
"ArgList : Expr",
};

//###############################################################
// method: yylexdebug : check lexer state
//###############################################################
void yylexdebug(int state,int ch)
{
String s=null;
  if (ch < 0) ch=0;
  if (ch <= YYMAXTOKEN) //check index bounds
     s = yyname[ch];    //now get it
  if (s==null)
    s = "illegal-symbol";
  debug("state "+state+", reading "+ch+" ("+s+")");
}





//The following are now global, to aid in error reporting
int yyn;       //next next thing to do
int yym;       //
int yystate;   //current parsing state from state table
String yys;    //current token string


//###############################################################
// method: yyparse : parse input and execute indicated items
//###############################################################
int yyparse()
{
boolean doaction;
  init_stacks();
  yynerrs = 0;
  yyerrflag = 0;
  yychar = -1;          //impossible char forces a read
  yystate=0;            //initial state
  state_push(yystate);  //save it
  val_push(yylval);     //save empty value
  while (true) //until parsing is done, either correctly, or w/error
    {
    doaction=true;
    //if (yydebug) debug("loop"); 
    //#### NEXT ACTION (from reduction table)
    for (yyn=yydefred[yystate];yyn==0;yyn=yydefred[yystate])
      {
      //if (yydebug) debug("yyn:"+yyn+"  state:"+yystate+"  yychar:"+yychar);
      if (yychar < 0)      //we want a char?
        {
        yychar = yylex();  //get next token
        //if (yydebug) debug(" next yychar:"+yychar);
        //#### ERROR CHECK ####
        if (yychar < 0)    //it it didn't work/error
          {
          yychar = 0;      //change it to default string (no -1!)
          //if (yydebug)
          //  yylexdebug(yystate,yychar);
          }
        }//yychar<0
      yyn = yysindex[yystate];  //get amount to shift by (shift index)
      if ((yyn != 0) && (yyn += yychar) >= 0 &&
          yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
        {
        //if (yydebug)
          //debug("state "+yystate+", shifting to state "+yytable[yyn]);
        //#### NEXT STATE ####
        yystate = yytable[yyn];//we are in a new state
        state_push(yystate);   //save it
        val_push(yylval);      //push our lval as the input for next rule
        yychar = -1;           //since we have 'eaten' a token, say we need another
        if (yyerrflag > 0)     //have we recovered an error?
           --yyerrflag;        //give ourselves credit
        doaction=false;        //but don't process yet
        break;   //quit the yyn=0 loop
        }

    yyn = yyrindex[yystate];  //reduce
    if ((yyn !=0 ) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
      {   //we reduced!
      //if (yydebug) debug("reduce");
      yyn = yytable[yyn];
      doaction=true; //get ready to execute
      break;         //drop down to actions
      }
    else //ERROR RECOVERY
      {
      if (yyerrflag==0)
        {
        yyerror("syntax error");
        yynerrs++;
        }
      if (yyerrflag < 3) //low error count?
        {
        yyerrflag = 3;
        while (true)   //do until break
          {
          if (stateptr<0)   //check for under & overflow here
            {
            yyerror("stack underflow. aborting...");  //note lower case 's'
            return 1;
            }
          yyn = yysindex[state_peek(0)];
          if ((yyn != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
            //if (yydebug)
              //debug("state "+state_peek(0)+", error recovery shifting to state "+yytable[yyn]+" ");
            yystate = yytable[yyn];
            state_push(yystate);
            val_push(yylval);
            doaction=false;
            break;
            }
          else
            {
            //if (yydebug)
              //debug("error recovery discarding state "+state_peek(0)+" ");
            if (stateptr<0)   //check for under & overflow here
              {
              yyerror("Stack underflow. aborting...");  //capital 'S'
              return 1;
              }
            state_pop();
            val_pop();
            }
          }
        }
      else            //discard this token
        {
        if (yychar == 0)
          return 1; //yyabort
        //if (yydebug)
          //{
          //yys = null;
          //if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
          //if (yys == null) yys = "illegal-symbol";
          //debug("state "+yystate+", error recovery discards token "+yychar+" ("+yys+")");
          //}
        yychar = -1;  //read another
        }
      }//end error recovery
    }//yyn=0 loop
    if (!doaction)   //any reason not to proceed?
      continue;      //skip action
    yym = yylen[yyn];          //get count of terminals on rhs
    //if (yydebug)
      //debug("state "+yystate+", reducing "+yym+" by rule "+yyn+" ("+yyrule[yyn]+")");
    if (yym>0)                 //if count of rhs not 'nil'
      yyval = val_peek(yym-1); //get current semantic value
    yyval = dup_yyval(yyval); //duplicate yyval if ParserVal is used as semantic value
    switch(yyn)
      {
//########## USER-SUPPLIED ACTIONS ##########
case 1:
//#line 43 "Parser.y"
{
						stmtGrp.add(val_peek(0).stmt);
					}
break;
case 2:
//#line 47 "Parser.y"
{
						stmtGrp = new StatementGroup();
					}
break;
case 3:
//#line 53 "Parser.y"
{
						yyval.stmt = val_peek(0).stmt;
					}
break;
case 4:
//#line 57 "Parser.y"
{
						yyval.stmt = val_peek(0).stmt;
					}
break;
case 5:
//#line 61 "Parser.y"
{
						yyval.stmt = val_peek(0).stmt;
					}
break;
case 6:
//#line 65 "Parser.y"
{
						yyval.stmt = val_peek(0).stmt;
					}
break;
case 7:
//#line 69 "Parser.y"
{
						yyval.stmt = val_peek(0).stmt;
					}
break;
case 8:
//#line 73 "Parser.y"
{
						yyval.stmt = val_peek(1).stmt;
					}
break;
case 9:
//#line 79 "Parser.y"
{
						Select selectStmt = new Select();
						selectStmt.setCriterion(val_peek(0).expr);
						if (val_peek(3).biNameList != null) {
							selectStmt.setSelectItems(val_peek(3).biNameList);
						} else {
							selectStmt.setSelectAll(true);
						}
						yyval.stmt = selectStmt;
					}
break;
case 10:
//#line 92 "Parser.y"
{
					yyval.biNameList = val_peek(0).biNameList;
				}
break;
case 12:
//#line 99 "Parser.y"
{
								yyval.biNameList.add(val_peek(0).biName);
							}
break;
case 13:
//#line 103 "Parser.y"
{
								yyval.biNameList = new ArrayList<BiName>();
								yyval.biNameList.add(val_peek(0).biName);
							}
break;
case 14:
//#line 110 "Parser.y"
{
							yyval.biName = new BiName(val_peek(2).text, val_peek(0).text);
						}
break;
case 15:
//#line 114 "Parser.y"
{
							yyval.biName = new BiName(val_peek(0).text, null);
						}
break;
case 16:
//#line 120 "Parser.y"
{
						yyval.expr = val_peek(0).expr;
					}
break;
case 17:
//#line 124 "Parser.y"
{
						yyval.expr = null;
					}
break;
case 19:
//#line 133 "Parser.y"
{
					yyval.assign = new Assignment(val_peek(2).text, val_peek(0).expr);
				}
break;
case 20:
//#line 139 "Parser.y"
{
						yyval.assignList = val_peek(2).assignList;
						yyval.assignList.add(val_peek(0).assign);
					}
break;
case 21:
//#line 144 "Parser.y"
{
						yyval.assignList = new ArrayList<Assignment>();
						yyval.assignList.add(val_peek(0).assign);
					}
break;
case 26:
//#line 161 "Parser.y"
{
				yyval.expr = new Variable(val_peek(0).text);
			}
break;
case 27:
//#line 165 "Parser.y"
{
				yyval.expr = new Const(val_peek(0).value);
			}
break;
case 28:
//#line 169 "Parser.y"
{
				yyval.expr = val_peek(1).expr;
			}
break;
case 29:
//#line 173 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.ADD, val_peek(0).expr);
			}
break;
case 30:
//#line 177 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.SUB, val_peek(0).expr);
			}
break;
case 31:
//#line 181 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.MUL, val_peek(0).expr);
			}
break;
case 32:
//#line 185 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.DIV, val_peek(0).expr);
			}
break;
case 33:
//#line 189 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.MOD, val_peek(0).expr);
			}
break;
case 34:
//#line 193 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.STR_CONCAT, val_peek(0).expr);
			}
break;
case 35:
//#line 197 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.LOGIC_AND, val_peek(0).expr);
			}
break;
case 36:
//#line 201 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.LOGIC_OR, val_peek(0).expr);
			}
break;
case 37:
//#line 205 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.LESS, val_peek(0).expr);
			}
break;
case 38:
//#line 209 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.GREAT, val_peek(0).expr);
			}
break;
case 39:
//#line 213 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.EQUAL, val_peek(0).expr);
			}
break;
case 40:
//#line 217 "Parser.y"
{
				yyval.expr = new BinaryExpr(val_peek(2).expr, ExprOp.NOT_EQUAL, val_peek(0).expr);
			}
break;
case 41:
//#line 221 "Parser.y"
{
				yyval.expr = new UnaryExpr(ExprOp.NEG, val_peek(0).expr);
			}
break;
case 42:
//#line 225 "Parser.y"
{
				yyval.expr = new UnaryExpr(ExprOp.NOT, val_peek(0).expr);
			}
break;
case 43:
//#line 229 "Parser.y"
{
				yyval.expr = new FunCall(val_peek(3).text, val_peek(1).exprList);
			}
break;
case 44:
//#line 235 "Parser.y"
{
				yyval.exprList = val_peek(2).exprList;
				yyval.exprList.add(val_peek(0).expr);
			}
break;
case 45:
//#line 240 "Parser.y"
{
				yyval.exprList = new ArrayList<Expr>();
				yyval.exprList.add(val_peek(0).expr);
			}
break;
//#line 771 "Parser.java"
//########## END OF USER-SUPPLIED ACTIONS ##########
    }//switch
    //#### Now let's reduce... ####
    //if (yydebug) debug("reduce");
    state_drop(yym);             //we just reduced yylen states
    yystate = state_peek(0);     //get new state
    val_drop(yym);               //corresponding value drop
    yym = yylhs[yyn];            //select next TERMINAL(on lhs)
    if (yystate == 0 && yym == 0)//done? 'rest' state and at first TERMINAL
      {
      //if (yydebug) debug("After reduction, shifting from state 0 to state "+YYFINAL+"");
      yystate = YYFINAL;         //explicitly say we're done
      state_push(YYFINAL);       //and save it
      val_push(yyval);           //also save the semantic value of parsing
      if (yychar < 0)            //we want another character?
        {
        yychar = yylex();        //get next character
        if (yychar<0) yychar=0;  //clean, if necessary
        //if (yydebug)
          //yylexdebug(yystate,yychar);
        }
      if (yychar == 0)          //Good exit (if lex returns 0 ;-)
         break;                 //quit the loop--all DONE
      }//if yystate
    else                        //else not done yet
      {                         //get next state and push, for next yydefred[]
      yyn = yygindex[yym];      //find out where to go
      if ((yyn != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn]; //get new state
      else
        yystate = yydgoto[yym]; //else go to new defred
      //if (yydebug) debug("after reduction, shifting from state "+state_peek(0)+" to state "+yystate+"");
      state_push(yystate);     //going again, so push state & val...
      val_push(yyval);         //for next action
      }
    }//main loop
  return 0;//yyaccept!!
}
//## end of method parse() ######################################



//## run() --- for Thread #######################################
//## The -Jnorun option was used ##
//## end of method run() ########################################



//## Constructors ###############################################
//## The -Jnoconstruct option was used ##
//###############################################################



}
//################### END OF CLASS ##############################
