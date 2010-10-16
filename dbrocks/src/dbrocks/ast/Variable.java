package dbrocks.ast;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import dbrocks.value.Value;

public class Variable extends Expr {
	
	String name;
	
	public Variable(String name) {
		this.name = name;
	}

	@Override
	public Value eval(Map<String, Value> var) {
		return var.get(name);
	}

	@Override
	public Expr optimize() {
		// cannot optimize
		return this;
	}

	@Override
	public Set<String> getVariables() {
		HashSet<String> varSet = new HashSet<String>();
		varSet.add(name);
		return varSet;
	}
	
	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
