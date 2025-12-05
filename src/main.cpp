#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include "secrets.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

float currentTemp = 0.0;
float currentHum = 0.0;

Adafruit_AHTX0 aht;

WebServer server(80);

WiFiUDP ntpUDP;
// UTC+8 (Singapore) offset = 8 * 3600 seconds
NTPClient timeClient(ntpUDP, "pool.ntp.org", 8 * 3600, 60000); 

String buildHtmlPage()
{
    String html =
        "<!DOCTYPE html><html><head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<meta http-equiv='refresh' content='5'>"
        "<title>Room Temp & Humidity</title>"
        "<style>body{font-family:Arial,sans-serif;padding:20px;background:#111;color:#eee;} "
        ".card{max-width:400px;margin:auto;padding:20px;border-radius:12px;background:#222;"
        "box-shadow:0 0 10px rgba(0,0,0,0.5);} h1{font-size:1.6rem;margin-bottom:0.5rem;} "
        ".temp{font-size:2.5rem;} .hum{font-size:1.8rem;color:#8fd3ff;}</style>"
        "</head><body><div class='card'>"
        "<h1>Room Environment</h1>";

    html += "<div class='time'>" "Time: " + timeClient.getFormattedTime() + " SGT</div>";
    html += "<br>";
    html += "<div class='temp'>" + String(currentTemp, 2) + "&deg;C</div>";
    html += "<div class='hum'>" + String(currentHum, 1) + "% RH</div>";
    html += "<p>Auto-refreshing every 5 seconds.</p>";
    html += "</div></body></html>";

    return html;
}

void handleRoot()
{
    server.send(200, "text/html", buildHtmlPage());
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("AHT20 + ESP32 WiFi monitor");

    // --- WiFi connect ---
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());

    // --- NTP Client for time recording ---
    timeClient.begin();
    Serial.println("Syncing NTP time...");
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
    Serial.print("Current time: ");
    Serial.println(timeClient.getFormattedTime());

    // --- I2C + Sensor ---
    Wire.begin(21, 22);
    if (!aht.begin())
    {
        Serial.println("❌ Sensor not found, check wiring!");
        while (1)
            delay(500);
    }
    Serial.println("✅ AHT20 detected.");

    // --- Web server routes ---
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started.");
    // CSV header for recordings (timestamp,temp_C,humidity_RH)
    Serial.println("timestamp,temp_C,humidity_RH");
}

void loop()
{
    static unsigned long lastRead = 0;
    unsigned long now = millis();

    // Update sensor every 5 seconds
    if (now - lastRead > 5000) {
        lastRead = now;

        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);

        currentTemp = temp.temperature;
        currentHum = humidity.relative_humidity;

        timeClient.update();

        // Print CSV formatted line with full date+time (YYYY-MM-DD HH:MM:SS,temp,hum)
        unsigned long epoch = timeClient.getEpochTime();
        time_t rawTime = (time_t)epoch;
        struct tm *ptm = gmtime(&rawTime); // epoch already adjusted with timeClient offset
        char timeBuf[20];
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", ptm);
        String fullstamp = String(timeBuf);

        Serial.print(fullstamp);
        Serial.print(",");
        Serial.print(currentTemp, 2);
        Serial.print(",");
        Serial.println(currentHum, 1);
    }

    // Handle incoming HTTP requests
    server.handleClient();
}
