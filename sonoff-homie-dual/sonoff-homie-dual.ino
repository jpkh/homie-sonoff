#include <Homie.h>
#include <ArduinoOTA.h>

#define PIN_LED     13      // PIN 12 / HSPI_MOSI; UART0_CTS MTCK

/**
  * Arduino IDE instructions
  *
  * select "generic ESP8266 Module"
  * 80Mhz CPU, 40Mhz Flash
  * Upload Speed 460800
  * 1M (64K SPIFFS)
  * ck
  *
*/

#define FW_NAME "sonoff-dual"
#define FW_VERSION "1.0.0"

boolean relay1 = false;
boolean relay2 = false;

HomieNode relayNode1("switch1", "switch");
HomieNode relayNode2("switch2", "switch");

void setRelays() {
  byte b = 0;
  if (relay2) b++;
  if (relay1) b += 2;
 
  Serial.write((byte)0xA0);
  Serial.write((byte)0x04);
  Serial.write((byte)b);
  Serial.write((byte)0xA1);;
  
  Serial.flush();
}

bool relayHandler1(const HomieRange& range, const String& value) {
  if (value != "true" && value != "false") return false;

  relay1 = (value == "true");

  setRelays();

  relayNode1.setProperty("on").send(value);
  return true;
}

bool relayHandler2(const HomieRange& range, const String& value) {
  if (value != "true" && value != "false") return false;

  relay2 = (value == "true");

  setRelays();
   
  relayNode2.setProperty("on").send(value);
  return true;
}

void setup() { 
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie_setBrand(FW_NAME);
  Homie.disableLogging();
  relayNode1.advertise("on").settable(relayHandler1);
  relayNode2.advertise("on").settable(relayHandler2);
  
  Homie.setLedPin(PIN_LED, LOW);
  Homie.setup();
  Serial.begin(19200);
  ArduinoOTA.setHostname(Homie.getConfiguration().deviceId);
  ArduinoOTA.begin();
}

void loop() {
  Homie.loop();
  ArduinoOTA.handle();
}
