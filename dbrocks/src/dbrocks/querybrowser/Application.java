package dbrocks.querybrowser;


public class Application {
	
	static MainFrame mainFrame;

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (mainFrame == null) {
			mainFrame = new MainFrame();
		}
		mainFrame.setVisible(true);
	}

}
