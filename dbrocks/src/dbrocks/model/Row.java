package dbrocks.model;

import dbrocks.value.Value;

public class Row {
	
	Schema schema;
	
	byte[] raw;
	
	public Schema getSchema() {
		return schema;
	}
	
	public Value getColumn(String column) {
		return null;
	}
	
}
