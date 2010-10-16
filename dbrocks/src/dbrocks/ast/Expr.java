package dbrocks.ast;

import java.util.Map;
import java.util.Set;

import dbrocks.value.Value;

public abstract class Expr extends Node {
	
	public abstract Value eval(Map<String, Value> var);
	
	public abstract Expr optimize();
	
	public abstract Set<String> getVariables();

}
