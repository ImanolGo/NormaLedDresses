/*
This code uses the T3.6 onboard sd card slot, BMP stored on the card are light painted using a cheap strip of ws2811 leds using FastLed
code derived from lightpainting sketch:
https://forum.pjrc.com/threads/24535-OctoWS2811-POV-reading-BMP-from-SD-card
and this post:
——> https://forum.pjrc.com/threads/40871-Teensy-6-5-SDFat-BMP-file-read-fail
*/

#include <SPI.h>
#include <SD.h>
#include <FastLED.h>

File bmpFile;
const int chipSelect = BUILTIN_SDCARD;

// you can remove all Serial.print when you have your paint staff
// set up, this is just for debug

int bmpWidth, bmpHeight;
uint8_t bmpDepth, bmpImageoffset;
#define BUFFPIXEL 512

unsigned int Color(byte b, byte r, byte g); //placed here to avoid compiler error

// How many leds in your strip?
#define NUM_LEDS 147

#define DATA_PIN 2 //the pin that Led strip is attached to
CRGB leds[NUM_LEDS];
int paintSpeed = 15; //adjust this to vary image refresh rate

void setup(void) {
Serial.begin(9600);

FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);

//test our led strip – you can remove this to the comment line "// if you dont get …"
for(int x=0;x<NUM_LEDS;x++){
leds[x] = CRGB::Green;}
FastLED.show();
delay(500);
for(int x=0;x<NUM_LEDS;x++){
leds[x] = CRGB::Red;}
FastLED.show();
delay(500);
for(int x=0;x<NUM_LEDS;x++){
leds[x] = CRGB::Blue;}
FastLED.show();
delay(500);
for(int x=0;x<NUM_LEDS;x++){
leds[x] = CRGB::Black;}
FastLED.show();
delay(500);
// if you dont get all leds lighting then going off, check your wiring

Serial.println("init");
delay(500);
Serial.print("Initializing SD card…");

if (!SD.begin(chipSelect)) {
Serial.println("initialization failed!");
return;
}
Serial.println("SD OK!");

}
void loop() {

bmpDraw("JEM.bmp");//example filename
bmpDraw("DWARFS.bmp");
bmpDraw("ISLA.bmp");
}

//////////////////Function to read BMP and send to Led strip a row at a time/////////////////////
void bmpDraw(char* filename){

File bmpFile;
int bmpWidth, bmpHeight; // W+H in pixels
uint8_t bmpDepth; // Bit depth (currently must report 24)
uint32_t bmpImageoffset; // Start of image data in file
uint32_t rowSize; // Not always = bmpWidth; may have padding
uint8_t sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
uint32_t povbuffer[BUFFPIXEL]; // pixel out buffer (16-bit per pixel)//////mg/////this needs to be 24bit per pixel////////
uint32_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
boolean goodBmp = false; // Set to true on valid header parse
boolean flip = true; // BMP is stored bottom-to-top
int w, h, row, col;
int r, g, b;
uint32_t pos = 0, startTime = millis();
uint8_t povidx = 0;
boolean first = true;

// Open requested file on SD card
bmpFile = SD.open(filename);
Serial.println(filename);
// Parse BMP header
if(read16(bmpFile) == 0x4D42) { // BMP signature
Serial.print("File size: ");
Serial.println(read32(bmpFile));
(void)read32(bmpFile); // Read & ignore creator bytes
bmpImageoffset = read32(bmpFile); // Start of image data
Serial.print("Image Offset: ");
Serial.println(bmpImageoffset, DEC);
// Read DIB header
Serial.print("Header size: ");
Serial.println(read32(bmpFile));
bmpWidth = read32(bmpFile);
bmpHeight = read32(bmpFile);
if(read16(bmpFile) == 1) { // # planes — must be ‘1’
bmpDepth = read16(bmpFile); // bits per pixel
Serial.print("Bit Depth: "); Serial.println(bmpDepth);
if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

goodBmp = true; // Supported BMP format — proceed!
Serial.print("Image size: ");
Serial.print(bmpWidth);
Serial.print(‘x’);
Serial.println(bmpHeight);

// BMP rows are padded (if needed) to 4-byte boundary
rowSize = (bmpWidth * 3 + 3) & ~3;

// If bmpHeight is negative, image is in top-down order.
// This is not canon but has been observed in the wild.
if(bmpHeight < 0) {
bmpHeight = -bmpHeight;
flip = false;
}

w = bmpWidth;
h = bmpHeight;

for (row=0; row<h; row++) {
if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
pos = bmpImageoffset + (bmpHeight – 1 – row) * rowSize;
else // Bitmap is stored top-to-bottom
pos = bmpImageoffset + row * rowSize;
if(bmpFile.position() != pos) { // Need seek?
bmpFile.seek(pos);
buffidx = sizeof(sdbuffer); // Force buffer reload
}

for (col=0; col<w; col++) { // For each column…
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
Serial.print(r);Serial.print(" ");Serial.print(g);Serial.print(" ");Serial.println(b);
//we need to output BRG 24bit colour//
povbuffer[povidx++] =(b<<16) + (g<<8) +r;
}

for(int i=0;i<NUM_LEDS;i++){
leds[i]=povbuffer[i];}
FastLED.show();
delay(paintSpeed);// change the delay time depending effect required
} // end scanline

} // end goodBmp
}
}//end of IF BMP
Serial.println();

bmpFile.close();
}

//*************Support Funcitons****************//
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t read16(File& f) {
uint16_t result;
((uint8_t *)&result)[0] = f.read(); // LSB
((uint8_t *)&result)[1] = f.read(); // MSB
return result;
}
uint32_t read32(File& f) {
uint32_t result;
((uint8_t *)&result)[0] = f.read(); // LSB
((uint8_t *)&result)[1] = f.read();
((uint8_t *)&result)[2] = f.read();
((uint8_t *)&result)[3] = f.read(); // MSB
return result;
