package dbrocks.storage;

public interface TableStore {

	public boolean addRecord(Record rec);
	
	public boolean removeRecord(Record rec);
	
}
