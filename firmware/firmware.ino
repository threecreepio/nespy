#define PIN_READ( pin )  (PIND&(1<<(pin)))
#define WAIT_FALLING_EDGE( pin ) while( !PIN_READ(pin) ); while( PIN_READ(pin) );
#define WAIT_LEADING_EDGE( pin ) while( PIN_READ(pin) ); while( !PIN_READ(pin) );

                              // NES    // SNES
#define PIN_LATCH           3 // BLACK  // ORANGE
#define PIN_DATA            4 // WHITE  // RED
#define PIN_CLOCK           6 // GRAY   // BROWN

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

uint8_t bits = 8;
void loop()
{
    noInterrupts();
    WAIT_FALLING_EDGE(PIN_LATCH);
    uint16_t nextValue = 0;
    for (uint8_t i=0; i<bits; ++i) {
        WAIT_FALLING_EDGE(PIN_CLOCK);
        nextValue += (!PIN_READ(PIN_DATA)) ? (1 << i) : 0;
    }
    interrupts();
    Serial.write(nextValue >> 8);
    Serial.write(nextValue & 0xFF);
    if (Serial.available()) {
        switch (Serial.read()) {
            case 0: bits = 8; break;
            case 1: bits = 16; break;
        }
    }
}
