#include <Bounce2.h>
#include <EncoderStepCounter.h>
#include <U8g2lib.h>


Bounce btn = Bounce();



#define ENCODER_PIN1 2
#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 3
#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)
#define ENCODER_BUTTON 4

// Create instance for one full step encoder
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);
// Use the following for half step encoders
// EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2, HALF_STEP);


U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 7, /* data=*/ 6, /* cs=*/ 5, /* reset=*/ U8X8_PIN_NONE);
// d4, enable , rs

void setup(){
	Serial.begin(9600);
	// Initialize encoder
	encoder.begin();
	// Initialize interrupts
	attachInterrupt(ENCODER_INT1, interrupt, CHANGE);
	attachInterrupt(ENCODER_INT2, interrupt, CHANGE);

	pinMode(9, INPUT);

	btn.attach(ENCODER_BUTTON, INPUT_PULLUP);
	btn.interval(5);
	u8g2.begin();
}


// This is an example on how to change a "long" variable
// with the library. With every loop the value is added
// and then cleared in the encoder library
signed long position = 0;
void loop(){
	// picture loop
	u8g2.firstPage();
	do {
			draw();
	} while( u8g2.nextPage() );

	// rebuild the picture after some delay


	signed char pos = encoder.getPosition();
	if (pos != 0){
		position += pos;
		encoder.reset();
		Serial.println(position);
	}


	if (digitalRead(8) == 0) {
		digitalWrite(9, 1);
		Serial.println("Buzz ON");
	}else {
		digitalWrite(9, 0);
	}

	// Serial.println(digitalRead(ENCODER_BUTTON));
	btn.update();
	if(btn.read() == 0){
		Serial.print("!");
	}
}

bool lamp1On = false;
bool lamp2On = true;
void draw(void) {
	// graphic commands to redraw the complete screen should be placed here
	u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
	// u8g2.drawStr(0, 22, "Hello World!");
	u8g2.drawStr(0,44, String(position).c_str());




	u8g2.drawStr(110, 10, "Y");
	u8g2.drawStr(110, 20, "B");

	u8g2.setFont(u8g2_font_unifont_t_symbols);
	u8g2.drawGlyph(118, 11, 0x25cf);  // Iconic font: Lamp on (C) and Lamp off (B)
	u8g2.drawGlyph(118, 20, 0x25ce);


}




// Call tick on every change interrupt
void interrupt(){
	encoder.tick();
}
void press(){
	Serial.println("Button pressed");
}

