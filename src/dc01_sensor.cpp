#include "dc01_sensor.h"

// Constructor
DC01_Sensor::DC01_Sensor(Stream *stream, float k_value) {
    this->stream = stream;

    // Initialize packet storing array
    std::fill(packet, packet + DC01_PACKET_LENGTH, 0);

    // Initialize the error count
    error_count = 0;

    // Initialize reading
    reading = -1;

    // Set K value
    reading_k_value = k_value;
}

// Destructor
DC01_Sensor::~DC01_Sensor() {
    // Cleanup if necessary
}

// Calculate checksum for DC01 data packet
uint8_t DC01_Sensor::dc01_calc_checksum(const uint8_t data[3]) {
    //Return the least significant 7 bits of the sum
    return (data[0] + data[1] + data[2]) & 0x7F;
}

// parse_once implementation
DC01_ParserState DC01_Sensor::parse_once() {
    // Implementation of parsing logic goes here

    // Check if stream pointer is valid
    if (stream == nullptr) {
        // increment error count
        increment_error_count();
        return DC01_ParserState::ERROR;
    }


    // peek the next byte and store it
    uint8_t peeked = stream->peek();

    // check if the incoming byte is the header byte
    while (peeked != DC01_PACKET_HEADER)
    {
        // if not header, discard the byte
        stream->read();

        // increment error count
        increment_error_count();

        // check if more data is available
        if (stream->available() == 0) {
            // No more data available, return IDLE state, maybe you shd yield? anyway you call
            return DC01_ParserState::IDLE;
        }

        // if there is more data, peek at the next byte again
        peeked = stream->peek();
    }


    // check if data is available, return WAITING readable is less than DC01_PACKET_LENGTH - 1 bytes
    if (stream->available() < DC01_PACKET_LENGTH - 1) {
        // Sorry we don't have enough data yet
        return DC01_ParserState::WAITING;
    }

    // Read the packet
    uint8_t packet[DC01_PACKET_LENGTH];
    stream->readBytes(packet, DC01_PACKET_LENGTH);


    // // there is no need to check header again since we have already peeked it
    // if (packet[0] != DC01_PACKET_HEADER) {
    //     // if header byte is incorrect, increment error count
    //     increment_error_count();
    //     return DC01_ParserState::ERROR;
    // }


    // Validate checksum
    // Calculate checksum
    uint8_t calculated_checksum = DC01_Sensor::dc01_calc_checksum(&packet[1]);
    
    // Compare with received checksum
    if (packet[3] != calculated_checksum) {
        // if not match with checksum, increment error count
        increment_error_count();

        // and return error
        return DC01_ParserState::ERROR;
    }


    // Extract raw reading
    uint16_t raw_reading = (packet[1] << 8) | packet[2];

    // Update the reading
    this->set_reading(static_cast<int>(raw_reading));

    // increment success count
    increment_success_count();

    // Successfully parsed a valid packet
    return DC01_ParserState::OK;

}
    
// Start parsing and allow attaching a callback for new readings
void DC01_Sensor::start_parsing(std::function<void(int)> reading_callback) {
    
    // check callback is valid
    if (reading_callback != nullptr) {
        reading_callback_ = reading_callback;
    } else {
        // put a default no-op callback
        reading_callback_ = [](int){};
    }

    // Example polling loop (user should call this in a task or main loop)
    for (;;) {
        DC01_ParserState state = parse_once();

        if (state == DC01_ParserState::OK) {
            // If a new reading is available, invoke the callback
            if (reading_callback_) {
                reading_callback_(get_air_quality_reading());
            }
        }


# if defined(USE_FREERTOS)
        vTaskDelay(pdMS_TO_TICKS(READING_INTERVAL_MS));
# else
        // yield to other tasks or delay
        yield();
        delay(READING_INTERVAL_MS);
# endif
    }
}

// Optionally detach the callback
void DC01_Sensor::detach_callback() {
    reading_callback_ = [](int){};
}