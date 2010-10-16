package dbrocks.value;

import dbrocks.ast.ExprOp;
import dbrocks.exception.NotImplementedException;
import dbrocks.exception.ValueOperationException;

public class FloatValue extends Value {
	
	public static double fmod(double v1, double v2) {
		if (v2 == 0) {
			throw new ValueOperationException("Divison by zero");
		}
		return v1 % v2;
	}
	
	public FloatValue(double value) {
		this.floatValue = value;
	}
	
	@Override
	public boolean equals(Object another) {
		if (another instanceof FloatValue) {
			return this.floatValue == ((FloatValue) another).floatValue;
		} else {
			return super.equals(another);
		}
	}

	@Override
	public String toString() {
		return Double.toString(floatValue);
	}

	@Override
	public Value op(ExprOp op, Value another) throws ValueOperationException {
		if (op == ExprOp.STR_CONCAT) {
			return new StringValue(this.floatValue + another.toString());
		} else if (op == ExprOp.LOGIC_AND) {
			return BooleanValue.construct(this.toBoolean() && another.toBoolean());
		} else if (op == ExprOp.LOGIC_OR) {
			return BooleanValue.construct(this.toBoolean() || another.toBoolean());
		}
		
		if (another instanceof FloatValue) {
			switch (op) {
			case ADD:
				return new FloatValue(this.floatValue + another.floatValue);
			case SUB:
				return new FloatValue(this.floatValue - another.floatValue);
			case MUL:
				return new FloatValue(this.floatValue * another.floatValue);
			case DIV:
				if (another.floatValue == 0) {
					throw new ValueOperationException("Divison by zero");
				}
				return new FloatValue(this.floatValue / another.floatValue);
			case MOD:
				return new FloatValue(FloatValue.fmod(this.floatValue, another.floatValue));
			default:
				throw new NotImplementedException();
			}
		} else if (another instanceof IntValue) {
			switch (op) {
			case ADD:
				return new FloatValue(this.floatValue + another.intValue);
			case SUB:
				return new FloatValue(this.floatValue - another.intValue);
			case MUL:
				return new FloatValue(this.floatValue * another.intValue);
			case DIV:
				if (another.intValue == 0) {
					throw new ValueOperationException("Divison by zero");
				}
				return new FloatValue(this.floatValue / another.intValue);
			case MOD:
				return new FloatValue(FloatValue.fmod(this.floatValue, (double) another.intValue));
			default:
				throw new NotImplementedException();
			}
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}

	@Override
	protected boolean toBoolean() {
		return this.floatValue != 0;
	}


	@Override
	public Value op(ExprOp op) throws ValueOperationException {
		if (op == ExprOp.NEG) {
			return new FloatValue(-this.floatValue);
		}
		throw new ValueOperationException("Not valid operation: " + op);
	}

}
