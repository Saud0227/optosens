#include <EncoderStepCounter.h>
#include <U8glib.h>

U8GLIB_ST7920_128X64_1X u8g(7, 6, 5);
// d4, enable , rs

void setup(){
	Serial.begin(9600);
	pinMode(LED_BUILTIN, 1);
	pinMode(9, 1);
}


void loop(void) {
	// picture loop
	u8g.firstPage();
	do {
			draw();
	} while( u8g.nextPage() );

	// rebuild the picture after some delay
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1000);
	digitalWrite(LED_BUILTIN, LOW);

	if (digitalRead(8) == 0) {
		digitalWrite(9, 1);
		Serial.println("Buzz ON");
	}else {
		digitalWrite(9, 0);
	}
	delay(1000);
}

void draw(void) {
	// graphic commands to redraw the complete screen should be placed here  
	u8g.setFont(u8g_font_unifont);
	u8g.drawStr( 0, 22, "Hello World!");
}
