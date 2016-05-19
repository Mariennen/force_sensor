//#include <SoftwareSerial.h>
#define RED "Batatinha"
#define PASS "naonaonao"
#define IP "10.0.1.3"
#define TIME_AD 200
#define TIME 10
#define TIME_CALIBRATION 5000
#define SERIAL_WIFI Serial
#define SERIAL_CONSOLE BT1
#define DEBUG 0

#if DEBUG == 1
  #define CONSOLE_PRINTLN(x) SERIAL_CONSOLE.println(x)
  #define CONSOLE_PRINT(x) SERIAL_CONSOLE.print(x)
#else
  #define CONSOLE_PRINTLN(x)
  #define CONSOLE_PRINT(x) 
#endif

//SoftwareSerial SERIAL_WIFI(10,11);//RX/Tx
//Variables
boolean flagAllowTransmit = true;
int Rp_signal=A6;// Right Pedal
int Lp_signal=A5; //Left Pedal
float voltager_reference=0;
float voltagel_reference=0;
long lastTime=0;

void setup()
{
  // Connect to PC serial terminal
  Serial.begin(115200);
  // Connect to ESP8266 via SoftwareSerial (RX = 10, TX = 11)
  SERIAL_WIFI.begin(115200);
    blink_(2); 
    
 //Calibration to find new zero of force
   CONSOLE_PRINTLN("Waiting for calibration");

   calibration();
   
 //References voltages
   CONSOLE_PRINTLN("Right Reference:");
   CONSOLE_PRINTLN(voltager_reference);
   CONSOLE_PRINT("Left Reference:");
   CONSOLE_PRINTLN(voltagel_reference);
   // Checking if we can talk to ESP8266
  CONSOLE_PRINTLN("Checking if we can talk to ESP8266");
  
  blink_(3); 
  sendCommand("ATE0");
  //delay(4000);
  checkResponse("OK");

  // Connecting to router
  CONSOLE_PRINTLN("Connecting to router");

  connectWiFi();
  
  // Connecting to server
  CONSOLE_PRINTLN("Connecting to server");
  connectServer();
  //Timer=0
  delay(200);
  lastTime=millis();
}

void loop (){
  
 if(millis() - lastTime > 10 &&
    flagAllowTransmit == true)//ms
 {    
   lastTime = millis();
  int Rp_value = analogRead(Rp_signal);   // right pedal signal
  int Lp_value = analogRead(Lp_signal);   // left pedal signal
  float voltager=(Rp_value*(5.0/1023.0)); // Digital output
  float voltagel=(Lp_value*(5.0/1023.0)); // Digital output
  
  //Force value of new zero
  float forcer_reference=((76.178*(voltager_reference))-3.0464);
  float forcel_reference=(55.165*(voltagel_reference)-2.0958);
  
  //Actual force 
  float forcer1=((76.178*(voltager))-3.0464);
  
  float forcel1=(55.165*(voltagel)-2.0958);

  //New force value
  float forcer=forcer1-forcer_reference;
  float forcel=forcel1-forcel_reference;
  
  //Force value to string
  String forceString="";
  forceString.concat(forcer);
  forceString += " ";
  
  forceString.concat(forcel);
  
  updateforce(forceString);
 }
}

void calibration()
{
  unsigned long counter=0;
  float sumr=0; 
  float suml=0; 

  long timer = millis();
    
  while ((millis()-timer)<TIME_CALIBRATION)
  {
  int Rp_value = analogRead(Rp_signal);   // right pedal signal
  int Lp_value = analogRead(Lp_signal);
  //Digital values   
  float voltager=(Rp_value*(5.0/1023.0)); 
  float voltagel=(Lp_value*(5.0/1023.0));
   
  sumr=sumr+voltager;
  suml=suml+voltagel;
  //CONSOLE_PRINTLN(sumr);
  //CONSOLE_PRINTLN(suml);
  counter++;
   
  }
  //Force average during TIME_CALIBRATION
 voltager_reference=sumr/(float)counter;
 voltagel_reference=suml/(float)counter;
 
//  CONSOLE_PRINTLN(voltager_reference);
//  CONSOLE_PRINTLN(voltagel_reference);
    
}

void sendCommand(String command)
{
  SERIAL_WIFI.println(command + "\r");
  //CONSOLE_PRINTLN(command + "\r");
}

void sendForce(String command)
{
  SERIAL_WIFI.print(command);
  //CONSOLE_PRINTLN(command + "\r");
}

void checkResponse(String response)
{
  String s = "";
  while(!s.equals(response + "\r"))
  {
    s = SERIAL_WIFI.readStringUntil('\n');
    
     CONSOLE_PRINTLN("[ESP8266]: " + s);
  }
}

void checkResponse2(char response)
{
  char c;
  long forcet=millis();
  while(c != response)
  {
    c = SERIAL_WIFI.read();
    //CONSOLE_PRINT(c);    CONSOLE_PRINT("-HEX:");    CONSOLE_PRINTLN(c,HEX);
    if (millis()-forcet>TIME) {
      break;
    }
  }
}


void checkResponse3(String response)
{
  long forcet=millis();
  String s = "";
  while(!s.equals(response + "\r"))
  {
    s = SERIAL_WIFI.readStringUntil('\n');
    //CONSOLE_PRINTLN(s);
    if (millis()-forcet>TIME) {
      break;
    }
  }
}

boolean connectWiFi(){
  sendCommand("AT+CWMODE=1");
  delay(2000);
  checkResponse("OK");
  
  String cmd="AT+CWJAP=\"";
  cmd+=RED;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  sendCommand(cmd);
  delay(4000);
  checkResponse("OK");
  
  return true;
}

boolean connectServer(){
  String cmd="AT+CIPSTART=\"TCP\",\"";
  cmd +=IP;
  cmd +="\",5432"; 
  sendCommand(cmd);
  delay(1000);
  checkResponse("OK");
}

//Sending force values
void updateforce(String force){
  //CONSOLE_PRINT("Enviando command ");
  String cmd = "AT+CIPSEND=";
  cmd += force.length();
  flagAllowTransmit = false;
  sendCommand(cmd);
  checkResponse2('>');
  sendForce(force);
  //CONSOLE_PRINT("enviado forca ");
  checkResponse3("SEND OK");
  flagAllowTransmit = true;
  //CONSOLE_PRINTLN("fim");
}

void blink_(int n){
  int i=0;
  
  for (i=0;i<n;i++)
      {
      digitalWrite (13,HIGH);
      delay(100);
      digitalWrite (13,LOW);
      delay(100);
      }

}
