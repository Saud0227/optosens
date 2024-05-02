#include <arduino-timer.h>
// #include <DisplayInterface.h>
#include <EncoderStepCounter.h>
#include <U8g2lib.h>

// Encoder defines
#define ENCODER_PIN1 2
#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 3
#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)
#define ENCODER_BUTTON 4
// LCD Screen defines
#define CLOCK_PIN 7
#define DATA_PIN 6
#define CS_PIN 5
// Non-critical screen defines
#define E_STOP_PIN 8
#define BUZZER_PIN 9
// LED pins defines
#define YELLOW_PIN 12
#define BLUE_PIN 13

EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, CLOCK_PIN, DATA_PIN, CS_PIN, U8X8_PIN_NONE);
void(* resetFunc) (void) = 0;

auto timer = timer_create_default();

void setup(){
	Serial.begin(9600);

	encoder.begin();

	attachInterrupt(ENCODER_INT1, interrupt, CHANGE);
	attachInterrupt(ENCODER_INT2, interrupt, CHANGE);

	pinMode(BUZZER_PIN, INPUT);
	pinMode(LED_BUILTIN, 1);
	digitalWrite(LED_BUILTIN, 0);
	pinMode(ENCODER_BUTTON, INPUT_PULLUP);

	u8g2.begin();

	splash_screen();
}


void splash_screen(){
	u8g2.firstPage();
	do {
		u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
		string_center("OptoSense", 25);
		string_center("Carl Johan - Tural", 40);
	} while( u8g2.nextPage() );

	delay(5*1000);
	encoder.reset();
}


void loop(){
	displayLogic();

	if(digitalRead(E_STOP_PIN)==0){
		tone(BUZZER_PIN, 2560);
		delay(2000);
		resetFunc();
	}

	digitalWrite(BUZZER_PIN,0);
}


void buzz(){
	tone(BUZZER_PIN, 2560);
	delay(100);
	noTone(BUZZER_PIN);
	delay(20);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
int lampMode = 0;
int dispState = 0;
int sel_max = 0;

bool running = false;

bool timerActive = false;
int timeUnit = 0;
int timeToSet = 0;

int delayTime = 0;


void string_center(String in_string, int y){
	int w = u8g2.getStrWidth(in_string.c_str());
	u8g2.drawStr(64-w/2, y, in_string.c_str());
}


signed long position = 0;
void displayLogic(){
	// picture loop
	u8g2.firstPage();
	do {
			draw();
	} while( u8g2.nextPage() );

	signed char pos = encoder.getPosition();
	if (pos != 0){
		if (0==sel_max){
			// prevent disp side having clicks
		} else if ((position + pos*-1)<0){
			tone(BUZZER_PIN, 100, 10);
			position = 0;
		}else if((position + pos*-1)>sel_max){
			tone(BUZZER_PIN, 100, 10);
			position = sel_max;
		}else {
			tone(BUZZER_PIN, 50, 10);
			position += pos*-1;
		}
		encoder.reset();
	}

	btn();
}

void interrupt(){
	encoder.tick();
}

bool last_state = false;
void btn(){
	bool c_state = !digitalRead(ENCODER_BUTTON);
	if (last_state != c_state){
		if (c_state) {
			press();
		}
		last_state = c_state;
	}
}

void press(){
	buzz();
	changeView();
	position = 0;
	encoder.reset();
}




void draw(void) {
	// graphic commands to redraw the complete screen should be placed here
	u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
	// u8g2.drawStr(5,44, String(position).c_str());
	switch(dispState){
		case 0:
			drawStatus();
			break;
		case 1:
			drawMainMenu();
			break;
		case 2:
			drawTimerMenu();
			break;
		case 3:
			numberSelector();
			break;
		case 4:
			numberSelector();
			break;
	}
}

void changeView(){
	switch(dispState){
		case 0:
			dispState = 1;
			sel_max = 4;
			break;
		case 1:
			switch (position){
				case 0:
					dispState = 0;
					sel_max = 0;
					break;
				case 1:
					running = !running;
					dispState = 0;
					sel_max = 0;
					buzz();
					break;
				case 2:
					dispState = 2;
					sel_max = 3;
					break;
				case 3:
					lampMode += 1;
					if (lampMode == 4) {
						lampMode = 0;
					}
					buzz();
					break;
				case 4:
					dispState = 4;
					sel_max = 1000;
					break;
			}
			break;
		case 2:
			switch (position){
				case 0:
					dispState = 1;
					sel_max = 4;
					break;
				case 1:
					timerActive = !timerActive;
					buzz();
					break;
				case 2:
					dispState = 3;
					sel_max = 60;
					break;
				case 3:
					timeUnit += 1;
					if (timeUnit == 3) {
						timeUnit = 0;
					}
					buzz();
					break;
			}
			break;
		case 3:
			timeToSet = position;
			dispState = 2;
			sel_max = 3;
			break;
		case 4:
			delayTime = position;
			dispState = 1;
			sel_max = 4;
			break;
	}
}



void drawStatus(){
	u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
	u8g2.drawStr(110, 10, "Y");
	u8g2.drawStr(110, 20, "B");
	if (lampMode == 2){
		u8g2.drawStr(100, 15, "A");
	}else if (lampMode==3){
		u8g2.drawStr(100, 15, "U");
	}

	if (delayTime != 0){
		u8g2.drawStr(8, 15, "Delay: ");
		u8g2.drawStr(39, 15, String(delayTime).c_str());
	}

	if (running){
		u8g2.drawStr(8,10+12*4,"Running");
	}else {
		u8g2.drawStr(8,10+12*4,"Idle");
	}



	u8g2.setFont(u8g2_font_unifont_t_symbols);
	if (lampMode != 1){
		u8g2.drawGlyph(118, 11, 0x25cf);
	}else{
		u8g2.drawGlyph(118, 11, 0x25ce);
	}
	if (lampMode != 0){
		u8g2.drawGlyph(118, 21, 0x25cf);
	}else{
		u8g2.drawGlyph(118, 21, 0x25ce);
	}


}

void selectorBase(){
	u8g2.setFont(u8g2_font_unifont_t_symbols);
	u8g2.drawGlyph(0, 13+position*12, 0x25b8);

	u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
	u8g2.drawStr(8,10,"Back");
}


void drawMainMenu(){
	selectorBase();
	if (running){
		u8g2.drawStr(8,10+12*1,"Stop");
	}else{
		u8g2.drawStr(8,10+12*1,"Start");
	}
	u8g2.drawStr(8,10+12*2,"Timer");
	switch (lampMode)
	{
	case 0:
		u8g2.drawStr(8,10+12*3,"LED Type: Yellow");
		break;
	case 1:
		u8g2.drawStr(8,10+12*3,"LED Type: Blue");
		break;
	case 2:
		u8g2.drawStr(8,10+12*3,"LED Type: Alternate");
		break;
	case 3:
		u8g2.drawStr(8,10+12*3,"LED Type: Unison");
		break;
	}
	u8g2.drawStr(8,10+12*4,"Delay");
	u8g2.drawStr(128-u8g2.getStrWidth(String(delayTime).c_str()),10+12*4, String(delayTime).c_str());
}

void drawTimerMenu(){
	selectorBase();
	if (timerActive){
		u8g2.drawStr(8,10+12*1, "Active: True");
	}else {
		u8g2.drawStr(8,10+12*1, "Active: False");
	}
	u8g2.drawStr(8,10+12*2, "Time value");
	u8g2.drawStr(128-u8g2.getStrWidth(String(timeToSet).c_str()),10+12*2, String(timeToSet).c_str());
	switch (timeUnit){
	case 0:
		u8g2.drawStr(8,10+12*3, "Time unit: Seconds");
		break;
	case 1:
		u8g2.drawStr(8,10+12*3, "Time unit: Minutes");
		break;
	case 2:
		u8g2.drawStr(8,10+12*3, "Time unit: Hours");
		break;
	}
}


void numberSelector(){
	string_center(String(position), 32);
}

