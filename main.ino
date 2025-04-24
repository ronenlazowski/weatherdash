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
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
  } else {
    Serial.println("Connected to WiFi");
  }
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
    DynamicJsonDocument doc(4096);
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
  html += generateCityDropdown();
  html += "</div>";
  html += "<br><button type=\"submit\" id=\"refreshButton\">🔄 Refresh Weather</button>";
  html += "<span id='loading' class='loading'>Loading...</span>";
  html += "</form></div><div class='footer'>Powered by ESP32 and OpenWeatherMap 🌍</div>";
  html += "</body></html>";
  return html;
}

const String allowedCities[] = {"Boston", "New York", "Los Angeles", "Chicago", "Miami"};

bool isValidCity(const String& city) {
  for (const String& allowedCity : allowedCities) {
    if (city == allowedCity) return true;
  }
  return false;
}

String generateCityDropdown() {
  String dropdown = "<select id=\"citySelect\" name=\"city\" onchange=\"window.location.href='/changeCity?city=' + this.value\">";
  for (const String& city : allowedCities) {
    dropdown += "<option value=\"" + city + "\" " + String(currentCity == city ? "selected" : "") + ">" + city + "</option>";
  }
  dropdown += "</select>";
  return dropdown;
}


void setup() {
  Serial.begin(115200);
  connectToWiFi();

  if (!MDNS.begin("weather")) {
    Serial.println("Error starting mDNS");
    while (true) delay(1000);
  }
  MDNS.addService("http", "tcp", 80);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateDashboard());
  });

  static unsigned long lastRefreshMillis = 0;
  server.on("/refresh", HTTP_GET, [&]() {
    if (millis() - lastRefreshMillis < 2000) {
      server.send(429, "text/plain", "Too many requests. Please wait.");
      return;
    }
    lastRefreshMillis = millis();
    fetchWeather();
    server.send(200, "text/html", generateDashboard());
  });

  static unsigned long lastCityChangeMillis = 0;
  const unsigned long cityChangeInterval = 1000;

  server.on("/changeCity", HTTP_GET, []() {
    if (millis() - lastCityChangeMillis < cityChangeInterval) {
      server.send(429, "text/plain", "Too many requests. Please wait.");
      return;
    }
    if (server.hasArg("city")) {
      String newCity = server.arg("city");
      if (isValidCity(newCity)) {
        currentCity = newCity;
        fetchWeather();
        lastCityChangeMillis = millis();
      } else {
        server.send(400, "text/plain", "Invalid city");
        return;
      }
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
