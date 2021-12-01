
// Load Wi-Fi library
#include <WiFi.h>
#include <Stepper.h>

// Define number of steps per rotation:
const int stepsPerRevolution = 2048;
Stepper pStepper = Stepper(stepsPerRevolution, 5, 23, 18, 19);//heading
Stepper hStepper = Stepper(stepsPerRevolution, 13, 14, 12, 27); //pitch
const int laserPin = 21;

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";//"FRITZ!Box Fon WLAN 7360";//
const char* password = "123456789";//"59267746303957006813";//"123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
String output28State = "off";


void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(laserPin, OUTPUT);
  toggleLaser(false);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
  hStepper.setSpeed(10);
  pStepper.setSpeed(10);
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /28/on") >= 0) {
              output28State = "on";
              toggleLaser(true);
            } else if (header.indexOf("GET /28/off") >= 0) {
              output28State = "off";
              toggleLaser(false);
            } else if (header.indexOf("GET /Up") >= 0) {
              pitch(100);
            } else if (header.indexOf("GET /Down") >= 0) {
              pitch(-100);
            } else if (header.indexOf("GET /Left") >= 0) {
              heading(-100);
            } else if (header.indexOf("GET /Right") >= 0) {
              heading(100);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #FF0000;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>LaserCatDisco</h1>");
            
            client.println("<p><a href=\"/Down\"><button class=\"button\">Down</button></a></p>");
            client.println("<p><a href=\"/Up\"><button class=\"button\">Up</button></a></p>");

            client.println("<p><a href=\"/Left\"><button class=\"button\">Left</button></a></p>");
            client.println("<p><a href=\"/Right\"><button class=\"button\">Right</button></a></p>");
            

            // Display current state, and ON/OFF buttons for Laser
            client.println("<p>Laser - State " + output28State + "</p>");
            // If the output27State is off, it displays the ON button
            if (output28State == "off") {
              client.println("<p><a href=\"/28/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/28/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void toggleLaser(bool state){
  if (state)
    digitalWrite(laserPin, HIGH);
  else
    digitalWrite(laserPin, LOW);
}
