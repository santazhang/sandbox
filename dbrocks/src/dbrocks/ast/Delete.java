package dbrocks.ast;

public class Delete extends Statement {
	
	String tableName;
	Expr criterion;
	
	public Delete(String tableName, Expr criterion) {
		this.tableName = tableName;
		this.criterion = criterion;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
