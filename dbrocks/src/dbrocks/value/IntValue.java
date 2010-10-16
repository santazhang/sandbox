package dbrocks.value;

import dbrocks.exception.ValueOperationException;
import dbrocks.exception.NotImplementedException;
import dbrocks.ast.ExprOp;

public class IntValue extends Value {

	public IntValue(int value) {
		intValue = value;
	}
	
	@Override
	public boolean equals(Object another) {
		if (another instanceof IntValue) {
			return this.intValue == ((IntValue) another).intValue;
		} else {
			return super.equals(another);
		}
	}
	

	@Override
	public String toString() {
		return Integer.toString(intValue);
	}
	
	@Override
	public Value op(ExprOp op, Value another) throws ValueOperationException {
		if (op == ExprOp.STR_CONCAT) {
			return new StringValue(this.intValue + another.toString());
		} else if (op == ExprOp.LOGIC_AND) {
			return BooleanValue.construct(this.toBoolean() && another.toBoolean());
		} else if (op == ExprOp.LOGIC_OR) {
			return BooleanValue.construct(this.toBoolean() || another.toBoolean());
		}
		
		if (another instanceof IntValue) {
			switch (op) {
			case ADD:
				return new IntValue(this.intValue + another.intValue);
			case SUB:
				return new IntValue(this.intValue - another.intValue);
			case MUL:
				return new IntValue(this.intValue * another.intValue);
			case DIV:
				if (another.intValue == 0) {
					throw new ValueOperationException("Divison by zero");
				}
				return new IntValue(this.intValue / another.intValue);
			case MOD:
				return new IntValue(this.intValue % another.intValue);
			default:
				throw new NotImplementedException();
			}
		} else if (another instanceof FloatValue) {
			switch (op) {
			case ADD:
				return new FloatValue(this.intValue + another.floatValue);
			case SUB:
				return new FloatValue(this.intValue - another.floatValue);
			case MUL:
				return new FloatValue(this.intValue * another.floatValue);
			case DIV:
				if (another.floatValue == 0) {
					throw new ValueOperationException("Divison by zero");
				}
				return new FloatValue(this.intValue / another.floatValue);
			case MOD:
				// use float mod
				return new FloatValue((double) this.intValue).op(ExprOp.MOD, another);
			default:
				throw new NotImplementedException();
			}
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}

	@Override
	protected boolean toBoolean() {
		return this.intValue != 0;
	}


	@Override
	public Value op(ExprOp op) throws ValueOperationException {
		if (op == ExprOp.NEG) {
			return new IntValue(-this.intValue);
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}
}
