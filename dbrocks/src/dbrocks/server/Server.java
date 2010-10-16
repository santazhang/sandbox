package dbrocks.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
	
	ServerConfig config;
	ServerStatus status;
	
	public Server(ServerConfig config) {
		this.config = config;
		status = ServerStatus.NOT_RUNNING;
	}
	
	public void start() {
		System.out.println("=== DBRocks ===");
		System.out.println("Server starting...");
		status = ServerStatus.STARTING;
		try {
			ServerSocket serverSocket = new ServerSocket(config.port);
			System.out.println("Server started, listening on port " + config.port);
			status = ServerStatus.RUNNING;
			while (status == ServerStatus.RUNNING) {
				Socket socket = serverSocket.accept();
				System.out.println("Received connection from " + socket.getInetAddress());
				ServerThread serverThread = new ServerThread(this, socket);
				serverThread.start();
			}
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Failed to start server");
		} finally {
			this.onStopped();
		}
	}
	
	public ServerStatus getStatus() {
		return status;
	}
	
	public void stop() {
		status = ServerStatus.STOPPING;
	}
	
	void onStopped() {
		status = ServerStatus.NOT_RUNNING;
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		ServerConfig config = ServerConfig.parse("port=8000");
		Server server = new Server(config);
		server.start();
	}
}
