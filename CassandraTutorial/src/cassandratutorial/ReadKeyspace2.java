package cassandratutorial;

import me.prettyprint.hector.api.exceptions.HectorException;

public class ReadKeyspace2 {
	
	private final static String KEYSPACE = "Keyspace2";
	private final static String HOST_PORT = "localhost:9160";
	
	public static void main(String[] args) throws HectorException {
		ExampleCommons ec = new ExampleCommons(HOST_PORT, KEYSPACE);
		int count = 100;
		int oopsCounter = 0;
		String prefix = "example2_key_";
		System.out.println("Reading " + count + " values from keyspace: " + KEYSPACE);
		for (int i = 0; i < count; i++) {
			try {
				String value = ec.get(prefix + i);
				if (value != null) {
					System.out.println(prefix + i + "=" + ec.get(prefix + i));
				} else {
					oopsCounter++;
					System.out.println("oops!");
				}
			} catch (Exception e) {
				oopsCounter++;
				System.out.println("oops!");
			}
		}
		System.out.println("Finished!");
		if (oopsCounter != 0) {
			System.out.println(oopsCounter + " of " + count + " (" + (oopsCounter * 100 / count) + "%) operations failed!");
		} else {
			System.out.println("Everything works fine!");
		}
	}

}
