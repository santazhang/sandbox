package dbrocks.ast;

public interface Visitor {
	
	public void visit(Assignment assign);
	
	public void visit(BinaryExpr binaryExpr);
	
	public void visit(Const constValue);
	
	public void visit(Create createStmt);
	
	public void visit(Delete deleteStmt);
	
	public void visit(Drop dropStmt);
	
	public void visit(FunCall funCall);
	
	public void visit(Select selectStmt);
	
	public void visit(StatementGroup stmtGrp);
	
	public void visit(UnaryExpr unaryExpr);
	
	public void visit(Update updateStmt);
	
	public void visit(Variable variable);
	
}
