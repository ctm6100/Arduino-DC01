# pragma once
# ifndef SRC_DC01_SENSOR_H_
# define SRC_DC01_SENSOR_H_

#ifndef READING_INTERVAL_MS
#define READING_INTERVAL_MS 2000 
#endif

// use freertos yield function if available
// #ifndef USE_FREERTOS
// #define USE_FREERTOS 1
// #endif

# include "Stream.h"

// DC01 Header byte
const uint8_t DC01_PACKET_HEADER = 0xA5;

// DC01 Packet length
const size_t DC01_PACKET_LENGTH = 4;

// SensorDC01 State
enum class DC01_ParserState {
    IDLE = 0,
    OK = 1,
    ERROR = -1,
};

class DC01_Sensor {
public:
    DC01_Sensor(Stream *stream, float k_value = 0.4f);
    ~DC01_Sensor();

    // calculate checksum for DC01 data packet
    static uint8_t dc01_calc_checksum(const uint8_t data[3]);

    DC01_ParserState parse_once();

    // Start parsing and allow attaching a callback for new readings
    void start_parsing(std::function<void(int)> reading_callback);

    // Optionally detach the callback
    void detach_callback();

    int set_reading(int new_reading) {
        reading = new_reading;
        return reading;
    }

    int get_raw_reading() {
        return reading;
    }

    bool set_k_value(float k_value){
        reading_k_value = k_value;
        return true;
    }

    float get_k_value(){
        return reading_k_value;
    }

    int get_air_quality_reading() {
        if (reading < 0) {
            return -1;
        }
        return static_cast<int>(reading * reading_k_value);
    }

    int get_error_count() {
        return error_count;
    }

    void increment_error_count() {
        error_count++;
    }

    int get_success_count() {
        return success_count;
    }

    void increment_success_count() {
        success_count++;
    }


private:
    // Callback for new readings
    std::function<void(int)> reading_callback_ = nullptr;

    // Stream object for communication
    Stream *stream = nullptr;

    // packet array
    uint8_t packet[DC01_PACKET_LENGTH];

    // last reading value
    int reading = -1;

    // K value to convert raw reading to pm2.5 concentration
    float reading_k_value = 0.4f;  
    
    // counter for error occurrences, may overflow eventually but who cares
    uint16_t error_count = 0;

    // counter for successful readings, may overflow eventually but who cares
    uint16_t success_count = 0;
};

# endif  // SRC_DC01_SENSOR_H