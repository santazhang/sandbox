package dbrocks.frontend;

import dbrocks.ast.StatementGroup;
import dbrocks.frontend.Lexer;
import dbrocks.frontend.Parser;

public abstract class ParserBase {
	
	private Lexer lexer;

	protected StatementGroup stmtGrp;

	public void setLexer(Lexer lexer) {
		this.lexer = lexer;
	}

	protected void issueError(Exception error) {
//		Compiler.getCompiler().issueError(error);
		error.printStackTrace();
	}

	void yyerror(String msg) {
//		Compiler.getCompiler().issueError(
//				new MsgError(lexer.getLocation(), msg));
		System.err.println(msg + "at" + lexer.getLocation());
	}

	int yylex() {
		int token = -1;
		try {
			token = lexer.yylex();
		} catch (Exception e) {
			e.printStackTrace();
			yyerror("lexer error: " + e.getMessage());
		}
		return token;
	}

	abstract int yyparse();

	public StatementGroup parseInput() {
		yyparse();
		return stmtGrp;
	}


	public static String opStr(int opCode) {
		switch (opCode) {
		case Parser.LOGIC_AND:
			return "and";
		case Parser.LOGIC_OR:
			return "or";
		case Parser.GREATER_EQUAL:
			return ">=";
		case Parser.LESS_EQUAL:
			return "<=";
		case Parser.NOT_EQUAL:
			return "!=";
		default:
			return "" + (char) opCode;
		}
	}
}
