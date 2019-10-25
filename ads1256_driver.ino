#include <SPI.h>

#define DRDY 9
#define CS 10

#define SPI_Hz  1250000L
#define XTAL_Hz 7680000L

const long SPI_ns =  1000000000L/SPI_Hz+1;
const long XTAL_ns = 1000000000L/XTAL_Hz+1;

/*
 * Arduino UNO rev3
 * 
 *  CLK  - pin 13
 *  DIN  - pin 11 (MOSI)
 *  DOUT - pin 12 (MISO)
 *  CS   - pin 10
 *  DRDY - pin 9
 *  
 */

void initialize(char* drate, char* gain) {

    //POWER-OFF to POWER-ON of the ADC
    delay(2000);
  
    SPI.begin();
    delay(2);

    pinMode(CS, OUTPUT);
    pinMode(DRDY, INPUT);

    begin_transaction();
    reset();

    byte in[1];
    byte out[1];
    
    in[0] = drate_to_byte(drate);
    write_registers(0x03, 0x01, in);

    in[0] = gain_to_byte(gain);
    write_registers(0x02, 0x01, in);

    selfcal();

    end_transaction();

}

byte gain_to_byte(char* string){
  if(strcmp(string,"1")==0){
    return 0x20;
  } else if (strcmp(string,"2")==0){
    return 0x21;
  } else if (strcmp(string,"4")==0){
    return 0x22;
  } else if (strcmp(string,"8")==0){
    return 0x23;
  } else if (strcmp(string,"16")==0){
    return 0x24;
  } else if (strcmp(string,"32")==0){
    return 0x25;
  } else if (strcmp(string,"64")==0){
    return 0x26;
  } else if (strcmp(string,"64")==0){
    return 0x27;
  }
}

byte drate_to_byte(char* string){
    if(strcmp(string,"30000")==0){
    return 0xF0;
  } else if (strcmp(string,"15000")==0){
    return 0xE0;
  } else if (strcmp(string,"7500")==0){
    return 0xD0;
  } else if (strcmp(string,"3750")==0){
    return 0xC0;
  } else if (strcmp(string,"2000")==0){
    return 0xB0;
  } else if (strcmp(string,"1000")==0){
    return 0xA1;
  } else if (strcmp(string,"500")==0){
    return 0x92;
  } else if (strcmp(string,"100")==0){
    return 0x82;
  } else if (strcmp(string,"60")==0){
    return 0x72;
  } else if (strcmp(string,"50")==0){
    return 0x63;
  } else if (strcmp(string,"30")==0){
    return 0x53;
  } else if (strcmp(string,"25")==0){
    return 0x43;
  } else if (strcmp(string,"15")==0){
    return 0x33;
  } else if (strcmp(string,"10")==0){
    return 0x23;
  } else if (strcmp(string,"5")==0){
    return 0x13;
  } else if (strcmp(string,"2.5")==0){
    return 0x03;
  }
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

/*
 * in[0] = neg_1, in[1] = pos_1, in[2] = neg_2 ...
 * out[0] = val_1...
 */
void read_sequence(byte in[], long out[], int num_of_readings) {

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

long read_input_e() {

    byte first_cmd = 0x01;
    long adc_val;

    SPI.transfer(first_cmd);

    delayMicroseconds(get_delay(50, 2));

    adc_val = SPI.transfer(0);
    adc_val <<= 8;
    adc_val |= SPI.transfer(0);
    adc_val <<= 8;
    adc_val |= SPI.transfer(0);
    
    delayMicroseconds(get_delay(4, 2));

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

void selfcal() {
    byte first_cmd = 0xF0;
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

// make sure that delay_time before return is less than or equal to 65535
unsigned int get_delay(long xtal_mul, long spi_mul) {

    long delay_time = (xtal_mul * XTAL_ns + spi_mul * SPI_ns) / 1000L;
    delay_time+=2;
    return delay_time;
}
