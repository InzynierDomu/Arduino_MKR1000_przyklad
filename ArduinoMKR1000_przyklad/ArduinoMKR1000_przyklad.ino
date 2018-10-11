#include <WiFi101.h>
const int PRZYCISK = 7;
const int LED = 6;

char ssid[] = "test";     // Nazwa sieci (ssid)
char pass[] = "test";     // shalo sieci 

String readString;

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  pinMode(PRZYCISK, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial) {
  }

  // Łączenie się sieci WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Łączenie się do SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    // czekam 10s na połączenie
    delay(10000);
  }
  server.begin();
  // po połączeniu wywołuje się wyświetlenie stanu wifi
  printWiFiStatus();
}


void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        readString += c;
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  
          client.println("Refresh: 5");  //strona aktualizuje się co 5s
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          client.print("<h3>Przyklad dla Arduino MKR1000</h3> <br/>");
          
          client.print("Przycisk w stanie: ");
          int przycisk = digitalRead(7);
          client.print(przycisk);
          client.println("<br />");

          client.println("<input type=button value='LED wlacz' onmousedown=location.href='/?LEDON'><br/>");
          client.println("<input type=button value='LED wylacz' onmousedown=location.href='/?LEDOFF'><br/>");

          client.print("<br/><a href='http://www.inzynierdomu.pl/'>Przygotowane przez inzynier Domu</a>");
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);

    //zamkniecie przycisku
    client.stop();

    //sprawdzamy czy zostaly wykonane akcje z pod przyciskow
    if (readString.indexOf("/?LEDON") > 0) {
      Serial.println();
      Serial.println("LED turn on");
      digitalWrite(LED, HIGH);
      Serial.println();
    }
    if (readString.indexOf("/?LEDOFF") > 0) {
      Serial.println();
      Serial.println("LED turn off");
      digitalWrite(LED, LOW);
      Serial.println();
    }
    readString = "";
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // z jaką siecią się połączono
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Adres IP
  IPAddress ip = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(ip);

  //pomiar sygnału
  long rssi = WiFi.RSSI();
  Serial.print("siła sygnału (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
