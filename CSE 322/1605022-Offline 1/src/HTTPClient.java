import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class HTTPClient  {


    public static void main(String[] args) throws Exception {


        while (true)
        {

            Socket socket=new Socket("localhost",6800);

            Thread thread=new ClientThread(socket);
            thread.start();

        }


    }


}




class ClientThread extends Thread
{
    Socket socket;
    String filepath;

    public ClientThread(Socket socket)
    {
        this.socket=socket;

        Scanner scanner = new Scanner(System.in);

        filepath = scanner.nextLine();

    }

    @Override
    public void run()
    {


        BufferedReader bufferedReader = null;
        PrintWriter printWriter = null;
        DataOutputStream dataOutputStream = null;

        try {


            printWriter = new PrintWriter(socket.getOutputStream());
            dataOutputStream = new DataOutputStream(socket.getOutputStream());
            bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));


            File file = new File(filepath);

            if (!file.exists())
            {
                System.out.println("INVALID FILEPATH");


            }

            else
            {
                FileInputStream fileInputStream = new FileInputStream(file);
                printWriter.println("up " + file.getName());
                printWriter.println();
                printWriter.flush();

                int len;
                byte[] buffer = new byte[4096];

                while ((len = fileInputStream.read(buffer)) > 0) {
                    dataOutputStream.write(buffer, 0, len);
                    System.out.println(buffer);
                }
                dataOutputStream.flush();
                fileInputStream.close();
            }


            bufferedReader.close();
            dataOutputStream.close();
            printWriter.close();


        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}