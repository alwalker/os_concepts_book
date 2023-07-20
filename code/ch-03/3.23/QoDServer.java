import java.net.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import java.io.*;

public class QoDServer {
	public static void main(String[] args) {
		try {
			List<String> quotes;
			try (Stream<String> lines = Files.lines(Paths.get("quotes.txt"))) {
				quotes = lines.collect(Collectors.toList());
			}

			ServerSocket sock = new ServerSocket(6017);

			while (true) {
				Socket client = sock.accept();

				PrintWriter pout = new PrintWriter(client.getOutputStream(), true);

				int quoteIndex = (int) Math.floor(Math.random() * (quotes.size() - 1));

				pout.println(quotes.get(quoteIndex));

				client.close();
			}
		} catch (IOException ioe) {
			System.err.println(ioe);
		}
	}
}
