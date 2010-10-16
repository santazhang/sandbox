package dbrocks.ast;

import java.util.Map;
import java.util.Set;

import dbrocks.value.Value;

public class UnaryExpr extends Expr {
	
	Expr expr;
	ExprOp op;
	
	public UnaryExpr(ExprOp op, Expr expr) {
		this.op = op;
		this.expr = expr;
	}

	@Override
	public Value eval(Map<String, Value> var) {
		return expr.eval(var);
	}

	@Override
	public Set<String> getVariables() {
		return expr.getVariables();
	}

	@Override
	public Expr optimize() {
		// TODO Auto-generated method stub
		expr = expr.optimize();
		if (expr instanceof Const) {
			
		}
		return null;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
