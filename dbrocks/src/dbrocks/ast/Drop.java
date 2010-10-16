package dbrocks.ast;

public class Drop extends Statement {
	
	String tableName;
	
	public Drop(String tableName) {
		this.tableName = tableName;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
