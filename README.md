# DC01 Arduino Library

This is Arduino library for the DC01 infrared air quality sensor. It provides an easy-to-use interface for reading sensor data and converting it to air quality values.

## Features

- Parse DC01 sensor packets
- Attach a callback for new readings
- Error and success counters
- Configurable K value for calibration

## Installation

### PlatformIO

Add the following to your `platformio.ini`:

```yaml
library_dependencies =
  https://github.com/ctm6100/Arduino-DC01
```

### Arduino IDE

1. Download or clone this repository.
2. Copy the `src` folder to your Arduino `libraries` directory.

## Usage Example

```cpp
#include <dc01_sensor.h>

DC01_Sensor sensor(&Serial);

void setup() {
    Serial.begin(9600);
    sensor.start_parsing([](int reading) {
        Serial.print("DC01 Reading: ");
        Serial.println(reading);
    });
}

void loop() {
    // Call parse_once() in your main loop if not using start_parsing
    //sensor.parse_once();
    // Other code...
}
```

## API Overview

- `DC01_Sensor(Stream *stream, float k_value = 0.4f)`
- `void start_parsing(std::function<void(int)> reading_callback)`
- `void detach_callback()`
- `int get_raw_reading()`
- `float get_k_value()`
- `int get_air_quality_reading()`
- `int get_error_count()`
- `int get_success_count()`

## License

MIT
