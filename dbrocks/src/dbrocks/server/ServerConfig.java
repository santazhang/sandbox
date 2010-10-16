package dbrocks.server;

public class ServerConfig {
	
	int port;
	
	/**
	 * Construct a default server config
	 */
	public ServerConfig() {
		port = 6400;
	}

	public static ServerConfig parse(String text) {
		ServerConfig config = new ServerConfig();
		return config;
	}
	
}
