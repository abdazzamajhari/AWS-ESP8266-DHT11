// https://github.com/HarringayMakerSpace/awsiot
// https://www.youtube.com/watch?v=28FS2qix2u4&t=0s

#include "DHT.h" 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "libb64/cdecode.h"
}

const char* ssid = "Ruang Rapat";
const char* password = "b55nj4y4";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "a2oh157fnrxmkx-ats.iot.us-east-1.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key, comment out the BEGIN and END 
// lines, add a quote character at the start of each line and a quote 
// and backslash at the end of each line:

// xxxxxxxxxx-certificate.pem.crt
const String certificatePemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDWjCCAkKgAwIBAgIVAKQh0qnRbj3NyEfQtMFu60o0EgztMA0GCSqGSIb3DQEB" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTAyMTgwMTQ0" \
"MDZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDLm6yFW48tn1QvPZQa" \
"pBcasKAlkPqj+njyKKS1jPrXFQxfIJxGYAiiHN3ZDut6gMyzJlAU96j192New1/c" \
"roLjiwuqV6gKwSk8fMhZqxmKP6MEJb/IOTe8p0I88jQ3ba94UC862txaAYIVd9Fm" \
"3PrJweRfAoRL93e+tErQqItmT1Lm5CF46wRoF4iUL4bikAc1w8FejaQgErcUM5PD" \
"Byf1yB/sW3Ddy5SshHFU5a+NFBVRPDN7EuI9IyeVC1wSY0l0Xjf4a3JVHR2/CdCE" \
"w3C5rMzmDghvQMnKj3rgG/SGd4QXRq7MMQRIirr9oOVSjl5hqapdKOfAkoKNUIzu" \
"zJJ3AgMBAAGjYDBeMB8GA1UdIwQYMBaAFAsbh3LN4zdlH7MHCjU0KjFG3THLMB0G" \
"A1UdDgQWBBSALEuOy+hWvDpUxYemaHy8zMBQUzAMBgNVHRMBAf8EAjAAMA4GA1Ud" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAw4nNAsSj5VyuwdVy80+jjhDP" \
"vU0YkHWSubvabcT4Ci/ejEyXK9eqf98acrw+w0LjdK/Ydf4V+9ybILY8OamKuY4w" \
"KWQ97EQ11ML3L7PQa8eRrt7uJzL3CoWAkBGByR1b8D2jVP3Yk53P1M3joL/WApDy" \
"/OFR+Goy3O4hI7LPjKY0Oge3rXxAlzi0GLt5MNJynHE9q57/78gp6RR6BSUu5sDF" \
"RpoP8ANzhMuFK4tvlFm2RcO/RBQ65IlSHnuallghriTrX2UaRa8nyyKll0hmQmPN" \
"Wc7iFJ4emeCLH3L9bdNPFvCPafrHJCSQiXiok0k7o20/A9cBtKJbGfWfZiGAMQ=="; 
//-----END CERTIFICATE-----

// xxxxxxxxxx-private.pem.key
const String privatePemKey = \
//-----BEGIN RSA PRIVATE KEY-----
"MIIEowIBAAKCAQEAy5ushVuPLZ9ULz2UGqQXGrCgJZD6o/p48iiktYz61xUMXyCc" \
"RmAIohzd2Q7reoDMsyZQFPeo9fdjXsNf3K6C44sLqleoCsEpPHzIWasZij+jBCW/" \
"yDk3vKdCPPI0N22veFAvOtrcWgGCFXfRZtz6ycHkXwKES/d3vrRK0KiLZk9S5uQh" \
"eOsEaBeIlC+G4pAHNcPBXo2kIBK3FDOTwwcn9cgf7Ftw3cuUrIRxVOWvjRQVUTwz" \
"exLiPSMnlQtcEmNJdF43+GtyVR0dvwnQhMNwuazM5g4Ib0DJyo964Bv0hneEF0au" \
"zDEESIq6/aDlUo5eYamqXSjnwJKCjVCM7sySdwIDAQABAoIBACEjAt7pWdII/kB5" \
"+yKi1gfH+yhiAgix+QEgUD4j5acfi9Xnjsd9VapcnFZ5MSXM1IPLlnjzHZJ6YrfH" \
"gFCALlkuBgYBIh4t9bIcdNbzG8L+PF+ubRn/Be00I3mbyZfDyC0Cj806VWZonG02" \
"BOSCzmYLokk8NZ/PdLOa0BpJc/aAZBuEcbj4svWrfQtCOccV1fOJMO2SaA5LylGq" \
"IEXzj/+KnDZB2flOk5aMa49QbBmKSThMXJ2AQZh9DSDrldNEP7E9SkGfLxpBqLKW" \
"Nfoy9mb/kpHA+jvc7qmtqIlqEL2jlOsWQpYKMIPAjgwkj/GWijQsRurHEmwqGVXq" \
"cizioOECgYEA6vCDsSkDfuQ8gnqWVlEgFTz/TG7xOsLBIH6+xMKnnNWbJqDQnueW" \
"8bX5DkKA+B3v9Dz7GDWNj9yaRmlJ8vckHK66i67sFXqK6WrPqt43F68cVvn2yj0p" \
"N33NOmaE4dMQn3tbSIYxjhcXAOpPOLS6Gr+DmYYYaVPb4o74ITJ5qp8CgYEA3dwn" \
"YaMhp6LnGnu760SwlJKvhC7AOxKhNd7wLYR7Pg3rgO5FWrHJwqqB7YTVP+hW5ezm" \
"4xA3b0HMF8yMZbnMhU1FsfpCC6Vp2+bOdnpQnh5XlmNv56BHaviBfZrjxzL8dPxs" \
"3xb7/f68N1i3fI/ZQvG544xGqRHUyGlltyoWYSkCgYEA6ieIuXttFn0XqdvQFnuI" \
"hGJk+KLiHnSY03A/Zg+zymZC2j8Rbi7XjFSXw5SYwbHkxAI7KCfHzYhK4r5xi4yy" \
"GzwyhKF0fvvBk153V49eTWaj0jQTugE3IS1q7DfdXM1mFe12T1Qf88Qjky/0gUnu" \
"6lXSLaMulVuVHbrspvmNtXUCgYA02A1hSjPn8/tL6ZD3FuNgYBgCSghriaJwbGuU" \
"y+t2+mXPzfEjCxndjjNoWa1ZjYO1PFo6V1B85ZUxTf1a9Yhrb95b6Bn3IC04vZ8L" \
"xVKSdvOx1HmBGEIqjop5Wn1KozBSxRVBhf37n8NBA5agyrqbIWx/cJgcCS0/lyst" \
"F49BEQKBgFVVT8P1NDCpREI9peR147pw67F7w/ULIPX4mhrZrMgDIqelGDfbp7Bn" \
"OEUNeJavIE1YXbmZW2gO7vQAWmTflw83y4zC2bv4DAsUJKT7nkZC6V946BSVhGJ6" \
"SMKOQacXitOOLzKv5uG7noMpBuUPOcaE8L0JvNuOz7KKjK7J10E3";
//-----END RSA PRIVATE KEY-----

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
const String caPemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy" \
"rqXRfboQnoZsG4q5WTP468SQvvG5";
//-----END CERTIFICATE-----

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

#define DHTPIN D1 
#define DHTTYPE DHT11  

DHT dht(DHTPIN, DHTTYPE);  

void setup(){   
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  uint8_t binaryCert[certificatePemCrt.length() * 3 / 4];
  int len = b64decode(certificatePemCrt, binaryCert);
  wiFiClient.setCertificate(binaryCert, len);
  
  uint8_t binaryPrivate[privatePemKey.length() * 3 / 4];
  len = b64decode(privatePemKey, binaryPrivate);
  wiFiClient.setPrivateKey(binaryPrivate, len);

  uint8_t binaryCA[caPemCrt.length() * 3 / 4];
  len = b64decode(caPemCrt, binaryCA);
  wiFiClient.setCACert(binaryCA, len);
  
  dht.begin(); 
} 

unsigned long lastPublish;
int msgCount;

void loop() {    
  Serial.println(" ====== DHT11 SENSOR ====== ");      
  float hum = dht.readHumidity();   
  float tem = dht.readTemperature();   
  Serial.print(" Humidity DHT11: ");   
  Serial.print(hum);   
  Serial.print("% ");   
  Serial.print("\n");   
  Serial.print(" Temperature DHT11: ");   
  Serial.print(tem);   
  Serial.print("°C ");   
  Serial.print(">>> ");   
  Serial.print((int)round(1.8*tem+32));   
  Serial.println("°F ");    

   pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Temperature sensor: ") +tem + "°C >>> Humidity is " +hum + "%" + ++msgCount;
    pubSubClient.publish("outTopic", msg.c_str()); //outTopic as a MQTT Client Test
    Serial.print("Published: "); Serial.println(msg);
    lastPublish = millis();
  }

  delay(10000); //waiting for 10 seconds
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

int b64decode(String b64Text, uint8_t* output) {
  base64_decodestate s;
  base64_init_decodestate(&s);
  int cnt = base64_decode_block(b64Text.c_str(), b64Text.length(), (char*)output, &s);
  return cnt;
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
