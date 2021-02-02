import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;


public class HTTPServer {
    static final int PORT = 6800;

    public static void main(String[] args) throws IOException {

        ServerSocket serverConnect = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");



        while(true)
        {

            Socket s = serverConnect.accept();

            Thread worker=new Worker(s);
            worker.start();


        }

    }

}
