       

import java.net.*;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Calendar;
import java.util.Date.*;

import java.awt.Event.*;

public class DataCollectionServer 
{

//HOUR:MIN:SEC:MONTH:DAY:YEAR
    
    public static void main(String [] args) throws IOException
    {
        int port = 4567;
                
        ServerSocket serverSocket;
        Socket socket;
        InputStream in = null;
        OutputStream out = null;
        
	
        
        //Arguments set the port
        if (args != null)
        {
            if (args.length > 0)
            {
                port = Integer.parseInt(args[0]);
            }
        }
            
        System.out.println("Data Collection Server starting...");
        
        try
        {
            InetAddress addy = InetAddress.getLocalHost();
            System.out.println("I'm at: " + addy.getHostAddress() + ":" + port + "\n");
        }
        catch (UnknownHostException e)
        {
            System.out.println("SERVER: Problem with resolving local host...");
        }
        
        serverSocket = new ServerSocket(port);
        
        System.out.println("Ready...");
        
        //Listen for socket connection requests; create a new socket and pass
        //it to a new ClientThread 
        while (true)
        {
            socket = serverSocket.accept();
            
            
            if (socket.isConnected() == true)
            {
                System.out.println("Connection...");
                
                try
                {
                    
                    in = socket.getInputStream();
                    out = socket.getOutputStream();

                }
                catch (IOException e)
                {
                    System.out.println("ClientThread: error opening streams and sockets");
                }
                
                //GET DATA
                int input = in.read();
                
                while(input != -1)
                {
                    System.out.print((char)input);
                    input = in.read();
                }
                
                socket.close();
                in.close();
                out.close();
                socket = null;
                
                System.out.println("CLOSED");
		
                //System.out.println("Served!");
            }
        }
        
    }
    
}  


