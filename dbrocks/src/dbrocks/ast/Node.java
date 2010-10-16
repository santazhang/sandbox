package dbrocks.ast;

public abstract class Node {
	
	public abstract void accept(Visitor visitor);

}
