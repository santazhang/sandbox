package dbrocks.frontend;

import java.util.ArrayList;

import dbrocks.util.BiName;
import dbrocks.util.Location;
import dbrocks.value.Value;
import dbrocks.ast.Assignment;
import dbrocks.ast.Statement;
import dbrocks.ast.Expr;

public class SemanticValue {

	public int code;

	public Location loc;
	
	public Value value;
	
	public Statement stmt;
	
	public Expr expr;
	
	public ArrayList<Expr> exprList;
	
	public Assignment assign;
	
	public ArrayList<Assignment> assignList;
	
	public String text;
	
	public BiName biName;
	
	public ArrayList<BiName> biNameList;
	
	public static SemanticValue createKeyword(int code) {
		SemanticValue v = new SemanticValue();
		v.code = code;
		return v;
	}
	
	public static SemanticValue createOperator(int code) {
		SemanticValue v = new SemanticValue();
		v.code = code;
		return v;
	}
	
	public static SemanticValue createConstValue(Value value) {
		SemanticValue v = new SemanticValue();
		v.code = Parser.CONST_VALUE;
		v.value = value;
		return v;
	}

	public static SemanticValue createIdentifier(String name) {
		SemanticValue v = new SemanticValue();
		v.code = Parser.IDENTIFIER;
		v.text = name;
		return v;
	}

}
