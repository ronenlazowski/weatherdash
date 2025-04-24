#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"
#define OPENWEATHERMAP_API_KEY "API_KEY"
#define COUNTRY_CODE "US"
#define WEATHER_CHECK_INTERVAL 3600000

unsigned long lastWeatherCheckMillis = 0;
String lastWeatherReport = "Waiting for weather data...";
String currentCity = "Boston";
WebServer server(80);

String urlEncode(const String &str) {
  String encodedStr = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (c == ' ') {
      encodedStr += "%20";
    } else {
      encodedStr += c;
    }
  }
  return encodedStr;
}

void fetchWeather() {
  String url = "https://api.openweathermap.org/data/2.5/weather?q=" +
                urlEncode(currentCity) + "," + COUNTRY_CODE +
                "&appid=" + String(OPENWEATHERMAP_API_KEY) + "&units=imperial";

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      float temp = doc["main"]["temp"];
      float feels = doc["main"]["feels_like"];
      float humidity = doc["main"]["humidity"];
      const char* condition = doc["weather"][0]["description"];
      int weatherId = doc["weather"][0]["id"];

      String conditionCapitalized = String(condition);
      conditionCapitalized[0] = toupper(conditionCapitalized[0]);
      for (int i = 1; i < conditionCapitalized.length(); i++) {
        if (conditionCapitalized[i - 1] == ' ') {
          conditionCapitalized[i] = toupper(conditionCapitalized[i]);
        }
      }

      String weatherEmoji = "🌤️";
      if (weatherId >= 200 && weatherId <= 232) weatherEmoji = "🌩️";
      else if (weatherId >= 300 && weatherId <= 321) weatherEmoji = "🌧️";
      else if (weatherId >= 500 && weatherId <= 531) weatherEmoji = "🌧️";
      else if (weatherId >= 600 && weatherId <= 622) weatherEmoji = "🌨️";
      else if (weatherId >= 700 && weatherId <= 762) weatherEmoji = "🌫️";
      else if (weatherId == 800) weatherEmoji = "🌞";
      else if (weatherId == 801) weatherEmoji = "🌤️";
      else if (weatherId == 802) weatherEmoji = "⛅";
      else if (weatherId == 803 || weatherId == 804) weatherEmoji = "☁️";

      lastWeatherReport =
        String("<h2>Weather Update 🌤️</h2>") +
        "<p><strong>Condition:</strong> " + conditionCapitalized + " " + weatherEmoji + "</p>" +
        "<p><strong>Temperature:</strong> " + String(temp, 1) + "°F 🌡️</p>" +
        "<p><strong>Feels Like:</strong> " + String(feels, 1) + "°F 🌬️</p>" +
        "<p><strong>Humidity:</strong> " + String(humidity) + "% 💧</p>";

      if (temp > 85) {
        lastWeatherReport += "<p>☀️ <strong>It's HOT! Stay cool!</strong> 🌞</p>";
      } else if (temp > 70) {
        lastWeatherReport += "<p>🌞 <strong>Perfect Weather! Enjoy the day!</strong> 😊</p>";
      } else if (temp > 50) {
        lastWeatherReport += "<p>🌤️ <strong>It's a bit chilly. Bundle up!</strong> 🧥</p>";
      } else {
        lastWeatherReport += "<p>❄️ <strong>It's COLD! Stay warm!</strong> 🧣</p>";
      }

    } else {
      lastWeatherReport = "<p>Error: Could not parse weather data. 😞</p>";
    }
  } else {
    lastWeatherReport = "<p>Error: HTTP request failed. 🚫</p>";
  }

  http.end();
  lastWeatherCheckMillis = millis();
}

String generateDashboard() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>🌦️ Weather Dashboard 🌈</title>";
  html += "<link rel=\"shortcut icon\" href=\"https://i.imgur.com/nEtSigP.png\" type=\"image/x-icon\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f0f8ff; color: #333; text-align: center; padding: 20px; margin: 0;}";
  html += "h1 { color: #008cba; font-size: 48px; }";
  html += "h2 { color: #ff6347; font-size: 32px; }";
  html += "p { font-size: 20px; line-height: 1.6; color: #ff6347; }";
  html += "button { padding: 10px 20px; font-size: 16px; background-color: #ff4500; color: white; border: none; border-radius: 5px; cursor: pointer; }";
  html += "button:hover { background-color: #e03e00; }";
  html += ".card { background-color: #fff; border-radius: 10px; padding: 20px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); margin: 10px auto; max-width: 600px; }";
  html += ".footer { margin-top: 20px; font-size: 14px; color: #666; }";
  html += ".loading { display: none; font-size: 24px; color: #ff6347; }";
  html += ".city-form { margin-top: 20px; padding: 15px; }";
  html += ".city-form label { font-size: 18px; margin-right: 10px; color: #ff4500; font-weight: bold; }";
  html += ".city-form select { font-size: 16px; padding: 8px 12px; border: 2px solid #ff4500; border-radius: 5px; background-color: #fff8f0; color: #333; cursor: pointer; transition: all 0.3s ease; }";
  html += ".city-form select:hover { background-color: #ffe0cc; border-color: #e03e00; }";
  html += "</style><script>";
  html += "function showLoading() { document.getElementById('loading').style.display = 'inline'; document.getElementById('refreshButton').disabled = true; }";
  html += "function hideLoading() { document.getElementById('loading').style.display = 'none'; document.getElementById('refreshButton').disabled = false; }";
  html += "</script></head><body>";
  html += "<h1>🌞 ESP32 Weather Dashboard 🌦️</h1>";
  html += "<div class='card'>";
  html += lastWeatherReport;
  html += "<br><form action=\"/refresh\" method=\"get\" onsubmit=\"showLoading()\">";
  html += "<div class='city-form'>";
  html += "<label for=\"citySelect\">Select a city:</label>";
  html += "<select id=\"citySelect\" name=\"city\" onchange=\"window.location.href='/changeCity?city=' + this.value\">";
  html += "<option value=\"Boston\" " + String(currentCity == "Boston" ? "selected" : "") + ">Boston</option>";
  html += "<option value=\"New York\" " + String(currentCity == "New York" ? "selected" : "") + ">New York</option>";
  html += "<option value=\"Los Angeles\" " + String(currentCity == "Los Angeles" ? "selected" : "") + ">Los Angeles</option>";
  html += "<option value=\"Chicago\" " + String(currentCity == "Chicago" ? "selected" : "") + ">Chicago</option>";
  html += "<option value=\"Miami\" " + String(currentCity == "Miami" ? "selected" : "") + ">Miami</option>";
  html += "</select></div>";
  html += "<br><button type=\"submit\" id=\"refreshButton\">🔄 Refresh Weather</button>";
  html += "<span id='loading' class='loading'>Loading...</span>";
  html += "</form></div><div class='footer'>Powered by ESP32 and OpenWeatherMap 🌍</div>";
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  if (!MDNS.begin("weather")) {
    Serial.println("Error starting mDNS");
    while (true) delay(1000);
  }
  MDNS.addService("http", "tcp", 80);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateDashboard());
  });

  server.on("/refresh", HTTP_GET, []() {
    fetchWeather();
    server.send(200, "text/html", generateDashboard());
  });

  server.on("/changeCity", HTTP_GET, []() {
    if (server.hasArg("city")) {
      currentCity = server.arg("city");
      fetchWeather();
    }
    server.send(200, "text/html", generateDashboard());
  });

  server.begin();
  Serial.println("Web server started.");

  fetchWeather();
}

void loop() {
  if (millis() - lastWeatherCheckMillis >= WEATHER_CHECK_INTERVAL) {
    fetchWeather();
  }
  server.handleClient();
}
