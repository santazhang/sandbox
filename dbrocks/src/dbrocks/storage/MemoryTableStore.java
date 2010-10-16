package dbrocks.storage;

import java.util.TreeSet;

public class MemoryTableStore implements TableStore {
	
	TreeSet<Record> dataSet;

	@Override
	public boolean addRecord(Record rec) {
		// TODO Auto-generated method stub
		
		return false;
	}

	@Override
	public boolean removeRecord(Record rec) {
		// TODO Auto-generated method stub
		return false;
	}

}
