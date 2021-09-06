//GGA Assignments
String inByte;
char Byte;
char buff[13][12];
float UTC = 0;//Time
float LAT = 0; //Latitude
char LATDir = 'I'; //Direction
float LON = 0; //Longitude
char LONDir = 'I'; //Direction
unsigned int fixStatus; //Quality Indicator
int NoSVs = 0; //Number of Satelites in use
float HDop = 0; //Height Dilution of Precision
float ALT = 0; //Altitude
char ALTUnit = 'I'; //Altitude unit in meters
float Sep = 0; //Geoid Separation
char uSep = 'I'; //Units of Separation
float diffAge = 0; //Age of Differential Connections

//Port D Assignments LED Indicators
//D2               (status button)
//D3               (takeoff button)
//D4               RED LED (System)
//D5               GREEN LED (Takeoff)
volatile unsigned char* port_d = (unsigned char*)0x2B;
volatile unsigned char* ddr_d = (unsigned char*)0x2A;
volatile unsigned char* pin_d = (unsigned char*)0x29;
unsigned char enable = 0x00;
unsigned char prev_enable = 0x00;
//--------------------Main Setup and Loop----------------------------------
void setup() {
  Serial.begin(9600); //Setup GPS
  //setup_timer();
  setup_PORTD();//For button and LED
  setup_interrupt();  //For state changes on button presses
}
void loop() {
  ///delay(3000);//do work
  if (enable == 0x00)
    off_state();
  else if (enable == 0x01)
    status_state();
  else if (enable == 0x02)
    takeoff_state();
  else if (enable == 0x03)
    takeoffStatus_state();
}
//-------------------Finite States-----------------------------------
void off_state()
{
  Serial.println("Off");
  delay(1000);
}
void status_state()
{
  Serial.println("Status");
  delay(1000);
}
void takeoff_state()
{
  Serial.println("takeOff");
  delay(1000);
}
void takeoffStatus_state()//Takeoff + Status
{
  Serial.println("takeOff + status");
  delay(1000);
}
//------------Main functions-------------------------------------------
void status_toggle(){
  enable ^= 0x01;//Toggle status bit
  set_LED();//Set LED to match next state
}
void takeoff_toggle(){
  enable ^= 0x02;//toggle takeoff bit
  set_LED();//Set LED to match next state
}
void get_GPS() {
  if (Serial.available()) { // read the incoming byte:
    Byte = Serial.read();
    if (Byte == '$') { //start of new data
      inByte = Serial.readStringUntil('\n');//read until return
      if (inByte[3] == 'G')
        parseData();//parse data into buff
    }
  }
}
void parseData() {
  int j = 0;
  int k = 0;;
  for (int i = 6; i < inByte.length(); i++)
  {
    
    if (inByte[i] == ',')
    {
      j++;
      k = 0;
    }
    else
    {
      buff[j][k] = inByte[i];
      k++;
    }
  } 
  print_data();
}

void print_data()
{
  Serial.print("Latitude :");
  Serial.println(buff[1]);
  Serial.print("Longitude :");
  Serial.println(buff[3]);
  Serial.print("Altitude :");  
  Serial.println(buff[8]);
  clearBuff();
}

void set_LED() {
  if (enable == 0x00)
  {
    *port_d &= !0x30;//Turn off RED and GREEN
  }
  else if (enable == 0x01)
  {
    *port_d &= !0x10;//Turn off RED
    *port_d |= 0x20;//Turn on GREEN
  }
  else if (enable == 0x02)
  {
    *port_d &= !0x20;//Turn off GREEN 
    *port_d |= 0x10;//Turn on RED
  }
  else if (enable == 0x03)
  {
    *port_d |= 0x30;//Turn on GREEN and RED
  }
}
void clearBuff()
{
  for (int i= 0; i < 13; i++)
    for (int j = 0; j < 12; j++)
      buff[i][j] = 0;
}
//------------------Setup functions-------------------------------------
void setup_interrupt()
{
  attachInterrupt(0,status_toggle,RISING);//d2 status button
  attachInterrupt(1,takeoff_toggle,RISING);//d3 takeoff button
}
void setup_PORTD()
{
  *ddr_d |= 0x30; //d5, d4 set to Output (green 5), (red 4)
  *ddr_d &= 0xF3; //d2, d3 to input
}
