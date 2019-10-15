#include <SPI.h>

#define DRDY      9
#define CS        10

#define SPI_Hz    1250000
#define XTAL_Hz   7680000

#define SPI_tau   801L
#define XTAL_tau  131L

/*
 * Arduino UNO rev3
 * 
 * pin config...
 * XTAL frequency
 * CLK  - pin 13
    DIN  - pin 11 (MOSI)
    DOUT - pin 12 (MISO)
    CS   - pin 10
    DRDY - pin 9
 * 
 * 
 */


void setup() {
  Serial.begin(9600);
  SPI.begin();

  pinMode(CS, OUTPUT);
  pinMode(DRDY, INPUT);

  delay(50); //(?)

  begin_transaction();
  reset();
  Serial.print("reset OK");
  Serial.println();

  byte in[1]; 
  byte out[1];
  // drate 1000 A1
  // drate 30000 F0

  in[0] = 0xA1;
  write_registers(0x03,0x01,in);
  read_registers(0x03,1,out);
  Serial.print(out[0]);
  Serial.println();

  // gain 1 00100000 0x20
  // gain 4 00100010 0x22
  in[0] = {0x22};
  write_registers(0x02,0x01,in);

  read_registers(0x02,1,out);
  Serial.print(out[0]);
  Serial.println();

  selfcal();
  Serial.print("cal OK");
  Serial.println();
  
  end_transaction();

  
}


void loop() {
begin_transaction();
  Serial.print(read_input(0x00, 0x01));
  Serial.print("\t");
  Serial.print(read_input(0x02,0x03));
  Serial.println();
end_transaction();
  //delay(1);

}

//read input sequence optional (maybe next time)

long read_input(byte negative, byte positive){
  positive <<= 4;
  byte in[] = {positive|negative};
  write_registers(0x01,1,in);
  sync();
  wakeup();
  while(digitalRead(DRDY)==HIGH){}
  return read_input_e();
}

void selfcal(){
  byte first_cmd = 0xF0;
  SPI.transfer(first_cmd);
  while(digitalRead(DRDY)==HIGH){}
  
}


long read_input_e(){

  byte first_cmd = 0x01;
  long adc_val;

  SPI.transfer(first_cmd);
  
  //XTAL_tau*50+2*SPI_tau 9
  delayMicroseconds(get_delay(50,2));

  adc_val=SPI.transfer(0);
  adc_val<<=8;
  adc_val|=SPI.transfer(0);
  adc_val<<=4;
  adc_val|=SPI.transfer(0);

    //The ADS1255/6 output 24 bits of data in Binary Two's
  //Complement format. The LSB has a weight of
  //2VREF/(PGA(223 − 1)). A positive full-scale input produces
  //an output code of 7FFFFFh and the negative full-scale
  //input produces an output code of 800000h.
  if(adc_val > 0x7fffff){ //if MSB == 1
    adc_val = (16777215ul - adc_val) + 1; //do 2's complement
  }
  
  return adc_val;
  
}

void sync(){
  
  byte first_cmd = 0xFC;
  SPI.transfer(first_cmd);

  //XTAL_tau*24+2*SPI_tau 5
  delayMicroseconds(get_delay(24,2));
}

void wakeup(){
  
  byte first_cmd = 0x00;
  SPI.transfer(first_cmd);
  
}

void reset(){

  byte first_cmd = 0xFE;
  SPI.transfer(first_cmd);
  while(digitalRead(DRDY)==HIGH){}
  
}

void write_registers(byte start, byte how_many, byte in[]){
  
  byte first_cmd = 0x50;
  first_cmd|=start;
  byte second_cmd = how_many-1;

  SPI.transfer(first_cmd);
  SPI.transfer(second_cmd);

  for(int i = 0; i<how_many; i++)
    SPI.transfer(in[i]);

    //XTAL_tau*4+2*SPI_tau 2
  delayMicroseconds(get_delay(4,2));
}

void read_registers(byte start, byte how_many, byte out[]){

  byte first_cmd = 0x10;
  first_cmd|=start;
  byte second_cmd = how_many-1;

  SPI.transfer(first_cmd);
  SPI.transfer(second_cmd);

  //XTAL_tau*50+2*SPI_tau 9
  delayMicroseconds(get_delay(50,2));

  for(int i =0; i<how_many; i++)
    out[i] = SPI.transfer(0);

  //XTAL_tau*4+2*SPI_tau 2
  delayMicroseconds(get_delay(4,2));
}

void begin_transaction(){
  digitalWrite(CS, LOW);
  SPI.beginTransaction(SPISettings(SPI_Hz, MSBFIRST, SPI_MODE1));
}

void end_transaction(){
  SPI.endTransaction();
  //XTAL_tau*8+2*SPI_tau 2
  delayMicroseconds(get_delay(8,2));
  digitalWrite(CS,LOW);
}

void test(){
  
  
}

// make sure that delay_time before return is less than or equal to 65535
unsigned int get_delay(long xtal_mul, long spi_mul){

  long delay_time = (xtal_mul*XTAL_tau + spi_mul*SPI_tau)/1000L;
  delay_time ++;
  return delay_time;
}
