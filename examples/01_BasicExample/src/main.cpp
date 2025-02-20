#include "Arduino.h"
#include "espasyncbutton.hpp"
#include <array>

#define BUTTON_1  GPIO_NUM_0                 // Set GPIO for the button #1
#define BUTTON_2  GPIO_NUM_35                // Set GPIO for the button #2

// shortcut alias
using ESPButton::event_t;

// strings for event logging
static constexpr std::array<const char *, 8> event_name = {
    "press",
    "release",
    "click",
    "longPress",
    "longRelease",
    "autoRepeat",
    "multiClick",
    "undefined"
};

// Buttons

/**
 * @brief gpio mapped button with Logic level LOW, i.e. button shorts gpio to the ground, gpio must be pulled high
 * 
 * @return GPIOButton<ESPEventPolicy> 
 */
GPIOButton<ESPEventPolicy> b1(BUTTON_1, LOW); 

/**
 * @brief gpio mapped button with Logic level LOW, i.e. button shorts gpio to the ground, gpio must be pulled high
 * 
 * @return GPIOButton<ESPEventPolicy> 
 */
GPIOButton<ESPEventPolicy> b2(BUTTON_2, LOW);

/**
 * @brief function declaration that will subsribe to the button events
 * 
 * @param handler_args - not used in this example, NULL here
 * @param base - EBTN_EVENTS identifier
 * @param id - event id
 * @param event_data - supplied data pointer via void*
 */
void evt_hndlr(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);



void setup(){
    Serial.begin(115200);
    // We MUST create default ESP event loop unless WiFi or Bluetooth is used
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // subscribe our callback function to button events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(EBTN_EVENTS, ESP_EVENT_ANY_ID, evt_hndlr, NULL, NULL));

    // let's enable ALL events for button 1
    b1.enableEvent(event_t::click);
    b1.enableEvent(event_t::longPress);
    b1.enableEvent(event_t::longRelease);
    b1.enableEvent(event_t::autoRepeat);
    b1.enableEvent(event_t::multiClick);

    // for button two in addition to basic 'press', 'release' we will enable only multiClicks
    b2.enableEvent(event_t::multiClick);

    // enable buttons
    b1.enable();
    b2.enable();
}

void loop(){
    // Simply do nothing here
    delay(1000);
}

// this function will simply print all events for both buttons
void evt_hndlr(void* handler_args, esp_event_base_t base, int32_t id, void* event_data){
    // decode event id into text format
    const char* event =  id < 0 || id >= event_name.size() ? event_name.at(7) : event_name.at(id);

    // get pointer to additional data about the event, we can get gpio number and click counter for the button that generated the event
    EventMsg *e = reinterpret_cast<EventMsg*>(event_data);

    // print event message
    Serial.printf( "Button Event generated %s:%s    gpio:%d, counter:%u\n", base, event, e->gpio, e->cntr);
}
