void WirelessConnect::serialOut(String output){

  if (SERIAL_OUTPUT )Serial.print(output);
}

WirelessConnect::WirelessConnect(void){
  int a = 0;
}

WirelessConnect::WirelessConnect(String cwName, String cwPassword){
  WirelessConnect output;

  wifi_con.begin(115200);

  String readings;
  
  output.serialOut("-------Program Starts----------\n");
  output.serialOut("       Checking connection     \n");
  delay(100);
  wifi_con.println(AT_OK);
  delay(300);
  if (wifi_con.available()){

      readings = wifi_con.readString();
           
      if( readings.length() == (int)RESPONSE){

        String connectionParam = (String)CONNECT+"="+cwName+","+cwPassword;
        wifi_con.println(connectionParam);
        output.serialOut("   Trying to connect... to: "+connectionParam);
        delay(1500);
        readings = wifi_con.readString();
        if (readings.length() == (int)RESPONSE){
          output.serialOut("   Connected successfully    ");
          
        }else{
          output.serialOut(readings);
          output.serialOut("   Wrong password or ssid    ");
        }
        
      }

    
  }else{
    if (SERIAL_OUTPUT) Serial.println("NoConnection"); 

  }
  
}

