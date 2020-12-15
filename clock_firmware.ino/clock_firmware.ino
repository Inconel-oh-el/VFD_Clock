#include <SPI.h>
#include <RTClib.h>

//CS pins
int cs_DAC = 6;
int rtc_RST = 7;
int cs_RTC = 10;


// digit selects, directly manipulate port c data register, pins 2-5
int D1 = 0b00000100;
int D2 = 0b00001000;
int D3 = 0b00010000;
int D4 = 0b00100000;
int D1234 = 0b00111100;

int digitArray[] = {D1,D2,D3,D4};

// enable cathode current (active LOW)
int en_CC = 4;

//interrupt register
byte sreg;

bool updateTime = false;

void setup() {
  // Timer Counter1 interrupt configuration - see ATmega328P datasheet
  cli();          //disable interrupts
  
  TCCR1A = 0x00;
  TCCR1B = 0x0f;
  TCNT1  = 0x00;  //initialize counter value to 0
  OCR1A  = 0x3F;  // output compare register 1A to 63 (every second, final code will be one minute)
  TIMSK1 = 0x02;  // enable timer compare interrupt
  
  sei();          //re enable interrupts

    // Cathode Current (works??)
  pinMode(en_CC,INPUT);
  digitalWrite(en_CC,HIGH);
  
  
  Serial.begin(115200);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  
  //Chip select pins as outputs
  pinMode(cs_DAC,OUTPUT);
  pinMode(rtc_RST,OUTPUT);
  pinMode(cs_RTC,OUTPUT);

  // digit select pins
  DDRC = DDRC | B00011110;
  
  
  // initial DAC value
  setBrightness(255);

  //RTC Initialization
  digitalWrite(rtc_RST,HIGH);
  digitalWrite(cs_RTC,HIGH);
  
  RTC_begin(); //For Power up. Clears the Halt Bit and the Stop Bit.
  RTC_adjust(DateTime(__DATE__, __TIME__)); //sets the date and time at the moment of compiling this sketch.
  RTC_sqw(0b10010000);

  Serial.println(__DATE__);
  Serial.println(__TIME__);

}

void loop() {  

  if (updateTime){
    int theTime[4];     //modified in functions
    RTC_now(theTime);
    setTime(theTime);
    //Serial.print(theTime[0]);
    //Serial.print(theTime[1]);
    //Serial.print(":");
    //Serial.print(theTime[2]);
    //Serial.println(theTime[3]);
    updateTime = false;
  }
  
}

void setBrightness(int adcVal){
  // write,buffered, 1x gain,active
  // 0b0110...0
  short setBits = 0x7000;
  // combine (logical or) setbits w/ value
  short val16 = setBits | ((short)adcVal << 4);

  //Serial.println(val16,BIN);
  digitalWrite(cs_DAC,LOW);
  SPI.transfer16(val16);
  digitalWrite(cs_DAC,HIGH);
}

void setTime(int timeArray[]){
  for (int i=0;i<4;i++){
    setDigit(digitArray[i-1],getDigit(timeArray[i]));
  }
}
void setDigit(int digit, byte value){
  // pull line low
  PORTC = digit;
  SPI.transfer(value);
  PORTC = 0x00;
}
byte getDigit(int digit){
  //binary values for each number. Not the most efficient way, should just put into a global array but who cares
  byte binVal = 0xFF;
  switch (digit) {
  case -1:
    binVal = 0b11111111;
  case 1:
    binVal = 0b10100000;
    break;
  case 2:
    binVal = 0b11001101;
    break;
  case 3:
    binVal = 0b11101001;
    break;
  case 4:
    binVal = 0b11100010;
    break;
  case 5:
    binVal = 0b01101011;
    break;
  case 6:
    binVal = 0b01101111;
    break;
  case 7:
    binVal = 0b10100001;
    break;
  case 8:
    binVal = 0b11101111;
    break;
  case 9:
    binVal = 0b11100011;
    break;
  case 0:
    binVal = 0b10101111;
    break;
  default:
    binVal = 0b00000000;
    break;
} 
  return binVal;
}

void digitFade(uint8_t oldByte, byte newByte,byte digit){
  byte flipReg = oldByte ^ newByte;
  byte pushByte = oldByte;
    for (int i = 7; i >=0 ; i--)  {
      if (bitRead(flipReg,i)) {
      // if bits at iterated position are XOR (not the same), do this fancy shit
      //pushByte = bitWrite(oldByte,i,bitRead(newByte,i)); 
      pushByte = bitWrite(oldByte,i,!bitRead(oldByte,i)); 
      setDigit(digit,pushByte);
      
      delay(60);
      }
   
    }
  setDigit(digit,newByte);

  delay(60);
}

ISR(TIMER1_COMPA_vect){
  updateTime = !updateTime;
  //Serial.println("update Time!!");
}

 
