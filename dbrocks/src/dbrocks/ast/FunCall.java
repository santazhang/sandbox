package dbrocks.ast;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import dbrocks.value.Value;

public class FunCall extends Expr {

	String funName;
	ArrayList<Expr> args;
	
	public FunCall(String funName, ArrayList<Expr> args) {
		this.funName = funName;
		this.args = args;
	}
	
	@Override
	public Value eval(Map<String, Value> var) {
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public Expr optimize() {
		boolean allArgsConst = true;
		
		// optimize each item in the array
		for (int i = 0; i < args.size(); i++) {
			Expr optimizedExpr = args.get(i).optimize(); 
			args.set(i, optimizedExpr);
			if (optimizedExpr instanceof Variable) {
				allArgsConst = false;
			}
		}
		
		// if all arguments are const, then we could evaluate this function call right now
		if (allArgsConst) {
			return new Const(this.eval(null));
		} else {
			return this;
		}
	}

	@Override
	public Set<String> getVariables() {
		HashSet<String> varSet = new HashSet<String>();
		for (Expr expr : args) {
			varSet.addAll(expr.getVariables());
		}
		return varSet;
	}
	
	@Override
	public void accept(Visitor visitor) {
		visitor.visit(this);
	}

}
