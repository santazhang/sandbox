package dbrocks.ast;

import java.util.ArrayList;

public class StatementGroup extends Node {
	
	ArrayList<Statement> stmtList = new ArrayList<Statement>();
	
	public void add(Statement stmt) {
		stmtList.add(stmt);
	}
	
	public ArrayList<Statement> getStatements() {
		return stmtList;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}
}
