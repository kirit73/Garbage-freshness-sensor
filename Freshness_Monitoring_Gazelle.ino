#include <TimeLib.h>
#include <DS1307RTC.h>
#define MOBILE_NUMBER     "8377920943"
#define APN             "www"
#define CLIENT_API_URL "oc5987siw8.execute-api.ap-south-1.amazonaws.com/logistics-radar/record-sensor-data"
//#define CLIENT_API_URL  "ptsv2.com/t/nl3kn-1569482623/post"
//#define CLIENT_API_URL  "ggpi08zzih.execute-api.ap-south-1.amazonaws.com/beta/record-data"
                        
//const String CID        =      "e3168e9425e52a5513c5c26e5fca0eb2";
//const String timestamp =       "123456";
const String Device_id  =      "Freshness-001";
String HTTP_POST_PARAMS;        // AS per client GD is sensor data , DC is battery data and DT is date time, DI is device ID,  CID is fixed and is given by client

int delay_val           =     3000;

//-------------------------------------------------//
//  Private Function define
void gsm_http_post ();
void co2_ppm (void);
void DHT22 (void);
void O2 (void);
void GPS (void) ;
void Time (void);
void Update_Data(String,int,float,float,float,float,float) ;
void Datalogger (void);






//-----------------------------------------------//

#include <Wire.h>                             //I2C for OLED
#define anInput     A0                        //analog feed from MQ135
#define digTrigger   3                        //digital feed from MQ135
#define co2Zero     55                        //calibrated CO2 0 level
int co2ppm = 0  ;
//-------------------------------------------------//

#include <SimpleDHT.h>
int pinDHT22 = 2;
SimpleDHT22 dht22(pinDHT22);
float temperature = 0  ;
float humidity = 0  ;
//-------------------------------------------------//

//o2//
float VRefer = 3.3;       // voltage of adc reference
const int pinAdc   = A1;
float Concentration = 0;
//------------------------------------------------//
//Connect with pin 18 and 19
#include <TinyGPS.h>
float lat = 0;
float lon = 0;
TinyGPS gps; // create gps object

//-----------------------------------------------------//
//Datalogger//
#include <SD.h>
#include <SPI.h>
File myFile;
int pinCS = 53;
char line2;
//-----------------------------------------------------//

 String Date_data;

//------------------------------------------------//
void setup() {
   Serial.begin(9600);
   pinMode(anInput,INPUT);                     //MQ135 analog feed set for input
  pinMode(digTrigger,INPUT);                  //MQ135 digital feed set for input
  Serial1.begin(9600); 
  pinMode(pinCS, OUTPUT);

   
// Begin block
      /*  RTC SET HERE    */
        tmElements_t tm;
              tm.Hour = 4;               // set the RTC to an arbitrary time
              tm.Minute = 15;
              tm.Second = 50;
              tm.Day = 03;
              tm.Month = 10;
              tm.Year = 2019 - 1970;      // tmElements_t.Year is the offset from 1970
              RTC.write(tm);              // set the RTC from the tm structure
//        /*RTC SET TILL HERE*/
  //   Block end
  time_t t; //create a temporary time variable so we can set the time and read the time from the RTC
  t = RTC.get(); 
  delay(200);

  (SD.begin());

     
}

 void loop() {


   // HTTP_POST_PARAMS = "CID=" + CID + "&" + "DI=" + Device_id + "&" + "GD=" + String(distance_value) + "&" + "DC=" + String(Batt_Value) + "&" + "DT=" + Date_data;
  
 // HTTP_POST_PARAMS = "CID= " + CID + "&" + "DI= " + Device_id + "&" + "CD= " + String(co2ppm) + "&" + "TD= " + String(temperature) + "&" + "HD= " + String(humidity) + "&" + "LAT= " + String(lat)    + "&" + "LON= " + String(lon)    +  "&" + "O2= " + String(Concentration);
 
  //HTTP_POST_PARAMS = "DI= " + Device_id + "&" + "CD= " + String(co2ppm) + "&" + "TD= " + String(temperature) + "&" + "HD= " + String(humidity) + "&" + "LAT= " + String(lat,8)    + "&" + "LON= " + String(lon,8)    +  "&" + "O2= " + String(Concentration)+ "&" + "DT=" + Date_data;

HTTP_POST_PARAMS =   "Time= " + Date_data + "&" +"DI= " + Device_id + "&" + "CD= " + String(co2ppm) + "&" + "TD= " + String(temperature) + "&" + "HD= " + String(humidity) + "&" + "LAT= " + String(lat,8)    + "&" + "LON= " + String(lon,8)    +  "&" + "O2= " + String(Concentration);

// HTTP_POST_PARAMS = "timestamp= " + timestamp +"&" +  "DI= " + Device_id + "&" + "CD= " + String(co2ppm);

 //HTTP_POST_PARAMS [] =  "{ \"DI\" : Device_id, \"CD\" : co2ppm }"; 

 //HTTP_POST_PARAMS = " {\"DI\": Test-001, \"CD\": 52.1}";

 
  DHT22() ;
  delay(5000);
  co2_ppm();
  delay(5000);
  O2();
  delay(5000);
  Time();
  delay(5000);
  GPS() ;
  delay(5000);
  Datalogger();
  delay(5000);
  gsm_http_post();// for posting data
  
}




void Update_Data(String Date_data ,int co2ppm, float temperature,float humidity, float lat, float lon, float Concentration)
{
  } 




void gsm_http_post ()
{
 delay(10000);
  Serial.println("AT");
  // send_to_serial();//Print GSM Status an the Serial Output;
  delay(1000);

  Serial.println("AT+CGATT?");
  // send_to_serial();//Print GSM Status an the Serial Output;
  delay(1000);

  Serial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  //send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+SAPBR=3,1,\"APN\",\"" + String(APN) + "\"");
  // send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+SAPBR=1,1");
  // send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+SAPBR=2,1");
  //  send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+HTTPINIT");
  // send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+HTTPPARA=\"CID\",1");
  // send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+HTTPPARA=\"URL\",\"" + String(CLIENT_API_URL) + "\"");
  // send_to_serial();//Print GSM Status an the Serial Output;SECURE_URL
  delay(delay_val);

  //Serial.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  // send_to_serial();//Print GSM Status an the Serial Output;
 // delay(delay_val);

  //Serial.println("AT+HTTPPARA=\"CONTENT\",\"application/form-data\"");
  // send_to_serial();//Print GSM Status an the Serial Output;
 //delay(delay_val);

 Serial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
//   send_to_serial();//Print GSM Status an the Serial Output;
 delay(delay_val);


  Serial.println("AT+HTTPDATA=" + String(HTTP_POST_PARAMS.length()) +  ",10000");
  //  send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);
  delay(delay_val);

  Serial.println(HTTP_POST_PARAMS);
  //  send_to_serial();//Print GSM Status an the Serial Output;
  delay(5000);

  Serial.println("AT+HTTPACTION=1");
  //  send_to_serial();//Print GSM Status an the Serial Output;
  delay(5000);

  //Terminate HTTP & GPRS and then switch off
  Serial.println("AT+HTTPTERM");
  //  send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("AT+SAPBR=0,1");
  //  send_to_serial();//Print GSM Status an the Serial Output;
  delay(delay_val);

  Serial.println("");
  delay(1000);
}



void Time (){

time_t t;

  t = RTC.get();

  String sec_value = String(second(t));
  String min_value = String(minute(t));
  String hour_value = String(hour(t));
  String day_value = String(day(t));
  String month_value = String(month(t));
  String year_value = String(year(t));

  if (second(t) < 10)
  {
    sec_value = "0" + String(second(t));
  }
  if (minute(t) < 10)
  {
    min_value = "0" + String(minute(t));
  }
  if (hour(t) < 10)
  {
    hour_value = "0" + String(hour(t));
  }
  if (day(t) < 10)
  {
    day_value = "0" + String(day(t));
  }
  if (month(t) < 10)
  {
    month_value = "0" + String(month(t));
    
  }
 Date_data = String(year(t)) + "-" + month_value + "-" + day_value + " " + hour_value + ":" + min_value + ":" + sec_value;
 Serial.println(Date_data);
 delay(5000);
  


}
void co2_ppm (){

int co2now[10];                               //int array for co2 readings
int co2raw = 0;                               //int for raw value of co2
int co2comp = 0;                              //int for compensated co2 
                             //int for calculated ppm
int zzz = 0;                                  //int for averaging


  for (int x = 0;x<10;x++){                   //samplpe co2 10x over 2 seconds
    co2now[x]=analogRead(A0);
    delay(200);
  }

for (int x = 0;x<10;x++){                     //add samples together
    zzz=zzz + co2now[x];
    
  }
  co2raw = zzz/10;                            //divide samples by 10
  co2comp = co2raw-co2Zero;                 //get compensated value
  co2ppm = map(co2comp,0,1023,400,5000);      //map value for atmospheric levels
  Serial.println("Co2 PPM");
 
  Serial.println(co2ppm);
  delay(2000);
}


void DHT22 () 
{ 
 
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT22...");
  
  // read without samples.
  // @remark We use read2 to get a float data, such as 10.1*C
  //    if user doesn't care about the accurate data, use read to get a byte data, such as 10*C.
//  float temperature = 0;
//  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err);delay(2000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((float)temperature); Serial.print(" *C, ");
  Serial.print((float)humidity); Serial.println(" RH%");
  
  // DHT22 sampling rate is 0.5HZ.
  delay(2000);
}


void GPS()
{
    while(Serial1.available()){ // check for gps data
    if(gps.encode(Serial1.read()))// encode gps data
    { 
    gps.f_get_position(&lat,&lon); // get latitude and longitude

    Serial.print("Position: ");
    
    //Latitude
    Serial.print("Latitude: ");
    Serial.print(lat,8);
    
    Serial.print(",");
    
    //Longitude
    Serial.print("Longitude: ");
    Serial.println(lon,8);  
   }
  }
 

}


void Datalogger () {



  // Create/Open file 
  myFile = SD.open("Logger.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");
    // Write to file
//    myFile.println(CID + "&" + Device_id + "&" + co2ppm + "&" + temperature + "&" + humidity + "&" + (lat,8) + "&" + (lon,8) + "&" + Concentration  );

    myFile.println( Date_data + "&" + Device_id + "&" + co2ppm + "&" + temperature + "&" + humidity + "&" + (lat,8) + "&" + (lon,8) + "&" + Concentration);

    Serial.println("done");
    myFile.close(); // close the file
  }


  //HTTP_POST_PARAMS = "CID= " + CID + "&" + "DI= " + Device_id + "&" + "CD= " + String(co2ppm) + "&" + "TD= " + String(temperature) + "&" + "HD= " + String(humidity) + "&" + "LAT= " + String(lat,8)    + "&" + "LON= " + String(lon,8)    +  "&" + "O2= " + String(Concentration);

  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  } 
 if (myFile) {
 Serial.println("Read:");
//// Reading the whole file
 while (myFile.available()) 
      {
Serial.write(myFile.read());}
   
    myFile.close();
  }
  
  }

void O2() {
 
    long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(pinAdc);
    }

    sum >>= 5;

    float MeasuredVout = sum * (VRefer / 1023.0);

    // Vout samples are with reference to 3.3V    

    //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
    //when its output voltage is 2.0V,
     Concentration = (MeasuredVout * 0.21 / 2.0) *100;
    //float Concentration_Percentage=Concentration*100;
    Serial.println("O2 Percentage");
    Serial.println(Concentration);
} 
