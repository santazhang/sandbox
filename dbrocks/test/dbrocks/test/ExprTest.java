package dbrocks.test;

import static org.junit.Assert.*;

import org.junit.Test;

import dbrocks.ast.BinaryExpr;
import dbrocks.ast.ExprOp;
import dbrocks.ast.Const;
import dbrocks.value.FloatValue;
import dbrocks.value.IntValue;
import dbrocks.value.StringValue;
import dbrocks.ast.Variable;

public class ExprTest {

	@Test
	public void testEval() {
		ExprOp[] arithmaticOp = {ExprOp.ADD, ExprOp.SUB, ExprOp.MUL, ExprOp.DIV, ExprOp.MOD};
		Const c1 = new Const(new IntValue(1));
		Const c2 = new Const(new FloatValue(1.0));
		for (ExprOp op : arithmaticOp) {
			BinaryExpr expr = new BinaryExpr(c1, op, c2);
			assertTrue(expr.eval(null) instanceof FloatValue);			
		}
		
	}

	@Test
	public void testOptimize() {
		Const c1 = new Const(new IntValue(3));
		Const c2 = new Const(new StringValue("Hello, boy"));
		BinaryExpr exp = new BinaryExpr(c1, ExprOp.STR_CONCAT, c2);
		assertTrue(exp.optimize() instanceof Const);
		
		Variable var = new Variable("va");
		BinaryExpr exp2 = new BinaryExpr(var, ExprOp.ADD, c1);
		assertEquals(exp2, exp2.optimize());
	}

	@Test
	public void testGetVariables() {
		Variable var = new Variable("nice");
		assertEquals(var.getVariables().size(), 1);
		Variable var2 = new Variable("nice");
		BinaryExpr exp = new BinaryExpr(var, ExprOp.MOD, var2);
		assertEquals(exp.getVariables().size(), 1);
		Variable var3 = new Variable("nice3");
		BinaryExpr exp2 = new BinaryExpr(exp, ExprOp.MUL, var3);
		assertEquals(exp2.getVariables().size(), 2);
		
		Const c1 = new Const(new IntValue(4));
		assertEquals(c1.getVariables().size(), 0);
		BinaryExpr exp3 = new BinaryExpr(c1, ExprOp.SUB, exp2);
		assertEquals(exp3.getVariables().size(), 2);
	}

}
