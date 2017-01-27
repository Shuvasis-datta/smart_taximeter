#include <SoftwareSerial.h>

SoftwareSerial RFID(10, 11); // RX, TX
SoftwareSerial gprs(7, 8); // RX, TX

const int encoderDataPin = 20;
const byte setDriverIDSW = 24, setPassengerIDSW = 26, HireOnSW = 25, HireOffSW= 22, PaymentSW = 28, BalanceCheckSW = 27, ledPin = 13;

int passengerID[14] = {0}, driverID[14] = {0};
long int _counter = 0;
const byte perRotationCounter = 8;
 
// the setup routine runs once when you press reset:
void setup() {
  // Serial Communication Configuaration
  Serial.begin(9600);
  RFID.begin(9600);
  gprs.begin(9600);
  
  webSetup();   // gprs Setup
      
  /* Inputs */
  pinMode(encoderDataPin, INPUT_PULLUP);
  
  byte inputSW[] = {HireOnSW, HireOffSW, PaymentSW, BalanceCheckSW};
  
  for ( byte i = 0; i < (byte)sizeof(inputSW)/sizeof(byte); i++ ){
    pinMode(inputSW[i], INPUT);
  }  
  
  /* Outputs */
  pinMode(ledPin, OUTPUT);

  /* Interrupt */  
  attachInterrupt(digitalPinToInterrupt(encoderDataPin), encoderCounter, CHANGE);
}

// the loop routine runs over and over again forever:
void loop() {
/*  float distance = getDistance();
  Serial.println( "Counter : " +  String( _counter) );
  Serial.println( "Distance : " +  String( distance) + " m");
*/  
  sw();  
//  Serial.println(getFair(5));
  delay(100);
}


void encoderCounter(){
/*
 * Increment the Counter when encoder cuts
 */
  _counter++;
}

float getDistance(){
  /*
   * Calculate Distance from encoder Data
   * Return Distance from start of a Taxi 
   * All calculation in meter.
   */
   
  float circumference = 2*3.1416* 0.0325; /* Here 0.0325m is the radius of the wheel */ 
  return ((float)_counter/perRotationCounter)*circumference;
}

void displayID(int ID[]){  
  for(int i = 0; i < 14; i++){  
    Serial.print(ID[i], DEC);
  }
  Serial.println();
}

void setID(int *ID){
  /*
   * Get ID from RFID & Set ID to the corresponding request.
  */

  if(RFID.available() > 0){
    delay(100); // needed to allow time for the data to come in from the serial buffer.
    for(int i = 0; i < 14; i++){
      ID[i] = RFID.read();
    }
    RFID.flush(); // stops multiple reads
  }  
}

void sw(){
  boolean HireOn = switchState(HireOnSW);
  boolean HireOff = switchState(HireOffSW);
  boolean Payment = switchState(PaymentSW);
  boolean BalanceCheck = switchState(BalanceCheckSW);
  boolean setDriverID = switchState(setDriverIDSW);
  boolean setPassengerID = switchState(setPassengerIDSW);
  
  if ( HireOn ){
    Serial.println("This taxi is hired");
    // Calculate distance from now.
    _counter = 0;  
  }
  if ( HireOff ){
    Serial.println("This taxi is now reached at the destination.");
//    Serial.println( concat("Distance Covered : ", word(getDistance())));
    /* Pass the Distance & jam time in the web. And then receive the total fair from the web. */
    web();
  }
  if ( Payment ){
    Serial.println("Payment Switch On");
    /* Pass the Distance, Driver & Passenger ID in the web. And then receive the confirmation of the billing. */ 
  }
  if ( BalanceCheck ){
    Serial.println("Balance Check Switch On"); 
    /* Pass the Distance & passenger ID in the web. And then receive passenger balance information from web. */
    
  }
  if ( setDriverID ){
    /* Set New Driver ID */
    setID(&driverID[0]);
    Serial.print("New Driver ID is ");
    displayID(&driverID[0]); 
  }
  if ( setPassengerID ){
    /* Set New Driver ID */
    setID(&passengerID[0]);
    Serial.print("Passenger ID is ");
    displayID(&passengerID[0]); 
  }
}

boolean switchState(byte sw){
  boolean state = digitalRead(sw);
  if(state){
    /* While loop until switch is off, to Prevent Debouncing */
    while(digitalRead(sw));
    return true;
  }
  else{
      return false;
  }
}

float getFair(float distance){
  /*
   * 1st 2km = 40 Tk
   * then 12 Tk per km  
   * 3 Tk per min of jam
   * Here every measurement in km.
   */
  float fixed_distance = 2;  /* Actually 2km */ 
  float extra_distance = (float)(distance - fixed_distance);
  
  /* Fair */
  float fixed_fair = 40.0;
  float per_km_fair = 12.0;
  float jam_fair = 0.0; /* actually (time * fair) */
  
  if( distance <= fixed_distance ){
    return (float)(fixed_fair + jam_fair);
  }
  else{
    return (float)(fixed_fair + extra_distance * per_km_fair + jam_fair);
  }
}


void webSetup(){
  Serial.println("Config SIM908...");
  delay(2000);
  Serial.println("Done!...");
  gprs.flush();
  
  // attach or detach from gprs service 
  gprs.println("AT+CGATT?");
  delay(100);
  toSerial();

  // bearer settings
  gprs.println("AT+SAPBR=3,1,\"CONTYPE\",\"gprs\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprs.println("AT+SAPBR=3,1,\"APN\",\"\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprs.println("AT+SAPBR=1,1");
  delay(2000);
  toSerial();
  Serial.println("gprs Setup Completed");
}


void web(){
  Serial.println("In Web Function");
  // initialize http service
   gprs.println("AT+HTTPINIT");
   delay(2000); 
   toSerial();
   
   // set http param value
   gprs.println("AT+HTTPPARA=\"URL\",\"http://www.smarttaximeter.rmabd.org/?pID=24850484852506649575254533&dID=24856484851566567544870673&distance=4&jamTime=1&request=payment\"");
   delay(2000);
   toSerial();

   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   gprs.println("AT+HTTPACTION=0");
   delay(6000);
   toSerial();

   // read server response
   gprs.println("AT+HTTPREAD"); 
   delay(1000);
   toSerial();

   gprs.println("");
   gprs.println("AT+HTTPTERM");
   toSerial();
   delay(300);

   gprs.println("");
   delay(10000);
   Serial.println("Going from Web");
}

void toSerial()
{
  while(gprs.available()!=0)
  {
    Serial.write(gprs.read());
  }
}
