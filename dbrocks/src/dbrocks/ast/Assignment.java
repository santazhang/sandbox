package dbrocks.ast;

public class Assignment extends Node {
	
	String name;
	Expr expr;
	
	public Assignment(String name, Expr expr) {
		this.name = name;
		this.expr = expr;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
