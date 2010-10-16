package dbrocks.server;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;

public class ServerThread extends Thread {
	
	Socket socket;
	Server server;
	
	public ServerThread(Server server, Socket socket) {
		this.server = server;
		this.socket = socket;
	}
	
	@Override
	public synchronized void run() {
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			PrintStream ps = new PrintStream(socket.getOutputStream());
			System.out.println(br.readLine());
			ps.println("HELLO!");
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				this.socket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			System.out.println("Closed connection from " + this.socket.getInetAddress());
		}
	}

}
