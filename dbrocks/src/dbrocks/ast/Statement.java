package dbrocks.ast;

/**
 * Root node of dbrocks SQL queries and updates
 * @author Santa
 *
 */
public abstract class Statement extends Node {

	@Override
	public abstract void accept(Visitor visitor);
	
}
