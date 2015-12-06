extern "C" {
#include "user_interface.h"
}

os_timer_t myTimer1;
os_timer_t myTimer2;
os_timer_t myTimer3;

//boolean to activate timer events
bool tick1Occured;
bool tick2Occured;
bool tick3Occured;

//Need to have a global address for the timer id
const char tickId1=1;
const char tickId2=2;
const char tickId3=3;

//Functions declaration
void timerCallback(void *);
void timerInit(os_timer_t *, uint32_t, os_timer_func_t *);


void setup() {
  
  //Start Serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("   ESP8266 Timer Test");
  Serial.println("--------------------------");
 

  //Init and start timers
  tick1Occured = false;
  tick2Occured = false;
  tick3Occured = false;
  timerInit(&myTimer1, 1000,     timerCallback, (char*)&tickId1);
  timerInit(&myTimer2, 5000,     timerCallback, (char*)&tickId2);
  timerInit(&myTimer3, 1000*60,  timerCallback, (char*)&tickId3);

}

void loop() {

  //Check if a timer occured to execute the action
  if (tick1Occured == true){
    Serial.println("Tick 1s Occurred");
    tick1Occured = false;
  }
  if (tick2Occured == true){
    Serial.println("Tick 5s Occurred");
    tick2Occured = false;
  }
  if (tick3Occured == true){
    Serial.println("Tick 1m Occurred");
    tick3Occured = false;
  }
  //Give th time to th os to do his things
  yield();  // or delay(0);
}

// start of timerCallback
void timerCallback(void *pArg) {
  
  char timerId = *(char*)pArg; //Value inside (*) of pArg, casted into a char pointer
  
  switch (timerId){
    case 1 :
      tick1Occured = true;
      break;
    case 2 :
      tick2Occured = true;
      break;
    case 3 :
      tick3Occured = true;
      break;
    default :
      //Nothings to do
      break;
  }
} 

void timerInit(os_timer_t *pTimerPointer, uint32_t milliSecondsValue, os_timer_func_t *pFunction, char *pId) {
 /*
  Maximum 7 timers
  os_timer_setfn - Define a function to be called when the timer fires
  void os_timer_setfn(os_timer_t *pTimer, os_timer_func_t *pFunction, void *pArg)
  
  Define the callback function that will be called when the timer reaches zero. 
  The pTimer parameters is a pointer to the timer control structure.
  The pFunction parameters is a pointer to the callback function.
  The pArg parameter is a value that will be passed into the called back function. 
  The callback function should have the signature: void (*functionName)(void *pArg)
  The pArg parameter is the value registered with the callback function.
*/
  os_timer_setfn(pTimerPointer, pFunction, pId);
/*
  os_timer_arm -  Enable a millisecond granularity timer.
  void os_timer_arm( os_timer_t *pTimer, uint32_t milliseconds, bool repeat)

  Arm a timer such that is starts ticking and fires when the clock reaches zero.
  The pTimer parameter is a pointed to a timer control structure.
  The milliseconds parameter is the duration of the timer measured in milliseconds. 
  The repeat parameter is whether or not the timer will restart once it has reached zero.
*/
  os_timer_arm(pTimerPointer, milliSecondsValue, true);
} // End of user_init

