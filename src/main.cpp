#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#include <user_config.h>

// Import the config
const char* ssid = STA_SSID1;
const char* password = STA_PASS1;

// Declare the pins
const int BUTTON_PIN = 0;
const int RELAY_PIN = 12;
const int LED_PIN = 13;

// Create the adapter
WebThingAdapter* adapter;

// Create the device descriptions
const char* relayTypes[] = {"OnOffSwitch", "Load", nullptr};
// Create the Relay Device
ThingDevice relay("switch", BROADCAST_NAME, relayTypes);
// Create the OnOff Boolean property that we're going to attach to the device
ThingProperty relayOn("on", "Whether the relay is turned on", BOOLEAN, "OnOffProperty");

void setup() {
  Serial.begin(115200);

  //Let's setup the output pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Turn off the relay
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Turn off the led

  //Let's setup the input pin
  pinMode(BUTTON_PIN, INPUT);

  // Let's connect to the WiFi
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(STA_SSID1);
  Serial.println("\"");
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_SSID1, STA_PASS1);

  Serial.println("");

   // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  digitalWrite(LED_PIN, HIGH); // active low led

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(STA_SSID1);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, LOW); // LOW will turn on the LED

  // WebThing Adapter Setup
  adapter = new WebThingAdapter(BROADCAST_NAME, WiFi.localIP());

  // Attach the property to the device
  relay.addProperty(&relayOn);
  // Attach the device to the adapter
  adapter->addDevice(&relay);
  // Start the adapter
  adapter->begin();

  // Just some info for debugging... !!don't attach a serial reader while plugged into mains!!
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(relay.id);
}

void loop() {
  adapter->update(); // Make the adapter fetch/send updates to the GateWay

  // Get the value set by the gateway, and toggle the relay accordingly
  if (relayOn.getValue().boolean) {
      digitalWrite(RELAY_PIN, LOW); 
    } else {
      digitalWrite(RELAY_PIN, HIGH);
  }
}