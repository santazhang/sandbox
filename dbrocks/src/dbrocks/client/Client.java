package dbrocks.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {
	
	Socket socket;
	Session session;
	
	public void connect(String serverHost, int port, String username, String password)
			throws UnknownHostException, IOException {
		
		socket = new Socket(serverHost, port);
		
		// TODO authentication
		session = new Session();
		
		BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		PrintStream ps = new PrintStream(socket.getOutputStream());
		
		ps.println("HELLO!");
		System.out.println(br.readLine());
	}
	
	public void close() throws IOException {
		socket.close();
		session = null;
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Client client = new Client();
		try {
			client.connect("localhost", 6400, "santa", "nopass");
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				client.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

}
