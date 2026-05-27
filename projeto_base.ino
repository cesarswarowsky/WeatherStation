//#include <Adafruit_BME280.h>

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

//#define SEALEVELPRESSURE_HPA (1013.25)
//Adafruit_BME280 bme;

ESP8266WebServer server(80);   //instantiate server at port 80 (http port) 
unsigned long delayTime;
String page = "";
const char* ssid = "WIFI-IOT";
const char* password = "Teste123";
const char* host = "unisc.br";
int count = 0;
String anemo = "";

String vel = "0";
String gust = "0";
String dir = "0";
//float raw = 0.0;
//float indiceUV = 0.0;
float temperature = 0.0;
float tempC = 0.0;
float humidity = 0.0;
float pressure = 0.0;
float dewpoint= 0.0;
float dewC= 0.0;

void setup() {
  Serial.begin(9600);
   
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("Connecting");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to IP address: ");
  Serial.println(WiFi.localIP());

  //Serial.println("BME280 test");
  //bool status;
  //default settings
  //status = bme.begin(0x76);
  //if (!status) {
  //  Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //  while (1);
  //}
  //bme.begin();
   
  Serial.println("-- Default Test --");
  delayTime = 1000;
 
  Serial.println();
 
  delay(1000); // let sensor boot up

  server.on("/", [](){
    //float temperature = bme.readTemperature()/1.0F; //Leitura Temperatura
    //float humidity = bme.readHumidity()/1.0F;// Leitura Humidade
    //float pressure = bme.readPressure()/100.0F; // Leitura Pressão Atmosférica
    //float dewpoint = (temperature - (14.55 + 0.114 * temperature) * (1 - (0.01 * humidity)) - pow(((2.5 + 0.007 * temperature) * (1 - (0.01 * humidity))), 3) - (15.9 + 0.117 * temperature) * pow((1 - (0.01 * humidity)), 14));
    
    page = "<h1>NodeMCU Weather Station</h1>";
    page+= "<h3>Temperatura: "+String(tempC)+" *C</h3>";
    page+= "<h3>Umidade Relativa: "+String(humidity)+" %</h3>";
    page+= "<h3>Ponto de Orvalho: "+String(dewC)+" *C</h3>";
    page+= "<h3>Pressao atm.: "+String(pressure * 33.8639)+" hPa</h3>";
    //page+= "<h3>Indice UV: "+ String(indiceUV) +"</h3>";
    page+= "<h3>Veloc. vento: "+ String(vel.toFloat() / 1.609) +" km/h </h3>";
    page+= "<h3>Rajadas: "+ String(gust.toFloat() / 1.609) +" km/h </h3>";
    page+= "<h3>Direcao: "+ getDir(dir) +"</h3>";
    server.send(200, "text/html", page);
  });
  
  server.begin();
}

String getDir(String d){
  if (d.equals("315")){
    return "NW";
  }
  else if (d.equals("270")){
    return "W";
  }
  else if (d.equals("225")){
    return "SW";
  }
  else if (d.equals("180")){
    return "S";
  }
  else if (d.equals("135")){
    return "SE";
  }
  else if (d.equals("90")){
    return "E";
  }
  else if (d.equals("45")){
    return "NE";
  }
  else {  
    return "N";
  }
}


void loop() { 
  delay(delayTime);
  if (Serial.available()) {
    anemo= Serial.readStringUntil('\n');
    //anemo= "4.10;215;4.83;78.49;80.58;30.20";
    Serial.println(anemo); 
    int del1 = anemo.indexOf(';');
    int del2 = anemo.indexOf(';', del1 + 1);
    int del3 = anemo.indexOf(';', del2 + 1);
    int del4 = anemo.indexOf(';', del3 + 1);
    int del5 = anemo.indexOf(';', del4 + 1);
     
    vel = anemo.substring(0, del1);
    dir = anemo.substring(del1+1, del2);    
    gust = anemo.substring(del2+1, del3);
    String te = anemo.substring(del3+1, del4);
    String hu = anemo.substring(del4+1, del5);
    String pr = anemo.substring(del5+1, anemo.length() -1 ); 

    temperature =te.toFloat();
    humidity = hu.toFloat();
    pressure = pr.toFloat(); 
  }
  
  server.handleClient();
  if (count == 10){
    printValues();
    count = 0;
  }
  count++;
}


void printValues() {
  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("Falha Comunicação");//Verifica o estado da ligação
    return;
  }
  //float temp1 = 0; //bmp.readTemperature();
  //float temp2 = temp1 * 9/5.0F + 32;
 // humidity = 0.0;
  //pressure = 0; //bmp.readPressure()/3386.39F; 
  //float dewF = 0.0;

  
  //temperature = bme.readTemperature();
  tempC = (temperature - 32) * 5/9;
  //humidity = bme.readHumidity()/1.0F;
  //pressure = bme.readPressure()/3386.39F; 
  dewpoint = (temperature - (14.55 + 0.114 * temperature) * (1 - (0.01 * humidity)) - pow(((2.5 + 0.007 * temperature) * (1 - (0.01 * humidity))), 3) - (15.9 + 0.117 * temperature) * pow((1 - (0.01 * humidity)), 14));
  dewC = (dewpoint  - 32) * 5/9;
  
  // Criar URL para o pedido
  String url = "http://rtupdate.wunderground.com/weatherstation/updateweatherstation.php?ID=ISANTA1355&PASSWORD=P0tPihMA&action=updateraw&dateutc=now&";//ID=ICAPOD1&PASSWORD=lZoTnTgh&action=updateraw&dateutc=now&
  url += "tempf="; 
  url += String(temperature);
  url += "&humidity="; 
  url += String(humidity);
  url += "&baromin="; 
  url += String(pressure);
  url += "&dewptf="; 
  url += String(dewpoint);
  url += "&windspeedmph="; 
  url += String(vel);
  url += "&windgustmph="; 
  url += String(gust);
  url += "&winddir="; 
  url += String(dir);
  Serial.println(url);//Send the request

  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin(client, url);  //Specify request destinatio
  int httpCode = http.GET(); 
  Serial.print("httpCode ");//Send the request
  Serial.println(String(httpCode));//Send the request
  //String payload = http.getString();   //Get the request response payload
  //Serial.println(payload);                     //Print the response payload
  http.end();   //Close connection
 
  
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.print(" F / ");
  Serial.print(tempC);
  Serial.println(" *C");
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Dew Point = ");
  Serial.print(dewC);
  Serial.println(" F");
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" inHg");
  Serial.print("Veloc. Vento = ");
  Serial.print(vel);
  Serial.println(" mp/h");
  Serial.print("Rajadas = ");
  Serial.print(gust);
  Serial.println(" mp/h");
  Serial.print("Direcao = ");
  Serial.print(dir);
  
  Serial.println();
}
