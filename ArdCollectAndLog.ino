/*
SkotTestSuite
by Skot West

*** THIS ATTEMPTS TO UNIFY DATA COLLECTION, STORAGE, AND TRANSMISSION ***

Queries a Java time server (TimerServer.java) and receives a String formatted:
(hr:min:sec:month:day:yr)  where year is the last two digits from 2000. (Yes, I remember Y2K...so what?)
This data is parsed and the time is set with the setTime command (from Time.h).

USAGE: compile and run TimeServer. TimeServer will announce its IP. THIS IP MUST BE CODED INTO
THE "server" BYTE ARRAY FOUND BELOW. This does not use NTP.

*/

#include <SPI.h>         
#include <Ethernet.h>
#include <SD.h>
#include <Time.h>

//SYSTEMS PARAMTERS
const int READING_DELAY = 1000;   //5 seconds
const int ZONE = 1;              //which area is monitored
const int MAX_LOOPS = 5;
time_t time;

//Sensors
int thermoPin = A4;
int photoPin = A5;

//Data
int dataValue = 0;
String dataString;

//SD card
int SDCardPin = 10;
const int chipSelect = 4;

//Whether or not the clock has been synced
boolean time_initialized = false;

//Local Ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  
EthernetClient client;

//time server
byte server[] = {192,168,0,117}; //put TimeServer IP here!
int port = 123;

//logging server
byte logServer[] = {192,168,0,117};
int logServerPort = 4567;


//SETUP
//Get SD card, Ethernet, and Time
void setup() 
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  //Initialize data pins
  pinMode(photoPin, INPUT);
  pinMode(thermoPin,INPUT);

  //Initialize SD card
  pinMode(SDCardPin, OUTPUT);
  if (SD.begin(chipSelect)) 
  {
    Serial.println("SD card OK");
  }

  //Start Ethernet
  Ethernet.begin(mac);
  
  //Take a breath...
  delay(1000);
  
  
  //Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    //Serial.print("."); 
  }
  Serial.println();
  
  
  //Syncronize with time server
  while (time_initialized == false)
  {
      time_initialized = initialize_time();    
  } 
  
  
}


//MAIN LOOP
//Take measurements and record them
void loop()
{ 
     for (int times = 0; times < MAX_LOOPS; times++)
     {   
  
        //Take a reading every XXX seconds
        delay(READING_DELAY);  
      
        dataString = "";
      
        //print human readable time
        time = now();
        dataString += month(time);
        dataString += "/";
        dataString += day(time);
        dataString += "/";
        dataString += year(time);
        dataString += " ";
        dataString += hour(time);
        dataString += ":";
        dataString += minute(time);
        dataString += ":";
        dataString += second(time);
        
        //Attach Zone
        dataString += " Z: ";
        dataString += ZONE;
        
        dataValue = analogRead(photoPin);
        dataString += " LIGHT: ";
        dataString += dataValue;
        
        //TEST STUBS
        dataString += " TEMP: ";
        float dat = analogRead(thermoPin);
        dat = (dat * 1.8) + 32;
        dataString += (int)dat;
        
        Serial.println(dataString);
        
        //OPEN SD CARD
        
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        if(dataFile)
        {
           dataFile.println(dataString);
           dataFile.close();
        }
     }
  
    //POST DATA TO SERVER
    while (!logDataToServer() == true)
    {
       Serial.println("trying again..."); 
    }
     
}//loop

boolean initialize_time()
{
  String timeString = NULL; 
  
  client.connect(server,port);
   delay(1000);

    if (client.available())
         {  
           timeString = client.readString();
           
           //Check for String population
           if (timeString != NULL)
           {
               client.stop();  
 
               //setTime(hr,min,sec,day,month,yr)
               setTime(timeString.substring(0,2).toInt(), 
               timeString.substring(3,5).toInt(), 
               timeString.substring(6,8).toInt(), 
               timeString.substring(12,14).toInt(), 
               timeString.substring(9,11).toInt(), 
               (timeString.substring(15,17).toInt()+2000));

              Serial.println("Time synced OK");
              return (true);
              
           }//timeString != null
          
       }//cleinet.available()
}

boolean logDataToServer()
{
   client.connect(logServer,logServerPort);
   delay(1000);

    if (client.connected())
    {
      File dataFile = SD.open("datalog.txt", FILE_READ); 
      
      if (dataFile)
      {
          Serial.println("file open...");
          
          while (dataFile.available())
          {
            client.print((char)dataFile.read());
          } 
         
         dataFile.close();
         client.stop(); 
         
         if (SD.exists("datalog.txt"))
         {
           SD.remove("datalog.txt");
           Serial.println("log file cleared");
         }
         
         return (true);
      }
     
       else    
         Serial.println("NO FILE");
         return (false);
      
    }
  
    else
      Serial.println("no log server");  
      return (false);
}

    


