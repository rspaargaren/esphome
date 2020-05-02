#include "max7219digit.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "max7219font.h"

namespace esphome {
namespace max7219digit {

static const char *TAG = "max7219";

static const uint8_t MAX7219_REGISTER_NOOP = 0x00;
static const uint8_t MAX7219_REGISTER_DECODE_MODE = 0x09;
static const uint8_t MAX7219_REGISTER_INTENSITY = 0x0A;
static const uint8_t MAX7219_REGISTER_SCAN_LIMIT = 0x0B;
static const uint8_t MAX7219_REGISTER_SHUTDOWN = 0x0C;
static const uint8_t MAX7219_REGISTER_DISPLAY_TEST = 0x0F;
//static const uint8_t MAX7219_UNKNOWN_CHAR = 0b11111111;
constexpr uint8_t MAX7219_NO_SHUTDOWN = 0x00;
constexpr uint8_t MAX7219_SHUTDOWN = 0x01;
constexpr uint8_t MAX7219_NO_DISPLAY_TEST = 0x00;
constexpr uint8_t MAX7219_DISPLAY_TEST = 0x01;


float MAX7219Component::get_setup_priority() const { return setup_priority::PROCESSOR; }

void MAX7219Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX7219...");
  this->spi_setup();
  this->buffer_ = new uint8_t[this->num_chips_ * 8];  // Create a buffer with chips*8 display positions
  for (uint8_t i = 0; i < this->num_chips_ * 8; i++)  // Clear buffer for startup
    this->buffer_[i] = 0;

  // let's assume the user has all 8 digits connected, only important in daisy chained setups anyway
  this->send_to_all_(MAX7219_REGISTER_SCAN_LIMIT, 7);
  // let's use our own ASCII -> led pattern encoding
  this->send_to_all_(MAX7219_REGISTER_DECODE_MODE, 0);
  // No display test with all the pixels on  
  this->send_to_all_(MAX7219_REGISTER_DISPLAY_TEST, MAX7219_NO_DISPLAY_TEST);
  // SET Intsity of display
  //this->send_to_all_(MAX7219_REGISTER_INTENSITY, this->intensity_);
  this->send_to_all_(MAX7219_REGISTER_INTENSITY, 1);
  this->display();
  // power up
  this->send_to_all_(MAX7219_REGISTER_SHUTDOWN, 1);
  ESP_LOGW(TAG,"MAX7219 SETUP STARTED");
}
void MAX7219Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MAX7219:");
  ESP_LOGCONFIG(TAG, "  Number of Chips: %u", this->num_chips_);
  ESP_LOGCONFIG(TAG, "  Intensity: %u", this->intensity_);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}

void MAX7219Component::display() {
  for (uint8_t i = 0; i < 8; i++) {                       //Run this loop 8 times
    this->enable();                                       //Pull CS high to enable SPI communication
    for (uint8_t j = 0; j < this->num_chips_; j++) {      //Run this routine for the number of chips
      this->send_byte_(8 - i, this->buffer_[j * 8 + i]);  //Send the byte from buffer (start at digit 8, buffer item [0]), buffer item [8]
    }                                                     //                                    digit 7, buffer item [1] , buffer item [9]
                                                          //                                    digit 1, buffer item [7] , buffer item [10]
    ESP_LOGW(TAG,"Display Called");                       //Debug feedback for testing
    this->disable();                                      //Pull CS low to disable SPI communication
  }
}
void MAX7219Component::send_byte_(uint8_t a_register, uint8_t data) {
  this->write_byte(a_register);
  this->write_byte(data);
}
void MAX7219Component::send_to_all_(uint8_t a_register, uint8_t data) {
  this->enable();
  for (uint8_t i = 0; i < this->num_chips_; i++)
    this->send_byte_(a_register, data);
  this->disable();
}
void MAX7219Component::update() {
  ESP_LOGW(TAG,"UPDATE CALLED");                      //Debug feedback for testing update is triggered by polling component
  for (uint8_t i = 0; i < this->num_chips_ * 8; i++)  //run this loop for chips*8 (all display positions)
    this->buffer_[i] = 0;                             //clear buffer on every position
  if (this->writer_.has_value())                      //inser Labda function if available
    (*this->writer_)(*this);                          
  //this->display();                                  //call display to write buffer
}
//uint8_t MAX7219Component::print(uint8_t start_pos, const char *str) {
//  uint8_t pos = start_pos;                    // start positie
//  for (; *str != '\0'; str++) {               // Doorloop de string van start to eind /0
//    uint8_t data = MAX7219_UNKNOWN_CHAR;      //set data op niet gevonden
//    if (*str >= ' ' && *str <= '~')           // als de string in een bepaalde range is lees de waarde uit
//      data = pgm_read_byte(&MAX7219_ASCII_TO_RAW[*str - ' ']);  //lees de byte waarde uit voor data bij de string
//    if (data == MAX7219_UNKNOWN_CHAR) {       // als de data een niet mogelijke char is geef melding
//      ESP_LOGW(TAG, "Encountered character '%c' with no MAX7219 representation while translating string!", *str);
//    }
//    if (*str == '.') {                        // als de string een punt is
//      if (pos != start_pos)                   // als de huidige positie geen start punt is 
//        pos--;                                // Zet de positie een stap terug
//      this->buffer_[pos] |= 0b10000000;       // Voeg punt toe aan buffer positie
//    } else {
//      if (pos >= this->num_chips_ * 8) {      // als de huidige positie groter is dan aantal digits geef foutmelding
//        ESP_LOGE(TAG, "MAX7219 String is too long for the display!");
//        break;                                // stop de routine
//      }
//      this->buffer_[pos] = data;              // schrijf de waarde van data in de buffer op pos
//    }
//    pos++;                                    // schuif de positie een waarde op
//  }
//  return pos - start_pos;                     // geef aantal gebruikte posities terug
//}

// write an entire null-terminated string to the LEDs
uint8_t MAX7219Component::print(const char *str) { return this->print(0, str); }

uint8_t MAX7219Component::print(uint8_t start_pos, const char *s)
{
  byte chip;
  this->offset_char = 0;
  for (chip = 0; chip < this->num_chips_ && *s; chip++)
  {
    if (*s == 58){
      this->offset_char = 1;
      *s++;
      chip--;
      this->double_dots_right=true;
    } else {
      *s++;
      if (*s == 58)
        this->double_dots_left=true;
      *s--;
      sendChar (chip, *s++);
    }
    
  }
 // space out rest
  while (chip < (this->num_chips_))
    sendChar (chip++, ' ');
return 0;
}  // end of sendString

void MAX7219Component::moveString (const char * s,const bool direction)
  {
  if (s != this->string_buffer) {
    if (direction) {
    this-> string_pos = (strlen(s) - this->num_chips_)*8;
    }
    else
    {
     this-> string_pos = 0; 
    }
    
    this-> string_buffer = s;
    ESP_LOGW(TAG,"lengte van %s is %i",s,this->string_pos);
  }
  ESP_LOGW(TAG,"De positie is %i",this->string_pos);
  this->sendSmooth(s,this->string_pos);
  if (direction)
  {
    this->string_pos --;
    if (this->string_pos==0)
      {
        this-> string_pos = (strlen(s) - this->num_chips_)*8;
      }
  }
  else
  {
    this->string_pos ++;
    if (this->string_pos==((strlen(s) - this->num_chips_)*8))
    {
      this->string_pos = 0;
    }
  }
  
}

uint8_t MAX7219Component::moveStringf(const char *format, const bool direction, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    this->moveString(buffer,direction);
  return 0;
}

void MAX7219Component::sendSmooth (const char * s, const int pixel)
  {
  int len = strlen (s);       // set len for length of string
  byte thisChip [3 * 8];      // pixels for current chip with allowance for one each side
  int firstByte = pixel / 8;  // device pixel input by 8 = 
  int offset = pixel - (firstByte * 8); //offset = pixel - multiply by 8 

  byte chip;                  // chip number

  for (chip = 0; chip < this->num_chips_; chip++) //Run procedure for every chip
    {
    memset (thisChip, 0, sizeof thisChip); // empty memory with 0 for thischip byte

    // get pixels to left of current character in case "pixel" is negative
    if (offset < 0)          // run only if offset is negative ()
      {
      if (firstByte + chip - 1 >= 0 && firstByte + chip - 1 < len)
        for (byte i = 0; i < 8; i++)
           thisChip [i] = pgm_read_byte (&MAX7219_Dot_Matrix_font [((const byte *) s) [firstByte  + chip - 1]] [i]);
      }  // negative offset

    // get the current character
    if (firstByte + chip >= 0 && firstByte + chip < len)
      for (byte i = 0; i < 8; i++)
         thisChip [i + 8] = pgm_read_byte (&MAX7219_Dot_Matrix_font [((const byte *) s) [firstByte + chip]] [i]);

    // get pixels to right of current character in case "pixel" is positive
    if (offset > 0)
      {
      if (firstByte + chip + 1 >= 0 && firstByte + chip + 1 < len)
        for (byte i = 0; i < 8; i++)
           thisChip [i + 16] = pgm_read_byte (&MAX7219_Dot_Matrix_font [((const byte *) s) [firstByte + chip + 1]] [i]);
      }  // positive offset

    // send the appropriate 8 pixels (offset will be from -7 to +7)
    send64pixels (chip, &thisChip [8 + offset]);

    } // for each chip

  } // end of MAX7219_Dot_Matrix::sendSmooth


void MAX7219Component::sendChar (const byte chip, const byte data)
  {
  // get this character from PROGMEM
  byte pixels [8];
  for (byte i = 0; i < this->offset_char; i++)
     pixels[i]=0;
  if (this->double_dots_right){
     pixels[0]=0x66;
     this->double_dots_right=false;
  }
  for (byte i = 0; i < 8-this->offset_char; i++)
     pixels [i+this->offset_char] = pgm_read_byte (&MAX7219_Dot_Matrix_font [data] [i]);

  if (this->double_dots_left){
     pixels[7]=0x66;
     this->double_dots_left=false;
  }
  this->send64pixels (chip, pixels);
  }  // end of sendChar

// send one character (data) to position (chip)

void MAX7219Component::send64pixels (const byte chip, const byte pixels [8])
  {
  for (byte col = 0; col < 8; col++)    //RUN THIS LOOP 8 times until column is 7
    {
    this->enable();                     // start sending by enabling SPI 
    for (byte i = 0; i < chip; i++)     // send extra NOPs to push the pixels out to extra displays
      this->send_byte_ (MAX7219_REGISTER_NOOP, MAX7219_REGISTER_NOOP);  //run this loop unit the matching chip is reached
    byte b = 0;                         // rotate pixels 90 degrees -- set byte to 0
    for (byte i = 0; i < 8; i++)        //  run this loop 8 times for all the pixels[8] received
      b |= bitRead (pixels [i], col) << (7 - i);  // change the column bits into row bits
    this->send_byte_(col + 1 , b);       // send this byte to dispay at selected chip
    for (int i = 0; i < this->num_chips_ - chip - 1; i++) // end with enough NOPs so later chips don't update
      this->send_byte_ (MAX7219_REGISTER_NOOP, MAX7219_REGISTER_NOOP);
    this->disable();                    // all done disable SPI
    }   // end of for each column
  }  // end of send64pixels

uint8_t MAX7219Component::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t MAX7219Component::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}
void MAX7219Component::set_writer(max7219_writer_t &&writer) { this->writer_ = writer; }
void MAX7219Component::set_intensity(uint8_t intensity) { this->intensity_ = intensity; }
void MAX7219Component::set_num_chips(uint8_t num_chips) { this->num_chips_ = num_chips; }

#ifdef USE_TIME
uint8_t MAX7219Component::strftime(uint8_t pos, const char *format, time::ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t MAX7219Component::strftime(const char *format, time::ESPTime time) { return this->strftime(0, format, time); }
#endif

}  // namespace max7219
}  // namespace esphome
