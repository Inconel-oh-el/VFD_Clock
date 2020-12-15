/*

RTC command library for the ST Micro's M41T93 Real Time Clock
Authored by Ted Ullrich, http://tomorrow-lab.com
Co-Authored by Justin Downs, http://groundlab.cc
January 2012
Built to work with the RTClib created by JeeLabs http://news.jeelabs.org/code/
Released to the public domain.

*/



#include <SPI.h>
#include <RTClib.h>


#define makewriteAddress(address) (address | 0x80);
uint8_t myAddress;


uint8_t bcd2bin(uint8_t val){
  return( (val/16*10) + (val%16) );
}

uint8_t bin2bcd(uint8_t val){
  return( (val/10*16) + (val%10) );
}


void RTC_cs(int _value){
  //SPI.setDataMode(SPI_MODE0);
  digitalWrite(cs_RTC,_value);
}

uint8_t RTC_begin(void)
{
  //POWER UP:

  //1. Clear Halt Bit (HT)
  myAddress = makewriteAddress(0x0C); //Set halt register as write
  RTC_cs(LOW);
  SPI.transfer(myAddress); //ADDRESS OF HALT WRITE MODE
  SPI.transfer(0x00); //clear bit 6
  RTC_cs(HIGH);


  // 2. Reset Stop Bit (ST) D7 bit to 1 then 0 (stop start clock, inits)
  myAddress = makewriteAddress(0x01); //Seconds register as write
  RTC_cs(LOW);
  SPI.transfer(myAddress); //ADRESS OF HALT WRITE MODE
  SPI.transfer(0x80); //ST bit high, setting it to 1
  SPI.transfer(myAddress); //ADRESS OF HALT WRITE MODE
  SPI.transfer(0x00); //ST bit low, setting it to 0
  RTC_cs(HIGH);

}

void RTC_sqw(byte freq){
    // Control byte for square wave output

  byte addr_13h = freq;
  byte addr_0Ah = 0b01000000;
  myAddress = makewriteAddress(0x13);
  digitalWrite(cs_RTC,LOW);
  SPI.transfer(0x13 + 0x80);
  SPI.transfer(addr_13h);
  SPI.transfer(0x6 + 0x80);
  SPI.transfer(addr_0Ah);
  digitalWrite(cs_RTC,HIGH);
}





void RTC_adjust(const DateTime& dt)
{
  RTC_cs(LOW);
  myAddress = makewriteAddress(0x00); //Set 00 reg as write
  SPI.transfer(myAddress); //ADDRESS OF milliseconds
  SPI.transfer(bin2bcd(0)); //Set to 0. Don't need this level of accuracy.
  RTC_cs(HIGH);

  RTC_cs(LOW);
  myAddress = makewriteAddress(0x01); //Set 01 reg as write
  SPI.transfer(myAddress); //ADDRESS OF seconds
  SPI.transfer(bin2bcd(dt.second())); //set value
  RTC_cs(HIGH);


  RTC_cs(LOW);
  myAddress = makewriteAddress(0x02); //Set 02 reg as write
  SPI.transfer(0x2 + 0x80); //ADDRESS OF minutes
  SPI.transfer(bin2bcd(dt.minute())); //set value
  RTC_cs(HIGH);
 
  RTC_cs(LOW);
  myAddress = makewriteAddress(0x03); //Set 03 reg as write
  SPI.transfer(myAddress); //ADDRESS OF hours
  SPI.transfer(bin2bcd(dt.hour())); //set value
  RTC_cs(HIGH);
  
  RTC_cs(LOW);
  myAddress = makewriteAddress(0x04); //Set 04 reg as write
  SPI.transfer(myAddress); //ADDRESS OF day of week
  SPI.transfer(bin2bcd(dt.dayOfTheWeek())); //set value
  RTC_cs(HIGH);
  
  RTC_cs(LOW);
  myAddress = makewriteAddress(0x05); //Set 05 reg as write
  SPI.transfer(myAddress); //ADDRESS OF day (date)
  SPI.transfer(bin2bcd(dt.day())); //set value
  RTC_cs(HIGH);
 
  RTC_cs(LOW);
  myAddress = makewriteAddress(0x06); //Set 06 reg as write
  SPI.transfer(myAddress); //ADDRESS OF month
  SPI.transfer(bin2bcd(dt.month())); //set value
  RTC_cs(HIGH);


  RTC_cs(LOW);
  myAddress = makewriteAddress(0x07); //Set 07 reg as write
  SPI.transfer(myAddress); //ADDRESS OF year
  SPI.transfer(bin2bcd(dt.year() - 2000)); //set value
  RTC_cs(HIGH);

  


}

int RTC_now(int timeArray[])
{

  /*
Reading the Time. 
   Time is held in Rgisters 0x00 through 0x07:
   00 - milliseconds (00-99)
   01 - seconds (00-59)
   02 - minutes (00-59)
   03 - hours, 24 hour format (00-23)
   04 - day of week (01-07)
   05 - date (01-31)
   06 - month (01-12)
   07 - year (00-99)
   
   Register reads are auto-advanced to the next line as you read each line,
   so you can send 0x00 transfers one after another.
   */

  RTC_cs(LOW);
  SPI.transfer(0x00); //Start address for reading time

  //Register 00
  uint8_t mss = SPI.transfer(0x00);  //Get the milliseconds
    
  //Register 01
  uint8_t ss = SPI.transfer(0x00);        // Get the seconds
    
  //Register 02
  uint8_t mm =  SPI.transfer(0x00);  // Get the minutes
    
  //Register 03
  uint8_t hh =  SPI.transfer(0x00);        // Get the hours  
  
  //Register 04
  uint8_t wday =  SPI.transfer(0x00);        // Get the day of the week
  
  //Register 05
  uint8_t d =  SPI.transfer(0x00);        // Get the month day (date)
  
  //Register 06
  uint8_t m =  SPI.transfer(0x00);        // Get the month
  
  //Register 07
  uint16_t y =  SPI.transfer(0x00);        // Get the year
      

  RTC_cs(HIGH);
  
  //mss = bcd2bin(mss);
  //ss = bcd2bin(ss);
  mm = bcd2bin(mm);
  hh = bcd2bin(hh);

  
  //wday = bcd2bin(wday);
  //d = bcd2bin(d);  
  //m = bcd2bin(m);  
  //y = bcd2bin(y) + 2000;                // add 2000 to the value returned.

    
  toTimeArray(hh,mm,timeArray);
}

void toTimeArray(uint8_t hh, uint8_t mm,int timeArray[]){
  //int timeArray[4];
  timeArray[0] = hh / 10;
  timeArray[1] = hh % 10;
  timeArray[2] = mm / 10;
  timeArray[3] = mm % 10;

  //return(timeArray);
}
void RTC_open() {
  digitalWrite(cs_RTC, LOW); //turn on RTC

}
void RTC_close() {
  digitalWrite(cs_RTC, HIGH); //turn off RTC
}

  
   
