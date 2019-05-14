#include <OctoWS2811.h>// ensure you are on PIN 2 + PINS 15 and 16 bridge 
#include <SdFat.h>

//OCtoWS2811 library from the good folk at PJRC http://forum.pjrc.com/forum.php
//SDfat library from this brilliant person http://code.google.com/p/sdfatlib/ 
// most of the code for the buffer workings come from ZTik.nl (many thanks)

//for SD card wiring
//MOSI goes to pin 11
//MISO goes to pin 12
//SCLK goes to pin 13
//SS goes to pin 10 

//fixed values for octoWS2811
const int ledsPerStrip = 147;//60;
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int NUM_LEDS = ledsPerStrip;

//for SD_Fat
SdFile myFile;
#define SD_CS SS // Set the chip select line to whatever you use (10 doesnt conflict with the library)
SdFat sd; // set filesystem
SdFile bmpFile; // set filesystem

//for image processing
#define BUF_SIZE 441//180//10240
#define BUFFPIXEL 50//20//147

int bmpLength = 300;//these two need filling in first
int paintTime =3000;//these two need filling in first
int time = paintTime/bmpLength;

// because the BMP is read from the bottom first, and we are displaying this using a POV wand,
// the BMP needs to be saved on its side to display correctly when the POV is waved, that is
// turned by 90 degrees, then saved as a 24bit bmp to the SD card

// BMP needs to be NUM_LEDS high when saving a version to the BMP.

// In the SD card, place 24 bit color BMP files (be sure they are 24-bit!)


ArduinoOutStream cout(Serial);
// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))


void setup()
{
//  Serial.begin(9600); //for bug testing and speed checking using serial
//while(!Serial){};
//  delay(100);
  progmemPrint(PSTR("Initializing SD card..."));
  if (!sd.begin(SD_CS, SPI_FULL_SPEED)) {
    progmemPrintln(PSTR("failed!"));
    return;
  }
  progmemPrintln(PSTR("OK!"));

  //begin for Octows2811
  leds.begin();
  delay(200);
}

void loop()
{

  upDown(5000); //this is a test pattern
//Chaser(2000,1); //this is a test pattern
//Blue_flower(5000,2); //this is a test pattern

  bmpDraw("tee.bmp");//,5000);
//  bmpDraw("diam.bmp",1000);
//  bmpDraw("wing.bmp",1000);
 //   bmpDraw("ball.bmp",1000);//filename and time in ms to keep displaying
    for(int a=0;a<ledsPerStrip;a++){
    leds.setPixel(a, Color(0,0,100));}//b,r,g - tested with single colour here
    leds.show();
delay (10000);
  }
/////////////////////////////////////////////////////////
//some patterns

 void upDown(unsigned long time){
   unsigned long currentTime = millis();
 while (millis()< currentTime + (time)) {
   
   for (int i=0;i<NUM_LEDS;i++) { 
  leds.setPixel(i, Color(255,0,0)); 
  leds.show();    
   delay(1);
   leds.setPixel(i,Color(0,0,0));
     }
    for (int i=NUM_LEDS-1;i>0;i--) { 
  leds.setPixel(i, Color(0,255,0)); 
  leds.show();    
   delay(1);
   leds.setPixel(i,Color(0,0,0));
    }
 }
 }
 ////////////////////////////////Chaser
void Chaser(long time, int frame){
unsigned long currentTime = millis();
unsigned int Colour2 = Color(255,0,0);//brg
unsigned int Colour =Color(0,0,0);
unsigned int Colour1 =Color(0,255,255);
int Led1 =0;
int Led2 =NUM_LEDS-1;
int increment =1;
int increment2=1;
while (millis()< currentTime + (time)) {
leds.setPixel(Led1, Colour1);
leds.setPixel(Led2, Colour2);
leds.show(); delay(1);
for(int x = 0 ; x < NUM_LEDS ; x++) leds.setPixel(x, Colour);
leds.show();
if(Led1>NUM_LEDS-1){increment=-1;}
else if(Led1<0){increment=1;}
Led1 = Led1+increment;
if(Led2<0){increment2=1;}
else if(Led2>NUM_LEDS-1){increment2=-1;}
Led2 = Led2+increment2;
}
Led1 =0;
Led2 =NUM_LEDS-1;
increment =1;
increment2=1;
}

 ///////////////////////////////////////////blue flower
 void Blue_flower(long time, int frame){
unsigned long currentTime = millis();
while (millis()< currentTime + (time)) {
 byte Counter;
 for(Counter=0;Counter < NUM_LEDS; Counter++)
 {
   leds.setPixel(Counter, Color(0,0,0));
   leds.setPixel(Counter+2, Color(0,255,0));
leds.show();
//delayMicroseconds(100);
delay(1);
}  
// Show a colour bar going down from 9 to 0
byte Counter2;
for(Counter2=NUM_LEDS;Counter2 > 0; Counter2--)
{
leds.setPixel(Counter2-1, Color(255,0,0));
if(Counter2>3){
leds.setPixel(Counter2-3,Color(0,255,0));}
leds.show();
//delayMicroseconds(100);
delay(1);
}  
}}

/////////////////////////////////////////////////////////////
void bmpDraw(char* filename){//, unsigned long time) {
// unsigned long currentTime = millis();
 
 //so that the image continues to be displayed again for a set time
//   while (millis()< currentTime + (time)) {
     
  SdFile     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel) 
  /*uint16_t*/uint32_t povbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)//////mg/////this needs to be 24bit per pixel////////
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  /*uint8_t*/int  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  povidx = 0;
  boolean  first = true;

  
  // Open requested file on SD card
  bmpFile.open(filename, O_READ);
progmemPrint(PSTR(filename));
  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    progmemPrint(PSTR("File size: ")); 
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    progmemPrint(PSTR("Image Offset: ")); 
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    progmemPrint(PSTR("Header size: ")); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      progmemPrint(PSTR("Bit Depth: ")); //Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!

        // BMP rows are padded (if needed) to 4-byte boundary
       rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        w = bmpWidth;
        h = bmpHeight;

     for (row=0; row<h; row++) {       
         if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.curPosition() != pos) { // Need seek?
            bmpFile.seekSet(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // read more pixel data
           if (buffidx >= sizeof(sdbuffer)) { 
             povidx = 0;         
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
           }
            // set pixel
            r = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            b = sdbuffer[buffidx++];
//we need to output BRG 24bit colour//

            povbuffer[povidx++] =(((unsigned int)b & 0x1F )<<16 | ((unsigned int)r & 0x1F)<<8 | (unsigned int)g & 0x1F);
            //Color(b,r,g);//octo colour, but you might need to change this
          }
        for(int i=0;i<ledsPerStrip;i++){
   leds.setPixel(i, povbuffer[i]); }
 
        leds.show();
        delay(time);// change the delay time depending effect required
       //   delayMicroseconds(220);
    } // end scanline

//        progmemPrint(PSTR("Loaded in ")); //for bug testing using serial output
//        Serial.print(millis() - startTime);
//        Serial.println(" ms");
      } // end goodBmp
    }
  }
//  Serial.println(read16(bmpFile), HEX);
   //}
  bmpFile.close();
}

//------- HELPER FUNCTION------//
// Create a 24 bit color value from R,G,B ////these strips are BRG, but you might need to change this
unsigned int Color(byte b/*r*/, byte g, byte /*b*/r)
{
  //Take the lowest 8 bits of each value and append them end to end
return( ((unsigned int)g & 0x1F )<<16 | ((unsigned int)r & 0x1F)<<8 | (unsigned int)b & 0x1F);
  }
//end of helper function

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(SdFile& f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
