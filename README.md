# 🌦️ ESP32 Weather Dashboard 🌞

This project creates a simple, colorful weather dashboard hosted on an ESP32 microcontroller. The dashboard displays real-time weather information fetched from the OpenWeatherMap API, allowing users to select their city and refresh the weather data directly from the web interface.

## Features
- Displays weather information such as temperature, humidity, and weather conditions.
- Allows users to select and change their city.
- Built using ESP32, Arduino framework, and the OpenWeatherMap API.
- Customizable and responsive dashboard design.
- Weather data refreshes every 1 hour (configurable).
- Real-time updates with a responsive design and animations.
- Basic rate limiting to prevent spammy refreshes and city changes.
- Predefined list of cities ensures reliable and secure updates.

## Requirements
- **Hardware:**
  - ESP32 development board
- **Software:**
  - Arduino IDE (or PlatformIO)
  - ArduinoJson library
  - WiFi.h (included with ESP32 core)
  - HTTPClient.h (included with ESP32 core)
  - WebServer.h (included with ESP32 core)
  - ESPmDNS.h (included with ESP32 core)

## Setup Instructions

### 1. Clone the repository
Clone this repository to your local machine:
```bash
git clone https://github.com/ronenlazowski/weatherdash.git
```

### 2. Install Required Libraries
Install the required libraries in the Arduino IDE:
- **ArduinoJson**: [ArduinoJson GitHub](https://github.com/bblanchon/ArduinoJson)
- **ESP32 board package**: Follow the instructions for installing the ESP32 board package in the Arduino IDE at [ESP32 Arduino](https://github.com/espressif/arduino-esp32).

### 3. Modify Wi-Fi Credentials
In the code, modify the following lines with your Wi-Fi credentials:
```cpp
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"
```

### 4. Add OpenWeatherMap API Key
Sign up for a free API key at [OpenWeatherMap](https://openweathermap.org/api) and replace `API_KEY` with your key in the code:
```cpp
#define OPENWEATHERMAP_API_KEY "API_KEY"
```

### 5. Upload the Code
Upload the code to your ESP32 board using the Arduino IDE or PlatformIO. After uploading, open the Serial Monitor to check if your ESP32 connects to the Wi-Fi.

### 6. Access the Dashboard
After the ESP32 connects to Wi-Fi, it will host the weather dashboard. You can access it from your local network by entering the ESP32’s IP address into your browser (or `http://weather.local`).

## How to Use the Dashboard
1. **Select a City**: Use the dropdown menu to select from a list of predefined cities.
2. **Refresh Weather**: Click the “🔄 Refresh Weather” button to update the weather data.
3. **Automatic Updates**: The weather data automatically refreshes every 1 hour.
4. **Validation & Rate Limiting**:
   - Only supported cities from a fixed list can be selected.
   - Requests to refresh weather or change cities are rate-limited to prevent spam (e.g., no more than once every second).

## Code Explanation
### `fetchWeather()`:
This function fetches weather data from the OpenWeatherMap API, processes it, and formats it into a string for display. It includes temperature, humidity, condition info, and emojis.

### `generateDashboard()`:
Generates the HTML content for the weather dashboard, including city selection, weather info, and responsive design with CSS and JavaScript.

### `setup()` and `loop()`:
- `setup()` initializes the ESP32, connects to Wi-Fi, starts mDNS, and sets up the web server routes (including city validation and request throttling).
- `loop()` handles ongoing client requests and triggers hourly weather updates.

## Customization
- You can modify the list of available cities in the code:
  ```cpp
  const String allowedCities[] = {"Boston", "New York", "Los Angeles", "Chicago", "Miami"};
  ```
- The dashboard style and behavior (colors, layout, intervals) can be adjusted by editing the HTML/CSS in `generateDashboard()`.

## License
This project is licensed under [The Unlicense](https://github.com/ronenlazowski/weatherdash/blob/main/LICENSE).

## Acknowledgments
- [OpenWeatherMap API](https://openweathermap.org/api) for weather data.
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32) for ESP32 support.

## Contributing
Feel free to fork the repository, submit pull requests, or open issues if you find bugs or have suggestions for improvements!

![Weather Dashboard Screenshot](https://i.imgur.com/ISZYmdO.png)
