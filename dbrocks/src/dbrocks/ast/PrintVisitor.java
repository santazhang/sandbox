package dbrocks.ast;

import java.io.PrintStream;

import dbrocks.util.BiName;

public class PrintVisitor implements Visitor {
	
	PrintStream ps = System.out;

	@Override
	public void visit(Assignment assign) {
		ps.println("Assignment");
	}

	@Override
	public void visit(BinaryExpr binaryExpr) {
		ps.println("BinaryExpr");
		ps.println("  Operator = " + binaryExpr.op);
		binaryExpr.left.accept(this);
		binaryExpr.right.accept(this);
	}

	@Override
	public void visit(Const constValue) {
		ps.println("ConstValue  " + constValue.getValue().toString() + " -> " + constValue.getValue().getClass());
	}

	@Override
	public void visit(Create createStmt) {
		ps.println("CreateStatement");
	}

	@Override
	public void visit(Delete deleteStmt) {
		ps.println("DeleteStatement");
	}

	@Override
	public void visit(Drop dropStmt) {
		ps.println("DropStatement");
	}

	@Override
	public void visit(FunCall funCall) {
		ps.println("FuncationCall " + funCall.funName);
		for (Expr expr : funCall.args) {
			expr.accept(this);
		}
	}

	@Override
	public void visit(Select selectStmt) {
		ps.println("SelectStmt");
		if (selectStmt.isSelectAll()) {
			ps.println("  Select *");
		} else {
			for (BiName biName : selectStmt.selectItems) {
				if (biName.name2 != null) {
					ps.println("  " + biName.name + " as " + biName.name2);
				} else {
					ps.println("  " + biName.name);
				}
			}
		}
		if (selectStmt.criterion != null) {
			selectStmt.criterion.accept(this);
		}
	}

	@Override
	public void visit(StatementGroup stmtGrp) {
		ps.println("StatementGroup");
		for (Statement stmt : stmtGrp.stmtList) {
			stmt.accept(this);
		}
	}

	@Override
	public void visit(UnaryExpr unaryExpr) {
		ps.println("UnaryExpr " + unaryExpr.op);
		unaryExpr.expr.accept(this);
	}

	@Override
	public void visit(Update updateStmt) {
		ps.println("UpdateStatement");
	}

	@Override
	public void visit(Variable variable) {
		ps.println("Variable " + variable.name);
	}

}
