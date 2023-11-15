/*******************************************************************************
 * Wukong2040 Toy Boat 
 * Build a wifi controlled toy boat with Wukong2040 and Raspberry Pi Pico W
 * 
 * Please find more details at Hackster.io:
 * https://www.hackster.io/idreams/build-a-toy-boat-with-wukong2040-and-rpi-pico-w-365c8d
 * 
 ******************************************************************************/

// Include WiFi and Servo libraries
#include <WiFi.h>
#include <Servo.h>

 // Create a servo object
Servo myservo; 
 
// Servo GPIO pin
static const int servoPin = 5;
 
// GPIO the motors are attached to
static const int M1forwardPin = 20;
static const int M1backwardPin = 21;
 
// Network credentials
const char* ssid     = "idreams";
const char* password = "98765432";
 
// Web server on port 80 (http)
WiFiServer server(80);
 
// Variable to store the HTTP request
String header;
 
// Decode HTTP GET value
String ServoValueStr = String(5);
int pos1 = 0;
int pos2 = 0;

String SpeedValueStr = String(5);
int pos3 = 0;
int pos4 = 0;
 
// Current time
unsigned long currentTime = millis();

// Previous time
unsigned long previousTime = 0; 

// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
 
void setup() { 
  // Attach to servo and define minimum and maximum positions
  // Modify as required
  myservo.attach(servoPin, 600, 2400);
  
  // Start serial
  Serial.begin(115200);
  
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("Pico W is connected!");
  Serial.print("Assigned IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void forward(int SPEED) {
  analogWrite(M1forwardPin, SPEED);
  analogWrite(M1backwardPin, 0);
}

void backward(int SPEED) {
  analogWrite(M1backwardPin, SPEED);
  analogWrite(M1forwardPin, 0);
}

void stopMotors() {
  analogWrite(M1forwardPin, 0);
  analogWrite(M1backwardPin, 0);
}
 
void loop(){
  
  // Listen for incoming clients
  WiFiClient client = server.available();   
  
  // Client Connected
  if (client) {                             
    // Set timer references
    currentTime = millis();
    previousTime = currentTime;
    
    // Print to serial port
    Serial.println("New Client."); 
    
    // String to hold data from client
    String currentLine = ""; 
    
    // Do while client is connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
        
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
 
            // Display the HTML web page
            // HTML Header
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=0\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // CSS - Modify as desired
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto; }");
            client.println(".slider { -webkit-appearance: none; width: 300px; height: 25px; border-radius: 10px; background: #ffffff; outline: none;  opacity: 0.9;-webkit-transition: .2s;  transition: opacity .2s;}");
            client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; border-radius: 50%; background: #ff3410; cursor: pointer; }");
            client.println(".vertical-heighest-first { -webkit-transform:rotate(270deg); -moz-transform:rotate(270deg); -o-transform:rotate(270deg); -ms-transform:rotate(270deg); transform:rotate(270deg); }");
            client.println(".row-container {display: flex; flex-direction: row; justify-content: center; align-items: center;}");
            client.println(".button {width: 135px; border: 0; font-family: \"Trebuchet MS\"; font-weight: bold; background-color: #fff200; box-shadow: inset 2px 2px 3px #b2b2b2, inset -2px -2px 3px #000; padding: 10px 25px; text-align: center; display: inline-block; border-radius: 5px; font-size: 16px; margin: 10px 25px; cursor: pointer;}");
            client.println(".button:hover {background-color: #F1C40F;}");
            client.println(".button:active{box-shadow: inset 2px 2px 3px #000, inset -2px -2px 3px #b2b2b2;}</style>");
            
            // Get JQuery
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
   
            // Page background
            client.println("</head><body style=\"background-color:#00BFFF;\">");
            
            // Position display
            client.println("<h2 style=\"color:#ffffff;\">Servo Position: <span id=\"servoPos\"></span>&#176;</h2>"); 
                     
            // Slider control
            client.println("<input type=\"range\" min=\"70\" max=\"106\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\""+ServoValueStr+"\"/>");
            client.println("<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>");
            client.println("<input type=\"range\" min=\"50\" max=\"255\" class=\"slider vertical-heighest-first\" id=\"motorSlider\" onchange=\"motorVal(this.value)\" value=\""+SpeedValueStr+"\"/>");
            client.println("<br/><br/><br/><br/><br/><br/><br/><br/>");
            client.println("<h2 style=\"color:#ffffff;\">Throttle: <span id=\"MotorValue\"></span></h2>");
            client.println("<div class=\"row-container\"><button class=\"button\" id=\"reverse-btn\" onmousedown=\"motorVal(456)\" onmouseup=\"motorVal(456)\">Reverse</button>");
            client.println("<button class=\"button\" id=\"stop-btn\" onmousedown=\"motorVal(0)\" onmouseup=\"motorVal(0)\">STOP</button></div>");
							
            // Javascript
            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("var MotorsliderV = document.getElementById(\"motorSlider\");");
            client.println("var MotorV = document.getElementById(\"MotorValue\"); MotorV.innerHTML = MotorsliderV.value;");
            client.println("MotorsliderV.oninput = function() { MotorsliderV.value = this.value; MotorV.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000});");
            client.println("function servo(pos) {$.get(\"/?value=\" + pos + \"&\");}");
            client.println("function motorVal(speed) { $.get(\"/?Mvalue=\" + speed + \"&\");}");
            client.println("{Connection: close};</script>");
            // End page
            client.println("</body></html>");     
            	
            //------------------------------------
            // GET data
            if(header.indexOf("GET /?value=") >= 0) {
              pos3 = header.indexOf('=');
              pos4 = header.indexOf('&'); 
              // String with motor position
              ServoValueStr = header.substring(pos3+1, pos4);
              // Move servo into position
              myservo.write(ServoValueStr.toInt());
              // Print value to serial monitor
              Serial.print("Servo Pos = ");
              Serial.println(ServoValueStr); 
            } 
            
            //------------------------------------
            // GET data
            if(header.indexOf("GET /?Mvalue=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              SpeedValueStr = header.substring(pos1+1, pos2);
              int SpeedVal = SpeedValueStr.toInt();
			  
              if (SpeedVal <= 70){stopMotors();}
              else if (SpeedVal == 456){backward(130);}
              else {forward(SpeedVal);}
             
              // Print value to serial monitor
              Serial.print("Speed value = ");
              Serial.println(SpeedValueStr); 
            }         
            // The HTTP response ends with another blank line
            client.println();
            
            // Break out of the while loop
            break;
          
          } else { 
            // New lline is received, clear currentLine
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
