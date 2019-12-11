//**************************************
//*** BEGIN CLASS REDESP 
//**************************************

#include "RedESP.h"

RedESP::RedESP() {
	status = WL_IDLE_STATUS;
}

void RedESP::configure(char *strSSID, char* strPWD, IPAddress IPA, int port = 80) {
	buf.configure(8);
	server.configure(port);
	IPserver = IPA;
	staticIP = true;
	sSSID = strSSID;
	sPWD = strPWD;
}

void RedESP::configure(char *strSSID, char* strPWD, int port = 80) {
	buf.configure(8);
	server.configure(port);
	staticIP = false;
	sSSID = strSSID;
	sPWD = strPWD;
}

bool RedESP::begin() {
	bool connectingWIFI = true;
	bool statWIFI;
	int tries = 0;

	Serial3.begin(115200);    // initialize serial for ESP module
	WiFi.init(&Serial3);    // initialize ESP module

	if (staticIP)
	  WiFi.config(IPserver);

	WiFi.begin(sSSID, sPWD);

	// Wait for connection
	/*
	Serial.println("Trying Main WiFi");
	while ((WiFi.status() != WL_CONNECTED) && (tries < 10)) {
	  delay(500);
	  Serial.print(".");
	  tries++;
	}
	Serial.println();
	if (tries >= 25) {
	  Serial.println("Too many trials, no WiFi connection was possible");
	} else {
	  Serial.println("");
	  Serial.print("Connected to ");
	  Serial.println(ssid);
	  Serial.print("IP address: ");
	  Serial.println(WiFi.localIP());

	  Serial.println("Network OK");
	 
	//    if (MDNS.begin("papilloniot-TP_Ext1")) {
	//      Serial.println("MDNS responder started");
	//    }
	*/     
	if (WiFi.status() == WL_CONNECTED) {
	  Serial.println("Network OK");
	  Serial.print("Connected to ");
	  Serial.println(sSSID);
	  Serial.print("IP address: ");
	  Serial.println(WiFi.localIP());
	  server.begin();
	  Serial.println("HTTP server started");
	  statWIFI = true;
	} else {
	  Serial.println("Network Error");
	  statWIFI = false;
	}
	return statWIFI;
}

void RedESP::update(String *sPayload) 
{
	bool enviarDatos;

	// Handle Web Clients
	WiFiEspClient client = server.available();  // listen for incoming clients

	if (client) {                               // if you get a client,
	  enviarDatos = false;
	//      Serial.println("New client");             // print a message out the serial port
	  buf.init();                               // initialize the circular buffer
	  while (client.connected()) {              // loop while the client's connected
	    if (client.available()) {               // if there's bytes to read from the client,
	      char c = client.read();               // read a byte, then
	      buf.push(c);                          // push it to the ring buffer

	      // printing the stream to the serial monitor will slow down
	      // the receiving of data from the ESP filling the serial buffer
	//        Serial.write(c);
	      
	      // that's the end of the HTTP request, so send a response
	      if (buf.endsWith("\r\n\r\n")) {
	//          Serial.println("Enviar respuesta");
	        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
	        // and a content-type so the client knows what's coming, then a blank line:
	        client.println("HTTP/1.1 200 OK");
	        client.println("Content-type:text/plain");
	        client.println();  
	        if (enviarDatos) {
	          client.println(*sPayload);
	        } else {
	          client.println("COMANDO INCORRECTO");
	        }
	        // The HTTP response ends with another blank line:
	        client.println();
	        break;
	      }
	      if (buf.endsWith("GET /D")) {
	        enviarDatos = true;
	        Serial.println("Enviar datos");
	      }
	    }
	  }
	  
	  // close the connection
	  client.stop();
	  Serial.println("Client disconnected");
	}
}

//**************************************
//*** END CLASS REDESP 
//**************************************
