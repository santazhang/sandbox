package dbrocks.frontend;

import dbrocks.util.Location;

%%
%public
%class Lexer
%extends LexerBase
%byaccj
%line
%column
%switch
%unicode
/*%debug*/

%{
	private Location sloc = null;
	private StringBuilder buffer = new StringBuilder();

	public Location getLocation() {
		return new Location(yyline + 1, yycolumn + 1);
	}
%}

NEWLINE				= (\r|\n|\r\n)
DIGIT 				= ([0-9])
INTEGER				= ({DIGIT}+)
DOUBLE				= ({INTEGER}"."{DIGIT}*([Ee][-+]{INTEGER})?)
IDENTIFIER			= ([A-Za-z][_0-9A-Za-z]*)
OPERATOR			= ("+"|"-"|"*"|"/"|"%"|"="|"<"|">"|"."|","|";"|"!"|"("|")"|"["|"]"|"{"|"}")
S_COMMENT			= ("--"[^\r\n]*{NEWLINE})
WHITESPACE			= ([ \t]+)

%x IN_QUOTE IN_APOS

%%
{WHITESPACE}		{	/* Just ignore */	}
{NEWLINE}			{	/* Just ignore */	}
{S_COMMENT}			{	/* Just ignore */	}

"select"			{	return keyword(Parser.SELECT);		}
"from"				{	return keyword(Parser.FROM);		}
"where"				{	return keyword(Parser.WHERE);		}
"update"			{	return keyword(Parser.UPDATE);		}
"set"				{	return keyword(Parser.SET);			}
"drop"				{	return keyword(Parser.DROP);		}
"delete"			{	return keyword(Parser.DELETE);		}
"table"				{	return keyword(Parser.TABLE);		}

"as"				{	return keyword(Parser.AS);			}

"<="				{	return operator(Parser.LESS_EQUAL);		}
">="				{	return operator(Parser.GREATER_EQUAL);	}
"!="				{	return operator(Parser.NOT_EQUAL);		}
"and"				{	return operator(Parser.LOGIC_AND);		}
"or"				{	return operator(Parser.LOGIC_OR);		}
{OPERATOR}			{	return operator((int) yycharat(0));		}

"true"				{	return booleanConst(true);		}
"false"				{	return booleanConst(false);		}
{INTEGER}			{	return intConst(yytext());		}
{DOUBLE}			{	return doubleConst(yytext()); 	}

<YYINITIAL>\"		{
						sloc = getLocation();		  
						yybegin(IN_QUOTE);
						buffer = new StringBuilder();
					}
					
<IN_QUOTE>{NEWLINE}		{ /*issueError(new NewlineInStrError(sloc, MiscUtils.quote(buffer.toString())))*/	}
<IN_QUOTE><<EOF>>		{
							/*issueError(new UntermStrError(sloc, MiscUtils.quote(buffer.toString())));*/
					  		yybegin(YYINITIAL);
					  	}
<IN_QUOTE>\"			{
							yybegin(YYINITIAL);
					  		return stringConst(buffer.toString(), sloc);
						}
<IN_QUOTE>"\\n"			{	buffer.append('\n');		}
<IN_QUOTE>"\\r"			{	buffer.append('\r');		}
<IN_QUOTE>"\\t"			{	buffer.append('\t'); 		}
<IN_QUOTE>"\\\""		{	buffer.append('"');			}
<IN_QUOTE>"\\\\"		{	buffer.append('\\');		}
<IN_QUOTE>.				{	buffer.append(yytext());	}

{IDENTIFIER}			{	return identifier(yytext());	}


.					{ /* issueError(new UnrecogCharError(getLocation(), yycharat(0)));*/ 		}
