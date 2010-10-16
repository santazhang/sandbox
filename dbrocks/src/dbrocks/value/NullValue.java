package dbrocks.value;

import dbrocks.ast.ExprOp;
import dbrocks.exception.ValueOperationException;

public class NullValue extends Value {
	
	public static NullValue instance = new NullValue();
	
	@Override
	public boolean equals(Object another) {
		if (another instanceof NullValue) {
			return true;
		} else {
			return super.equals(another);
		}
	}
	
	@Override
	public String toString() {
		return "null";
	}

	@Override
	public Value op(ExprOp op, Value another) throws ValueOperationException {
		if (op == ExprOp.LOGIC_AND) {
			return BooleanValue.construct(false);
		} else if (op == ExprOp.LOGIC_OR) {
			return BooleanValue.construct(another.toBoolean());
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}

	@Override
	protected boolean toBoolean() {
		return false;
	}

	@Override
	public Value op(ExprOp op) throws ValueOperationException {
		throw new ValueOperationException("Not valid operation: " + op);
	}

}
