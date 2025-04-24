# 🌦️ ESP32 Weather Dashboard 🌞

This project creates a simple, colorful weather dashboard hosted on an ESP32 microcontroller. The dashboard displays real-time weather information fetched from the OpenWeatherMap API, allowing users to select their city and refresh the weather data directly from the web interface.

## Features
- Displays weather information such as temperature, humidity, and weather conditions.
- Allows users to select and change their city.
- Built using ESP32, Arduino framework, and the OpenWeatherMap API.
- Customizable and responsive dashboard design.
- Weather data refreshes every 1 hour (configurable).
- Real-time updates with a responsive design and animations.

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
After the ESP32 connects to Wi-Fi, it will host the weather dashboard. You can access it from your local network by entering the ESP32’s IP address into your browser (or weather.local).

## How to Use the Dashboard
1. **Select a City**: Use the dropdown menu to select a city.
2. **Refresh Weather**: Click the “🔄 Refresh Weather” button to update the weather data.
3. **Automatic Updates**: The weather data automatically refreshes every 1 hour.

## Code Explanation
### `fetchWeather()`:
This function fetches weather data from the OpenWeatherMap API, processes it, and formats it into a string for display. It includes information such as temperature, humidity, and weather condition (with corresponding emojis).

### `generateDashboard()`:
This function generates the HTML content for the weather dashboard, including city selection, weather information, and styling. It uses CSS for layout and JavaScript for the loading spinner during weather refresh.

### `setup()` and `loop()`:
The `setup()` function initializes the ESP32, connects to Wi-Fi, starts the mDNS service, and sets up the web server. The `loop()` function continually checks for client requests.

## Customization
- You can modify the list of cities available in the city dropdown menu.
- The design and colors of the dashboard can be customized by editing the CSS within `generateDashboard()`.

## License
This project is licensed under [The Unlicense](https://github.com/ronenlazowski/weatherdash/blob/main/LICENSE).

## Acknowledgments
- [OpenWeatherMap API](https://openweathermap.org/api) for weather data.
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32) for ESP32 support.

## Contributing
Feel free to fork the repository, submit pull requests, or open issues if you find bugs or have suggestions for improvements!
