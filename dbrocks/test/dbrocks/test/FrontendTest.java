package dbrocks.test;

import static org.junit.Assert.*;

import org.junit.Test;
import dbrocks.frontend.Frontend;
import dbrocks.ast.StatementGroup;

public class FrontendTest {

	@Test
	public void testParseString() {
		StatementGroup stmtGrp = Frontend.parse("select a from b");
		assertNotNull(stmtGrp);
	}

}
