package dbrocks.value;

import dbrocks.ast.ExprOp;
import dbrocks.exception.NotImplementedException;
import dbrocks.exception.ValueOperationException;

public class BinaryValue extends Value {
	
	public BinaryValue(byte[] value) {
		binaryValue = value;
	}
	
	@Override
	public boolean equals(Object another) {
		if (another instanceof BinaryValue) {
			byte[] binAnother = ((BinaryValue) another).binaryValue;
			if (binaryValue == binAnother) {
				// if both array are same reference, they must be equal
				return true;
			}
			if (binAnother.length != binaryValue.length) {
				return false;
			}
			for (int i = 0; i < binaryValue.length; i++) {
				if (binaryValue[i] != binAnother[i]) {
					return false;
				}
			}
			return true;
		} else {
			return super.equals(another);
		}
	}

	@Override
	public Value op(ExprOp op, Value another) throws ValueOperationException {
		throw new ValueOperationException("Not valid operation: " + op);
	}

	@Override
	protected boolean toBoolean() {
		throw new NotImplementedException();
	}

	@Override
	public Value op(ExprOp op) throws ValueOperationException {
		throw new ValueOperationException("Not valid operation: " + op);
	}
}
