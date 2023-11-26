#include <Thing.h>
#include <BlinkPattern.h>
#include <WEMOS_Matrix_LED.h>
#include <ArduinoJson.h>

#define BUFFER_SIZE 1024

using namespace ootb;

Thing thing;
BlinkPattern led(BUILTIN_LED);

BlinkPattern::Pattern<2> start{{1,9},25};
BlinkPattern::Pattern<2> normal{{1,39},25};
BlinkPattern::Pattern<0> disable{{},1000};

MLED mled(0);

void setup()
{
    Serial.begin(230400);
    Serial.println();
    Serial.println("Client:" + thing.clientId());

    led.setPattern(start);

    thing.onStateChange([](const String& msg){
        Serial.println(msg);
    });

    thing.begin();

    String topic = "things/" + thing.clientId() + "/ledmatrix/buffer";
    thing.addActuator(topic, [](Value& value)
    {
        Serial.println(value);
        StaticJsonDocument<BUFFER_SIZE> doc;
        String msg = value;
        deserializeJson(doc, msg);

        JsonArray buffer = doc["buffer"].as<JsonArray>();
        mled.intensity = 7;
        uint8_t index = 0;
        for(JsonVariant v : buffer)
        {
            uint8_t b = v.as<int>();
            //Serial.println(b);
            mled.disBuffer[index++] = b;
            mled.display();
        }
        return true;
    });

    led.setPattern(normal);
}

void loop()
{
    thing.handle();
    led.handle();
}
