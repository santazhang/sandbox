package dbrocks.model;

import java.util.HashSet;
import java.util.Set;

public class Database {
	
	String name;
	Set<Table> tables;
	
	public Database(String name) {
		this.name = name;
		tables = new HashSet<Table>();
	}
	
	public String getName() {
		return name;
	}
	
	public Set<Table> getTables() {
		return tables;
	}

}
