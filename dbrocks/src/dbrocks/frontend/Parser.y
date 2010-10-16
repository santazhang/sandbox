%{

package dbrocks.frontend;

import java.util.ArrayList;
import dbrocks.ast.*;
import dbrocks.util.BiName;

%}

%Jclass Parser
%Jextends ParserBase
%Jsemantic SemanticValue
%Jnorun
%Jnodebug
%Jnoconstruct

%token SELECT FROM WHERE UPDATE SET CREATE TABLE COLUMN AS DROP DELETE
%token INT DOUBLE CHAR VARCHAR BOOLEAN
%token LOGIC_AND LOGIC_OR STR_CONCAT NOT
%token IDENTIFIER
%token CONST_VALUE
%token LESS_EQUAL GREATER_EQUAL NOT_EQUAL
%token '+'  '-'  '*'  '/'  '%'  '='  '>'  '<'  ','  ';'  '('  ')'


%left '='
%left LOGIC_OR
%left LOGIC_AND
%nonassoc EQUAL NOT_EQUAL LESS_EQUAL GREATER_EQUAL '<' '>'
%left  STR_CONCAT
%left  '+' '-'
%left  '*' '/' '%'
%nonassoc UMINUS NOT
%nonassoc ')'


%start StatementGroup
 
%%

StatementGroup	:	StatementGroup Statement
					{
						stmtGrp.add($2.stmt);
					}
				|
					{
						stmtGrp = new StatementGroup();
					}
				;

Statement		:	SelectStatement
					{
						$$.stmt = $1.stmt;
					}
				|	UpdateStatement
					{
						$$.stmt = $1.stmt;
					}
				|	CreateStatement
					{
						$$.stmt = $1.stmt;
					}
				|	DropStatement
					{
						$$.stmt = $1.stmt;
					}
				|	DeleteStatement
					{
						$$.stmt = $1.stmt;
					}
				|	Statement ';'
					{
						$$.stmt = $1.stmt;
					}
				;

SelectStatement	:	SELECT SelectList FROM RenamableIdentifierList WhereOpt
					{
						Select selectStmt = new Select();
						selectStmt.setCriterion($5.expr);
						if ($2.biNameList != null) {
							selectStmt.setSelectItems($2.biNameList);
						} else {
							selectStmt.setSelectAll(true);
						}
						$$.stmt = selectStmt;
					}
				;

SelectList	:	RenamableIdentifierList
				{
					$$.biNameList = $1.biNameList;
				}
			|	'*'
			; 

RenamableIdentifierList	:	RenamableIdentifierList ',' RenamableIdentifier
							{
								$$.biNameList.add($3.biName);
							}
						|	RenamableIdentifier
							{
								$$.biNameList = new ArrayList<BiName>();
								$$.biNameList.add($1.biName);
							}
						;

RenamableIdentifier	:	IDENTIFIER AS IDENTIFIER
						{
							$$.biName = new BiName($1.text, $3.text);
						}
					|	IDENTIFIER
						{
							$$.biName = new BiName($1.text, null);
						}
					;

WhereOpt		:	WHERE Expr
					{
						$$.expr = $2.expr;
					}
				|
					{
						$$.expr = null;
					}
				;

UpdateStatement	:	UPDATE IDENTIFIER SET AssignmentList WhereOpt
				;

Assignment	:	IDENTIFIER '=' Expr
				{
					$$.assign = new Assignment($1.text, $3.expr);
				}
			;

AssignmentList	:	AssignmentList ',' Assignment
					{
						$$.assignList = $1.assignList;
						$$.assignList.add($3.assign);
					}
				|	Assignment
					{
						$$.assignList = new ArrayList<Assignment>();
						$$.assignList.add($1.assign);
					}
				;

CreateStatement	:	CREATE TABLE IDENTIFIER
				;

DropStatement	:	DROP TABLE IDENTIFIER
				;

DeleteStatement	:	DELETE FROM TABLE IDENTIFIER WHERE Expr
				|	DELETE '*' FROM TABLE IDENTIFIER
				;

Expr	:	IDENTIFIER
			{
				$$.expr = new Variable($1.text);
			}
		|	CONST_VALUE
			{
				$$.expr = new Const($1.value);
			}
		|	'(' Expr ')'
			{
				$$.expr = $2.expr;
			}
		|	Expr '+' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.ADD, $3.expr);
			}
		|	Expr '-' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.SUB, $3.expr);
			}
		|	Expr '*' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.MUL, $3.expr);
			}
		|	Expr '/' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.DIV, $3.expr);
			}
		|	Expr '%' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.MOD, $3.expr);
			}
		|	Expr STR_CONCAT Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.STR_CONCAT, $3.expr);
			}
		|	Expr LOGIC_AND Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.LOGIC_AND, $3.expr);
			}
		|	Expr LOGIC_OR Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.LOGIC_OR, $3.expr);
			}
		|	Expr '<' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.LESS, $3.expr);
			}
		|	Expr '>' Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.GREAT, $3.expr);
			}
		|	Expr '=' %prec EQUAL Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.EQUAL, $3.expr);
			}
		|	Expr NOT_EQUAL Expr
			{
				$$.expr = new BinaryExpr($1.expr, ExprOp.NOT_EQUAL, $3.expr);
			}
		|	'-' %prec UMINUS Expr
			{
				$$.expr = new UnaryExpr(ExprOp.NEG, $2.expr);
			}
		|	NOT Expr
			{
				$$.expr = new UnaryExpr(ExprOp.NOT, $2.expr);
			}
		|	IDENTIFIER '(' ArgList ')'
			{
				$$.expr = new FunCall($1.text, $3.exprList);
			}
		;
		
ArgList	:	ArgList ',' Expr
			{
				$$.exprList = $1.exprList;
				$$.exprList.add($3.expr);
			}
		|	Expr
			{
				$$.exprList = new ArrayList<Expr>();
				$$.exprList.add($1.expr);
			}
		;

%%
