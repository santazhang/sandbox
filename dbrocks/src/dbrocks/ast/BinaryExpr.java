package dbrocks.ast;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import dbrocks.value.Value;

public class BinaryExpr extends Expr {
	
	Expr left;
	Expr right;
	ExprOp op;
	
	public BinaryExpr(Expr left, ExprOp op, Expr right) {
		this.left = left;
		this.op = op;
		this.right = right;
	}
	
	@Override
	public Value eval(Map<String, Value> var) {
		Value leftValue = left.eval(var);
		Value rightValue = right.eval(var);
		return leftValue.op(op, rightValue);
	}
	
	@Override
	public Expr optimize() {
		left = left.optimize();
		right = right.optimize();
		
		// TODO enhance (((x + 1) + 1) + 1), which could be optimized to x + 3
		
		if (left instanceof Const && right instanceof Const) {
			// if both left and right expr are constant, then we could evaluate the expression right now
			Const leftConst = (Const) left;
			Const rightConst = (Const) right;
			Value value = leftConst.getValue().op(op, rightConst.getValue());
			return new Const(value);
		} else {
			return this;
		}
	}

	@Override
	public Set<String> getVariables() {
		HashSet<String> varSet = new HashSet<String>();
		varSet.addAll(left.getVariables());
		varSet.addAll(right.getVariables());
		return varSet;
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
