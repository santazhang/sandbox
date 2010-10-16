package dbrocks.ast;

public class Create extends Statement {

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
