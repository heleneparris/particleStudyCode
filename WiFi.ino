// WiFi Functions


void openWiFi(){
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {delay(1000); Serial.print(".");} 
  Serial.println("Connected!");
  digitalWrite(ONBOARD_LED,HIGH);
  delay(100);
  digitalWrite(ONBOARD_LED,LOW);
}
void closeWiFi(){WiFi.disconnect();}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverName);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  
  if (httpResponseCode>0) {Serial.println("Connected"); payload = http.getString();}
  else {Serial.println("Not Connected");}
  http.end();

  return payload;
}
