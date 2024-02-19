#include <Arduino.h>
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

struct pins
{
  const char *pin__name;
  byte pin;
  byte pin__mode;
  byte pin__status;
};
// name, pin, in/out, status
struct pins relay__pin[] = {{"Relay__pin_D1", 5, 1, 0}, {"Relay__pin_D2", 4, 1, 0}}; // INPUT = 0, OUTPUT = 1
byte pins__count = sizeof(relay__pin) / sizeof(relay__pin[0]);

// Create AsyncWebServer object
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

void handleRoot(); // function prototypes for HTTP handlers
void handleNotFound();

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 15);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

const char *location = "nodemcu2";
const char *ssid = "electrical";
const char *wifi_password = "electro2021";

// uint8_t pin__test = D1;

void setup_pins()
{
  delay(10);
  for (byte i = 0; i < pins__count; i++)
  {
    pinMode(relay__pin[i].pin, relay__pin[i].pin__mode);
    digitalWrite(relay__pin[i].pin, relay__pin[i].pin__status);
  }
}

void setup_wifi()
{
  delay(10);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void relay(AsyncWebServerRequest *request)
{
  int paramsNr = request->params();
  for (int i = 0; i < paramsNr; i++)
  {
    AsyncWebParameter *p = request->getParam(i);
    String temp__name = p->name();
    const char *name = temp__name.c_str();
    int value = p->value().toInt();
    for (byte i = 0; i < pins__count; i++)
    {
      if (strcmp(name, relay__pin[i].pin__name) == 0)
      {
        digitalWrite(relay__pin[i].pin, value);
        relay__pin[i].pin__status = digitalRead(relay__pin[i].pin);
        request->send(200, "text/html", String(relay__pin[i].pin__status));
        String temp__send = "{";
        temp__send += '"';
        temp__send += relay__pin[i].pin__name;
        temp__send += '"';
        temp__send += ":";
        temp__send += relay__pin[i].pin__status;
        temp__send += "}";
        events.send(String(temp__send).c_str(), "change", millis());
      }
      // else
      // {
      //   request->send(200, "text/html", String("Error"));
      // }
    }
  }
}

void get__html(AsyncWebServerRequest *request)
{
  String pins__info = "{";
  for (struct pins *p = relay__pin; p < relay__pin + pins__count; p++)
  {
    if (pins__count > 0)
    {
      pins__info += '"';
      pins__info += p->pin__name;
      pins__info += '"';
      pins__info += ":{";
    }
    pins__info += '"';
    pins__info += "name";
    pins__info += '"';
    pins__info += ':';
    pins__info += '"';
    pins__info += p->pin__name;
    pins__info += '"';
    pins__info += ",";
    pins__info += '"';
    pins__info += "pin";
    pins__info += '"';
    pins__info += ":";
    pins__info += p->pin;
    pins__info += ",";
    pins__info += '"';
    pins__info += "mode";
    pins__info += '"';
    pins__info += ":";
    pins__info += p->pin__mode;
    pins__info += ",";
    pins__info += '"';
    pins__info += "status";
    pins__info += '"';
    pins__info += ":";
    pins__info += p->pin__status;
    if (pins__count > 0)
      pins__info += "}";
    if (pins__count > 0 && p == relay__pin + (pins__count - 2))
      pins__info += ",";
  }
  pins__info += "}";
  request->send(200, "text/html", String(pins__info));
}

void get__info_pin(AsyncWebServerRequest *request)
{
  String res = "{";
  for (byte i = 0; i < pins__count; i++)
  {
    res += '"';
    res += relay__pin[i].pin__name;
    res += '"';
    res += ':';
    res += relay__pin[i].pin__status;
    if (i != pins__count - 1)
    {
      res += ',';
    }
  }
  res += "}";
  request->send(200, "text/html", String(res));
}

void setup()
{
  // Setup wifi
  Serial.begin(115200);
  setup_pins();
  setup_wifi();
  Serial.print("\n\n");
  Serial.print("Location: ");
  Serial.println(location);

  // Initialze LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("An error has occurred while mounting LittleFS");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html"); });

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/index.html", "text/plain"); });

  server.on("/styles/reset.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles/reset.css", "text/css", false); });
  server.on("/styles/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles/style.css", "text/css", false); });
  server.on("/scripts/app.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/scripts/app.js", "text/plain", false); });

  // server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request)
  //           {
  //             int paramsNr = request->params();
  //             // String commands = String();
  //             // String value = String();
  //             int commands = 0;
  //             int value = 0;
  //             for(int i=0;i<paramsNr;i++){
  //                 AsyncWebParameter* p = request->getParam(i);
  //                 commands = p->name().toInt();
  //                 value = p->value().toInt();
  //                 commands += p->name();
  //                 commands += '=';
  //                 commands += p->value();
  //                 commands += ';';
  //                 Serial.print("Param name: ");
  //                 Serial.println(p->name());
  //                 Serial.print("Param value: ");
  //                 Serial.println(p->value());
  //                 Serial.println("------");
  //             }
  //             byte result = test(commands, value);
  //             request->send(200, "text/plain", String(result)); });

  server.on("/relay", HTTP_GET, relay);
  server.on("/get__html", HTTP_GET, get__html);
  server.on("/get__info_pin", HTTP_GET, get__info_pin);

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }

    client->send("hi!", NULL, millis(), 10000); });

  server.addHandler(&events);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
}
