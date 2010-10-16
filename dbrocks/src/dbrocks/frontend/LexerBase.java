package dbrocks.frontend;

import java.io.IOException;

import dbrocks.util.Location;
import dbrocks.value.BooleanValue;
import dbrocks.value.StringValue;
import dbrocks.value.FloatValue;
import dbrocks.value.IntValue;

public abstract class LexerBase {

	private Parser parser;

	public void setParser(Parser parser) {
		this.parser = parser;
	}

	abstract int yylex() throws IOException;

	
	abstract Location getLocation();

	
	protected void issueError(Exception error) {
		
	}

	protected void setSemantic(Location where, SemanticValue v) {
		v.loc = where;
		parser.yylval = v;
	}

	protected int keyword(int code) {
		setSemantic(getLocation(), SemanticValue.createKeyword(code));
		return code;
	}

	protected int operator(int code) {
		setSemantic(getLocation(), SemanticValue.createOperator(code));
		return code;
	}

	protected int booleanConst(boolean bval) {
		setSemantic(getLocation(), SemanticValue.createConstValue(new BooleanValue(bval)));
		return Parser.CONST_VALUE;
	}

	protected int stringConst(String sval, Location loc) {
		setSemantic(getLocation(), SemanticValue.createConstValue(new StringValue(sval)));
		return Parser.CONST_VALUE;
	}

	protected int intConst(String ival) {
		setSemantic(getLocation(), SemanticValue.createConstValue(new IntValue(Integer.parseInt(ival))));
		return Parser.CONST_VALUE;
	}

	protected int doubleConst(String dval) {
		setSemantic(getLocation(), SemanticValue.createConstValue(new FloatValue(Double.parseDouble(dval))));
		return Parser.CONST_VALUE;
	}

	protected int identifier(String name) {
		setSemantic(getLocation(), SemanticValue.createIdentifier(name));
		return Parser.IDENTIFIER;
	}

	public void diagnose() throws IOException {
		while (yylex() != 0) {
			System.out.println(parser.yylval);
		}
	}
}

