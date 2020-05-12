#define PIR 5

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIR, INPUT_PULLUP);
    Serial.begin(9600);
}

void loop()
{
    if (digitalRead(PIR))
    {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("1");
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("0");
    }
}