#define PIN_READ( pin )  (PIND&(1<<(pin)))
#define WAIT_FALLING_EDGE( pin ) while( !PIN_READ(pin) ); while( PIN_READ(pin) );
#define WAIT_LEADING_EDGE( pin ) while( PIN_READ(pin) ); while( !PIN_READ(pin) );

#define PIN_LATCH           3 // BLUE
#define PIN_DATA            4 // RED
#define PIN_CLOCK           6 // ORANGE

void setup()
{
    pinMode(PIN_LATCH, INPUT_PULLUP);
    pinMode(PIN_CLOCK, INPUT_PULLUP);
    pinMode(PIN_DATA, INPUT_PULLUP);
    Serial.begin(115200, SERIAL_8N1);

    bitClear(ADCSRA, ADPS0);
    bitSet(ADCSRA, ADPS1);
    bitClear(ADCSRA, ADPS2);
}

uint8_t lastSent = 0;
void loop()
{
    noInterrupts();
    WAIT_FALLING_EDGE(PIN_LATCH);
    uint8_t nextValue = 0;
    for (uint8_t i=0; i<8; ++i) {
        WAIT_FALLING_EDGE(PIN_CLOCK);
        nextValue += (!PIN_READ(PIN_DATA)) ? (1 << i) : 0;
    }
    interrupts();
    //if (lastSent != nextValue) {
      lastSent = nextValue;
      Serial.write(nextValue);
    //}
}
