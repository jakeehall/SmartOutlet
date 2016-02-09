#include <SPI.h>
#include <WiFi.h>

char ssid[] = "ACM";      //SSID
char pass[] = "Bob Is Steve";   //Password
static int digitalPin = 8;

WiFiServer server(80);

void setup() {
  pinMode(digitalPin, OUTPUT);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // wait for serial port to connect. Needed for Leonardo only
  while (!Serial) {;}
  
  int status = WL_IDLE_STATUS;
  //make sure the wifi shield is connected
  while(WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not connected!");
    delay(10000);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(7000); //wait 7 seconds to connect
  }
  server.begin(); //start server
  printWifiStatus(); //print connection information
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client Connected");
    boolean currentLineIsBlank = true;
    String buffer = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        buffer+=c;
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Smart Outlet</title></head>");
          client.println("<body>");
          if (digitalRead(digitalPin)) {
            client.print("The Smart Outlet is <font color='green'>ON</font>.");
          } else {
            client.print("The Smart Outlet is <font color='red'>OFF</font>.");
          }
          client.println("<br/>");
          client.print("<FORM action=\"http://");
          client.print(WiFi.localIP());
          client.print("/\" >");
          client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"1\">ON");
          client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"0\">OFF");
          client.print("<P> <INPUT type=\"submit\" value=\"Submit\"></FORM>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          buffer = "";
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          if(buffer.indexOf("GET /?status=1")>=0) {
            digitalWrite(digitalPin, HIGH);
          } else if(buffer.indexOf("GET /?status=0")>=0) {
            digitalWrite(digitalPin, LOW);
          }
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client Disonnected");
  }
}


void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("signal strength (RSSI):");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}
