#include <SPI.h>

#define DRDY 9
#define CS 10

#define SPI_Hz 1250000
#define XTAL_Hz 7680000

#define SPI_tau 801L
#define XTAL_tau 131L

/*
 * Arduino UNO rev3
 * 
 * pin config...
 * XTAL frequency
 * CLK  - pin 13
 *  DIN  - pin 11 (MOSI)
 *  DOUT - pin 12 (MISO)
 *  CS   - pin 10
 *  DRDY - pin 9
 *  
 */

void setup() {
    Serial.begin(9600);
    SPI.begin();

    delay(2);

    pinMode(CS, OUTPUT);
    pinMode(DRDY, INPUT);

    begin_transaction();
    reset();
    Serial.print("reset OK");
    Serial.println();

    byte in[1];
    byte out[1];
    
    // drate 2.5    03
    // drate 1000   A1
    // drate 30000  F0
    in[0] = 0xA1;
    write_registers(0x03, 0x01, in);

    read_registers(0x03, 1, out);
    Serial.print(out[0]);
    Serial.println();

    // gain 1 00100000 0x20
    // gain 4 00100010 0x22
    in[0] = 0x20;
    write_registers(0x02, 0x01, in);

    read_registers(0x02, 1, out);
    Serial.print(out[0]);
    Serial.println();

    selfcal();
    Serial.print("cal OK");
    Serial.println();

    end_transaction();

}

void loop() {

    begin_transaction();
    ///*  
        Serial.print(read_input(0x00, 0x01));
        Serial.print("\t");
        Serial.print(read_input(0x02,0x03));
        Serial.println();
    //*/
    /*
    byte in[4] = {
        0x00,
        0x01,
        0x02,
        0x03
    };
    long out[2];

    read_sequence(in, out);

    Serial.print(out[0]);
    Serial.print("\t");
    Serial.print(out[1]);
    Serial.println();
     */  
    end_transaction();

}

/*
 * in[0] = neg_1, in[1] = pos_1, in[2] = neg_2 ...
 * out[0] = val_1...
 */
void read_sequence(byte in[], long out[]) {
    int num_of_readings = sizeof(out);

    byte to_write[1];
    int i = 0, j = -1;

    while (i < num_of_readings) {

        in[2 * i + 1] <<= 4;
        to_write[0] = ( in[2 * i + 1] | in[2 * i]);

        while (digitalRead(DRDY) == HIGH) {}
        write_registers(0x01, 1, to_write);
        sync();
        wakeup();

        if (j > -1)
            out[j] = read_input_e();
       
        i++; j++;
    }

    while (digitalRead(DRDY) == HIGH) {}
    sync();
    wakeup();
    out[j] = read_input_e();

}

long read_input(byte negative, byte positive) {
    positive <<= 4;
    byte in[] = {
        positive | negative
    };
    write_registers(0x01, 1, in);
    sync();
    wakeup();
    while (digitalRead(DRDY) == HIGH) {}
    return read_input_e();
}

void selfcal() {
    byte first_cmd = 0xF0;
    SPI.transfer(first_cmd);
    while (digitalRead(DRDY) == HIGH) {}
}

long read_input_e() {

    byte first_cmd = 0x01;
    long adc_val;

    SPI.transfer(first_cmd);

    delayMicroseconds(get_delay(50, 2));

    adc_val = SPI.transfer(0);
    adc_val <<= 8;
    adc_val |= SPI.transfer(0);
    adc_val <<= 4;
    adc_val |= SPI.transfer(0);

    //The ADS1255/6 output 24 bits of data in Binary Two's
    //Complement format. The LSB has a weight of
    //2VREF/(PGA(2^23 − 1)). A positive full-scale input produces
    //an output code of 7FFFFFh and the negative full-scale
    //input produces an output code of 800000h.
    if (adc_val > 0x7fffff) { //if MSB == 1
        adc_val = (16777215ul - adc_val) + 1; //do 2's complement
    }

    return adc_val;

}

void sync() {

    byte first_cmd = 0xFC;
    SPI.transfer(first_cmd);

    delayMicroseconds(get_delay(24, 2));
}

void wakeup() {

    byte first_cmd = 0x00;
    SPI.transfer(first_cmd);

}

void reset() {

    byte first_cmd = 0xFE;
    SPI.transfer(first_cmd);
    while (digitalRead(DRDY) == HIGH) {}

}

void write_registers(byte start, byte how_many, byte in[]) {

    byte first_cmd = 0x50;
    first_cmd |= start;
    byte second_cmd = how_many - 1;

    SPI.transfer(first_cmd);
    SPI.transfer(second_cmd);

    for (int i = 0; i < how_many; i++)
        SPI.transfer(in[i]);

    delayMicroseconds(get_delay(4, 2));
}

void read_registers(byte start, byte how_many, byte out[]) {

    byte first_cmd = 0x10;
    first_cmd |= start;
    byte second_cmd = how_many - 1;

    SPI.transfer(first_cmd);
    SPI.transfer(second_cmd);

    //XTAL_tau*50+2*SPI_tau 9
    delayMicroseconds(get_delay(50, 2));

    for (int i = 0; i < how_many; i++)
        out[i] = SPI.transfer(0);

    delayMicroseconds(get_delay(4, 2));
}

void begin_transaction() {
    digitalWrite(CS, LOW);
    SPI.beginTransaction(SPISettings(SPI_Hz, MSBFIRST, SPI_MODE1));
}

void end_transaction() {
    SPI.endTransaction();
    delayMicroseconds(get_delay(8, 2));
    digitalWrite(CS, LOW);
}

void test() {

}

// make sure that delay_time before return is less than or equal to 65535
unsigned int get_delay(long xtal_mul, long spi_mul) {

    long delay_time = (xtal_mul * XTAL_tau + spi_mul * SPI_tau) / 1000L;
    delay_time++;
    return delay_time;
}
