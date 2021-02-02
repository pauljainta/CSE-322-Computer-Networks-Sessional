import java.io.*;
import java.net.Socket;
import java.util.Date;
import java.util.StringTokenizer;
import java.nio.file.Files;


public class Worker extends Thread {
    Socket socket;
    FileWriter fileWriter;


    public Worker(Socket socket){
        this.socket=socket;

        try {
            fileWriter=new FileWriter("log.txt",true);
        }catch (Exception e){

            e.printStackTrace();
        }


    }



    private void sendFile(File file) {

        long size = file.length();
        byte[] buffer = new byte[4096];
        try {
            PrintWriter pr = new PrintWriter(socket.getOutputStream());
            FileInputStream fis = new FileInputStream(file);
            String mimeType = Files.probeContentType(file.toPath());

            pr.write("HTTP/1.1 200 OK\r\n");
            pr.write("Server: Java HTTP Server: 1.1\r\n");
            pr.write("Date: " + new Date() + "\r\n");
            pr.write("Content-Type: " + mimeType + "\r\n");
            pr.write("Content-Length: " + size + "\r\n");
            pr.write("Content-Transfer-Encoding: binary\r\n");
            pr.write("Content-Disposition: attachment; filename=\"" + file.getName() + "\"\r\n");
            pr.write("\r\n");
            pr.flush();

            fileWriter.write("\r\n");
            fileWriter.write(" HTTP/1.1 200 OK\r\n");
            fileWriter.write("Server: Java HTTP Server: 1.1\r\n");
            fileWriter.write("Date: " + new Date() + "\r\n");
            fileWriter.write("Content-Type: " + mimeType + "\r\n");
            fileWriter.write("Content-Length: " + size + "\r\n");
            fileWriter.write("Content-Transfer-Encoding: binary\r\n");
            fileWriter.write("Content-Disposition: attachment; filename=\"" + file.getName() + "\"\r\n");
            fileWriter.write("\r\n");



            DataOutputStream dos = new DataOutputStream(socket.getOutputStream());

            int len;
            while ((len = fis.read(buffer)) > 0) {
                dos.write(buffer, 0, len);
            }
            dos.flush();
            pr.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run()
    {

        DataInputStream din=null;
        BufferedReader in=null;
        PrintWriter pr=null;
        String input = null;
        try {
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            pr = new PrintWriter(socket.getOutputStream());
            din=new DataInputStream(new BufferedInputStream(socket.getInputStream()));
            input = in.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }


        System.out.println("Inp="+input);

        if (input==null) System.out.println("INVALID FILEPATH");



        if(input!=null){

            pr.write("HTTP/1.1 200 OK\r\n");
            pr.write("Server: Java HTTP Server: 1.1\r\n");
            pr.write("Date: " + new Date() + "\r\n");
            pr.write("Content-Type: text/html\r\n");

            StringTokenizer stringTokenizer=new StringTokenizer(input);
            String method=stringTokenizer.nextToken();
            String path=stringTokenizer.nextToken();
            path=path.replace("%20"," ");


            if (input.length() > 0) {

                System.out.println(method);

                 if(method.equalsIgnoreCase("up"))
                {
                    try {

                        File file = new File("root\\"+path);
                        FileOutputStream fos = new FileOutputStream(file);


                        int len;
                        byte[] buffer = new byte[4096];

                        while ((len = din.read(buffer, 0, 4096)) > 0) {
                         //   System.out.println(buffer);
                            fos.write(buffer, 0, len);

                        }
                        fos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }






                if (method.equalsIgnoreCase("GET")) {

                    String html="<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" +
                            "\t\t<link rel=\"icon\" href=\"data:,\"></head><br>\n";
                    html+="<body><ul>\n";
                    int counter=0;

                    for(int i=0;i<path.length();i++)
                    {
                        if(path.charAt(i)=='/') counter++;

                    }


                    if(counter==1) {



                        if(path.equals("/") || path.equals("/root")) {


                            try {
                                fileWriter.write(input);
                                fileWriter.write("\r\n");
                                fileWriter.write(" HTTP/1.1 200 OK\r\n");
                                fileWriter.write("Server: Java HTTP Server: 1.1\r\n");
                                fileWriter.write("Date: " + new Date() + "\r\n");
                                fileWriter.write("Content-Type: text/html\r\n");
                                // fileWriter.write("Content-Length: " + size + "\r\n");
                                fileWriter.write("Content-Transfer-Encoding: binary\r\n");
                                //  fileWriter.write("Content-Disposition: attachment; filename=\"" + file.getName() + "\"\r\n");
                                fileWriter.write("\r\n");


                            } catch (IOException e) {
                                e.printStackTrace();
                            }

                            File directory = new File("root");

                            for (File f : directory.listFiles()) {

                                if (f.isDirectory())
                                    html += "<li><b><a href=\"" + f.getPath() + "\">" + f.getName() + "</a></b></li>";


                                else {
                                    String link = f.getParentFile().getName() + "/" + f.getName();
                                    System.out.println("link=" + link);
                                    html += "<li><i><a href=\"" + f.getPath() + "\">" + f.getName() + "</a></i></li>";

                                }

                            }
                        }


                        else
                        {

                            try {
                                fileWriter.write(input);
                                fileWriter.write("\r\n");
                                fileWriter.write(" HTTP/1.1 404 NOT OK\r\n");
                                fileWriter.write("Server: Java HTTP Server: 1.1\r\n");
                                fileWriter.write("Date: " + new Date() + "\r\n");
                                fileWriter.write("Content-Type: null\r\n");
                                fileWriter.write("Content Length : 0\n");

                                fileWriter.write("Content-Disposition: attachment; filename=\"" + path + "\"\r\n");

                                // fileWriter.write("Content-Length: " + size + "\r\n");
                                fileWriter.write("Content-Transfer-Encoding: binary\r\n");
                                // fileWriter.write("Content-Disposition: attachment; filename=\"" + file.getName() + "\"\r\n");
                                fileWriter.write("\r\n");



                            } catch (IOException e) {
                                e.printStackTrace();
                            }

                            html+= "<li><b> Error 404:page to pawa jassena mama,valomoto input de</b></li>";
                        }

                        html += "</ul></body></html>";
                        pr.write("Content-Length: " + html.length() + "\r\n");
                        pr.write("\r\n");
                        pr.write(html);
                        pr.flush();

                    }
                    else
                    {

                        File dir=new File("."+path);

                        if(dir.exists()) {
                            if (dir.isDirectory()) {

                                try {
                                    fileWriter.write(input);
                                    fileWriter.write("\r\n");
                                    fileWriter.write(" HTTP/1.1 200 OK\r\n");
                                    fileWriter.write("Server: Java HTTP Server: 1.1\r\n");
                                    fileWriter.write("Date: " + new Date() + "\r\n");
                                    fileWriter.write("Content-Type: text/html\r\n");

                                    // fileWriter.write("Content-Length: " + size + "\r\n");
                                    fileWriter.write("Content-Transfer-Encoding: binary\r\n");
                                    //fileWriter.write("Content-Disposition: attachment; filename=\"" + file.getName() + "\"\r\n");
                                    fileWriter.write("\r\n");

                                } catch (IOException e) {
                                    e.printStackTrace();
                                }

                                for (File file : dir.listFiles()) {

                                    if (file.isDirectory() && file.exists()) {
                                        String link = file.getParentFile().getName() + "/" + file.getName();
                                        html += "<li><b><a href=\"" + link + "\">" + file.getName() + "</a></b></li>";
                                    }

                                    if (file.isFile() && file.exists()) {



                                        String link = file.getParentFile().getName() + "/" + file.getName();
                                        System.out.println("link=" + link);
                                        html += "<li><b><a href=\"" + link + "\">" + file.getName() + "</a></b></li>";


                                    }


                                }
                            } else {
                                try {
                                    fileWriter.write(input);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }

                                sendFile(dir);

                            }
                        }

                        else
                        {
                            try {
                                fileWriter.write(input);
                                fileWriter.write("\r\n");
                                fileWriter.write(" HTTP/1.1 404  NOT OK\r\n");
                                fileWriter.write("Server: Java HTTP Server: 1.1\r\n");
                                fileWriter.write("Date: " + new Date() + "\r\n");
                                fileWriter.write("Content-Type: null\r\n");
                                fileWriter.write("Content length 0\n");

                                // fileWriter.write("Content-Length: " + size + "\r\n");
                                fileWriter.write("Content-Transfer-Encoding: binary\r\n");
                                //fileWriter.write("Content-Disposition: attachment; filename=\"" + file.getName() + "\"\r\n");
                                fileWriter.write("\r\n");

                            } catch (IOException e) {
                                e.printStackTrace();
                            }

                            html+= "<li><b> Error 404:page to pawa jassena mama,valomoto input de</b></li>";
                        }



                        html+= "</ul></body></html>" ;
                        pr.write("Content-Length: " + html.length() + "\r\n");
                        pr.write("\r\n");
                        pr.write(html);
                        pr.flush();




                    }



                }


            }

            try {
              //  socket.close();
                fileWriter.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        try {
            socket.close();
            //fileWriter.close();
        } catch (IOException e) {
            e.printStackTrace();
        }



    }



}
