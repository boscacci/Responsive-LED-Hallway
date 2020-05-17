void setup()
{
    pinMode(30, OUTPUT);
    pinMode(32, OUTPUT);

    digitalWrite(30, LOW);
    digitalWrite(32, LOW);

    delay(1000);
}

void loop()
{
    tone(32, 1000, 100);
    delay(2000);
}