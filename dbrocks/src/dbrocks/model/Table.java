package dbrocks.model;

public class Table {
	
	String name;
	Schema schema;
	
	public Table(String name, Schema schema) {
		this.name = name;
		this.schema = schema;
	}

	public String getName() {
		return name;
	}
	
	public Schema getSchema() {
		return schema;
	}
	
	public RowIterator iterator() {
		return new RowIterator();
	}
	
}
