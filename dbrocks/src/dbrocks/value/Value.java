package dbrocks.value;

import dbrocks.exception.ValueOperationException;
import dbrocks.ast.ExprOp;

public abstract class Value {
	
	protected int intValue;
	protected double floatValue;
	protected boolean booleanValue;
	protected byte[] binaryValue;
	protected String stringValue;
	
	protected abstract boolean toBoolean();
	
	abstract public Value op(ExprOp op, Value another) throws ValueOperationException;
	
	abstract public Value op(ExprOp op) throws ValueOperationException;
	
}
