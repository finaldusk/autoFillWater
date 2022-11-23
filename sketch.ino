hw_timer_t * wdtimer = NULL;  //watchdog timer
hw_timer_t * timer = NULL;  //fill water timer
const int wdtTimeout = 1000; //watchdog timeout
const int fillWaterTimeout = 60 * 1000; //fill water timeout

const int highPositionPin = 02; //high water level io
const int lowPositionPin =03; //low water level
const int valuePin =10; //solenoid valve io
const int relayPin =6; //electromagnetic relayio

void ARDUINO_ISR_ATTR resetModule() {
  Serial.println("reboot\n");
  esp_restart();
}

void ARDUINO_ISR_ATTR stopFillWater() {
  fillWater(false);
  timerAlarmDisable(timer);
}

void fillWater(bool on) {
  if(!on){
    digitalWrite(valuePin, LOW);
    digitalWrite(relayPin, LOW);    
  }else{
    digitalWrite(valuePin, HIGH);
    digitalWrite(relayPin, HIGH);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(valuePin, OUTPUT);
  digitalWrite(valuePin, LOW);
  pinMode(highPositionPin, INPUT_PULLUP);
  pinMode(lowPositionPin, INPUT_PULLUP);
  
  //watchdog
  wdtimer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(wdtimer, &resetModule, true);  //attach callback
  timerAlarmWrite(wdtimer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(wdtimer);                          //enable interrupt
	
  //fill water timer
  timer = timerBegin(1, 80, true);                  //timer 1, div 80
  timerAttachInterrupt(timer, &stopFillWater, true);  //attach callback
  timerAlarmWrite(timer, fillWaterTimeout * 1000, false); //set time in us
}

void loop() {
  timerWrite(wdtimer, 0); //reset timer (feed watchdog)
 
  if(digitalRead(highPositionPin)==LOW){
    fillWater(false);
    timerAlarmDisable(timer);
  }else if(digitalRead(lowPositionPin)==LOW){
    fillWater(true);
    timerAlarmEnable(timer);
    timerWrite(timer, 0);
  }
  delay(100); 
}
