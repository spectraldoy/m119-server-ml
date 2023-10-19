#include <WiFiNINA.h>
#include <SPI.h>
#include <Arduino_LSM6DS3.h>
#include "secrets.h"

// setup
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

int status = WL_IDLE_STATUS;
char server[] = "hwsrv-1103953.hostwindsdns.com";
int port = 80;

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 5L * 1000L; // delay between updates, in milliseconds

WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // If IMU fails, just loop infinitely
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    while (true);
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    exit(1);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    Serial.println("Waiting a few seconds for connection:");
    for (int i = 1; i <= 5; i++) {
      Serial.print(i);
      Serial.print("...");
      delay(1000);
    }
    Serial.println();
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
}


void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval && IMU.accelerationAvailable()) {
    httpRequest();
  }
}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();
  Serial.println();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);
    Serial.println("Requesting direction given:");
    Serial.print("ax=");
    Serial.print(ax);
    Serial.print(" and ay=");
    Serial.print(ay);
    Serial.print(" and az=");
    Serial.println(az);

    Serial.println("connecting...");
    // Make a HTTP request:
    client.print("GET /query?ax=");
    client.print(ax);
    client.print("&ay=");
    client.print(ay);
    client.print("&az=");
    client.print(az);
    client.println(" HTTP/1.1");
    client.println("Host: hwsrv-1103953.hostwindsdns.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
