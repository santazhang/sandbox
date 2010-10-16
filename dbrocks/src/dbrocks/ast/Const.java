package dbrocks.ast;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import dbrocks.value.Value;

public class Const extends Expr {
	
	Value value;
	
	public Const(Value value) {
		this.value = value;
	}
	
	public Value getValue() {
		return value;
	}

	@Override
	public Value eval(Map<String, Value> var) {
		return getValue();
	}

	@Override
	public Expr optimize() {
		// cannot optimize
		return this;
	}
	
	@Override
	public Set<String> getVariables() {
		return new HashSet<String>();
	}

	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
