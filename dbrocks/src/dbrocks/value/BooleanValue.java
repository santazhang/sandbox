package dbrocks.value;

import dbrocks.ast.ExprOp;
import dbrocks.exception.ValueOperationException;

public class BooleanValue extends Value {
	
	public static final BooleanValue trueInstance = new BooleanValue(true);
	
	public static final BooleanValue falseInstance = new BooleanValue(false);
	
	public BooleanValue(boolean value) {
		booleanValue = value;
	}

	@Override
	public String toString() {
		return booleanValue ? "true" : "false";
	}
	
	@Override
	public boolean equals(Object another) {
		if (another instanceof BooleanValue) {
			return this.booleanValue == ((BooleanValue) another).booleanValue;
		} else {
			return super.equals(another);
		}
	}
	
	public static BooleanValue construct(boolean b) {
		return b ? trueInstance : falseInstance;
	}

	@Override
	public Value op(ExprOp op, Value another) throws ValueOperationException {
		if (op == ExprOp.LOGIC_AND) {
			return BooleanValue.construct(this.booleanValue && another.toBoolean());
		} else if (op == ExprOp.LOGIC_OR) {
			return BooleanValue.construct(this.booleanValue || another.toBoolean());
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}

	@Override
	protected boolean toBoolean() {
		return booleanValue;
	}


	@Override
	public Value op(ExprOp op) throws ValueOperationException {
		if (op == ExprOp.NOT) {
			return new BooleanValue(!this.booleanValue);
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}
}
