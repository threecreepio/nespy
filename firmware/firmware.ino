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

void loop()
{
    noInterrupts();
    WAIT_FALLING_EDGE(PIN_LATCH);
    uint8_t b0 = 0;
    uint8_t b1 = 0;
    for (uint8_t i=0; i<8; ++i) {
        WAIT_FALLING_EDGE(PIN_CLOCK);
        b0 |= (!PIN_READ(PIN_DATA)) ? (1 << i) : 0;
    }
    for (uint8_t i=0; i<8; ++i) {
        WAIT_FALLING_EDGE(PIN_CLOCK);
        b1 |= (!PIN_READ(PIN_DATA)) ? (1 << i) : 0;
    }
    Serial.write(b0 & 0x0F | 0x80); // mark start byte
    Serial.write(b0 >> 4);
    Serial.write(b1 & 0x0F);
    Serial.write(b1 >> 4);
    interrupts();
    Serial.flush();
}
