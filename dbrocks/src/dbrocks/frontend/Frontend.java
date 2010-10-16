package dbrocks.frontend;

import java.io.Reader;
import java.io.StringReader;

import dbrocks.ast.StatementGroup;

public class Frontend {
	
	public static StatementGroup parse(Reader inputReader) {
		Parser parser = new Parser();
		Lexer lexer = new Lexer(inputReader);
		lexer.setParser(parser);
		parser.setLexer(lexer);
		return parser.parseInput();
	}
	
	public static StatementGroup parse(String inputText) {
		return parse(new StringReader(inputText));
	}

}
