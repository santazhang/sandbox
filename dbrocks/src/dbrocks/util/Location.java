package dbrocks.util;

public class Location implements Comparable<Location> {

	public static final Location NO_LOCATION = new Location(Integer.MAX_VALUE,
			Integer.MAX_VALUE);

	private int line;

	private int column;

	public Location(int line, int column) {
		this.line = line;
		this.column = column;
	}

	@Override
	public String toString() {
		return "(" + line + "," + column + ")";
	}

	@Override
	public int compareTo(Location another) {
		if (line > another.line) {
			return 1;
		}
		if (line < another.line) {
			return -1;
		}
		if (column > another.column) {
			return 1;
		}
		if (column < another.column) {
			return -1;
		}
		return 0;
	}
}
