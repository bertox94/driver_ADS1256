void setup() {
    //Serial.begin(9600);

}

void loop() {

    begin_transaction();
    
    end_transaction();
}

long maximum(long n1, long n2){
  if(n1>n2)
    return n1;
  return n2;
}

long minimum(long n1, long n2){
  if(n1>n2)
    return n2;
  return n1;
}

void test(){
  
  static long i =0;
  long test_size = 10000L;

  if(i == test_size){
    Serial.println();
    i++;
   } else if (i<test_size) {
      test_single_e(i);
      i++;
   }
}

void test_single_e(long i){
  static long max0=0L;
  static long min0=2147483647L;
  static long max1=0L;
  static long min1=2147483647L;
  static long max2=0L;
  static long min2=2147483647L;
  static long max3=0L;
  static long min3=2147483647L;
  static long max4=0L;
  static long min4=2147483647L;
  static long max5=0L;
  static long min5=2147483647L;
  static long max6=0L;
  static long min6=2147483647L;
  static long max7=0L;
  static long min7=2147483647L;
  
  long alpha0 = read_input(0x00, 0x08);
  long alpha1 = read_input(0x01, 0x08);
  long alpha2 = read_input(0x02, 0x08);
  long alpha3 = read_input(0x03, 0x08);
  long alpha4 = read_input(0x04, 0x08);
  long alpha5 = read_input(0x05, 0x08);
  long alpha6 = read_input(0x06, 0x08);
  long alpha7 = read_input(0x07, 0x08);

  //long alphadiff = readADSDiff(0, 1);

  max0=maximum(max0,alpha0);
  max1=maximum(max1,alpha1);
  max2=maximum(max2,alpha2);
  max3=maximum(max3,alpha3);
  max4=maximum(max4,alpha4);
  max5=maximum(max5,alpha5);
  max6=maximum(max6,alpha6);
  max7=maximum(max7,alpha7);
  min0=minimum(min0,alpha0);
  min1=minimum(min1,alpha1);
  min2=minimum(min2,alpha2);
  min3=minimum(min3,alpha3);
  min4=minimum(min4,alpha4);
  min5=minimum(min5,alpha5);
  min6=minimum(min6,alpha6);
  min7=minimum(min7,alpha7);

  Serial.print(">>");
  Serial.print(++i);
  Serial.println();
  Serial.print(min0);
  Serial.print("\t");
  Serial.print(alpha0);
  Serial.print("\t");
  Serial.print(max0);
  Serial.println();
  Serial.print(min1);
  Serial.print("\t");
  Serial.print(alpha1);
  Serial.print("\t");
  Serial.print(max1);
  Serial.println();
  Serial.print(min2);
  Serial.print("\t");
  Serial.print(alpha2);
  Serial.print("\t");
  Serial.print(max2);
  Serial.println();
  Serial.print(min3);
  Serial.print("\t");
  Serial.print(alpha3);
  Serial.print("\t");
  Serial.print(max3);
  Serial.println();
  Serial.print(min4);
  Serial.print("\t");
  Serial.print(alpha4);
  Serial.print("\t");
  Serial.print(max4);
  Serial.println();
  Serial.print(min5);
  Serial.print("\t");
  Serial.print(alpha5);
  Serial.print("\t");
  Serial.print(max5);
  Serial.println();
  Serial.print(min6);
  Serial.print("\t");
  Serial.print(alpha6);
  Serial.print("\t");
  Serial.print(max6);
  Serial.println();
  Serial.print(min7);
  Serial.print("\t");
  Serial.print(alpha7);
  Serial.print("\t");
  Serial.print(max7);
  Serial.println();
 
  Serial.println();
  
}

void test_sequence_e(long i){
  static long max0=0L;
  static long min0=2147483647L;
  static long max1=0L;
  static long min1=2147483647L;
  static long max2=0L;
  static long min2=2147483647L;
  static long max3=0L;
  static long min3=2147483647L;


  byte in[8] = {
        0x00,
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07
    };
    
    long out[4];
    read_sequence(in, out,4);

  //long alphadiff = readADSDiff(0, 1);

  max0=maximum(max0,out[0]);
  max1=maximum(max1,out[1]);
  max2=maximum(max2,out[2]);
  max3=maximum(max3,out[3]);

  min0=minimum(min0,out[0]);
  min1=minimum(min1,out[1]);
  min2=minimum(min2,out[2]);
  min3=minimum(min3,out[3]);

  Serial.print(">>");
  Serial.print(++i);
  Serial.println();
  Serial.print(min0);
  Serial.print("\t");
  Serial.print(out[0]);
  Serial.print("\t");
  Serial.print(max0);
  Serial.println();
  Serial.print(min1);
  Serial.print("\t");
  Serial.print(out[1]);
  Serial.print("\t");
  Serial.print(max1);
  Serial.println();
  Serial.print(min2);
  Serial.print("\t");
  Serial.print(out[2]);
  Serial.print("\t");
  Serial.print(max2);
  Serial.println();
  Serial.print(min3);
  Serial.print("\t");
  Serial.print(out[3]);
  Serial.print("\t");
  Serial.print(max3);
  Serial.println();

  Serial.println();

}
