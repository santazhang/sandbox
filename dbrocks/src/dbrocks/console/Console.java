package dbrocks.console;

import dbrocks.ast.StatementGroup;
import dbrocks.frontend.Frontend;
import dbrocks.ast.PrintVisitor;

public class Console {
	
	public static void main(String[] args) {
		System.out.println("TODO : Console");
		StatementGroup stmtGrp1 = Frontend.parse("select * from b");
		PrintVisitor pv = new PrintVisitor();
		pv.visit(stmtGrp1);
		System.out.println("----");
		StatementGroup stmtGrp2 = Frontend.parse("select a from b where a > 2 ;;;;;;;;;; select * from b;select a as ab from b as ba where sin(sin(sin(noway, 23 + 234, sd(eee))))");
		pv.visit(stmtGrp2);
		System.out.println("----");
		StatementGroup stmtGrp3 = Frontend.parse("select a as x, c as d, f as fu, kolumn1 from b as T, sa as U where a < 324234 \nselect a from b where a < abs(-3.12123) + len(\"what\")");
		pv.visit(stmtGrp3);
	}

}
