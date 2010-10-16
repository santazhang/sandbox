package dbrocks.value;

import dbrocks.ast.ExprOp;
import dbrocks.exception.ValueOperationException;

public class StringValue extends Value {
	
	public StringValue(String value) {
		stringValue = value;
	}
	
	@Override
	public boolean equals(Object another) {
		if (another instanceof StringValue) {
			return this.stringValue.equals(((StringValue) another).stringValue);
		} else {
			return super.equals(another);
		}
	}
	
	@Override
	public String toString() {
		return stringValue;
	}

	@Override
	public Value op(ExprOp op, Value another) throws ValueOperationException {
		if (op == ExprOp.STR_CONCAT) {
			return new StringValue(this.stringValue + another.toString());
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}

	@Override
	protected boolean toBoolean() {
		throw new ValueOperationException("String value cannot be converted to boolean value");
	}

	@Override
	public Value op(ExprOp op) throws ValueOperationException {
		throw new ValueOperationException("Not valid operation: " + op);
	}

}
