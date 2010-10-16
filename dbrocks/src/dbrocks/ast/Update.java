package dbrocks.ast;

public class Update extends Statement {
	
	Expr criterion;
	
	public void setCriterion(Expr criterion) {
		this.criterion = criterion;
	}
	
	public Expr getCriterion() {
		return criterion;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
