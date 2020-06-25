//Standard
#define RING_ON_TIME 1500 //ms
#define RING_OFF_TIME 3500 //ms
#define PULSE_WIDTH 100 //ms
#define PULSE_PAUSE 700 //ms
#define NUM_SIZE 3 //Number of digits in number

//Hardware definition
#define N_LINE 3

//Pins definition
#define MASTER_RING A5

typedef struct {
  int selector;
  int ring;
  int negative;
  int positive;
  int state;
} linepin_t;

const linepin_t linePin[N_LINE] = {
  {.selector= 2, .ring= 4, .negative= 6, .positive= 8, .state=A0},
  {.selector= 3, .ring= 5, .negative= 7, .positive= 9, .state=A1},
  {.selector=10, .ring=11, .negative=12, .positive=13, .state=A2}
};

//Lines number
int linesNum[N_LINE][NUM_SIZE] = {
  {5,8,8},//LUV
  {2,3,6},//BEN
  {5,9,6}//LYN
};

//Differents modes of a line
typedef enum {
  WAITING, RINGING, PTT, CONNECTED
} linemode_t;

//True if active, false if hung up
volatile bool linesState[N_LINE] = {false, false, false};

inline void setOutput(int pin){
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
}

void printState(){
  for(int i=0; i<N_LINE; i++){
    Serial.println(String(i) + " is currently " + (linesState[i]?"active":"inactive"));
  }
  Serial.println();
}

void setLineMode(linemode_t lm, int lineA, int lineB=-1);


void setup() {

  //Setting up pins
  for(int i=0; i<N_LINE; i++){
    setOutput(linePin[i].selector);
    setOutput(linePin[i].ring);
    setOutput(linePin[i].negative);
    setOutput(linePin[i].positive);
    pinMode(linePin[i].state, INPUT_PULLUP);
  }
  setOutput(MASTER_RING);

  Serial.begin(115200);
  Serial.println("Small RTC, at your service !");
}

void loop() {
  int caller = -1;
  int called = -1;

  //printState();
  //delay(500);
  
  caller = getActiveLine();
  //A line is active !
  if(caller != -1){
    //Hello there
    Serial.println(String(caller) + " is here !");
    setLineMode(PTT, caller);
    delay(500); //waiting music
  
    //Dialling
    called = getDialedLine(caller);
    if(called != -1){
      Serial.println(String(caller) + " is calling " + String(called));
    
      //Ringing
      ringUntilResponse(called);

      if((linesState[caller] && linesState[called])){
        //Connect them
        Serial.println("connecting...");
        setLineMode(CONNECTED, caller, called);
        //Wait until end of the conversation
        delay(100);
        while(linesState[caller]){ //Because of the loop, we only need (and have to) check the caller
          refreshLinesState();
        }
      }

      //End
      Serial.println("End of the conversation");
      resetLines();
      
    } else
      Serial.println("Wrong number ?");
  }

}


void resetLines(){
  for(int i=0; i<N_LINE; i++){
    setLineMode(WAITING, i);
  }
}

bool refreshLinesState(){
  bool hasChanged=false;
  
  for(int i=0; i<N_LINE; i++){
    //Line is considered active if the maximum duration of time HIGH (inactive) is less than 10ms (50Hz).
    //This is like that to prevent false detect introduced by the ringing.
    unsigned long highTime = pulseIn(linePin[i].state, HIGH, 50e3);
    bool active = (highTime==0) ? (!digitalRead(linePin[i].state)) : (highTime < 10e3);
    //Serial.println(String(i) + " is high for "+String(highTime)+"us -> =0? "+String(highTime==0)+"  state: "+String(!digitalRead(linePin[i].state))+"  ==>"+String(active));
    if(linesState[i] != active)
      hasChanged = true;
    linesState[i] = active;
  }
  
  return hasChanged;
}

int getActiveLine(){
  int active = -1;
  
  refreshLinesState();
  
  for(int i=0; i<N_LINE; i++){
    if(linesState[i] == true)
      active = i;
  }

  return active;
}

void setLineMode(linemode_t lm, int lineA, int lineB){
  switch(lm){
    case WAITING:
      digitalWrite(linePin[lineA].selector, LOW);
      digitalWrite(linePin[lineA].ring, LOW);
      digitalWrite(linePin[lineA].negative, LOW);
      digitalWrite(linePin[lineA].positive, LOW);
    break;
    case RINGING:
      digitalWrite(linePin[lineA].selector, LOW);
      digitalWrite(linePin[lineA].ring, HIGH);
      digitalWrite(linePin[lineA].negative, LOW);
      digitalWrite(linePin[lineA].positive, LOW);
    break;
    case PTT:
      digitalWrite(linePin[lineA].selector, HIGH);
      digitalWrite(linePin[lineA].ring, LOW);
      digitalWrite(linePin[lineA].negative, LOW);
      digitalWrite(linePin[lineA].positive, HIGH);
    break;
    case CONNECTED:
      digitalWrite(linePin[lineA].selector, HIGH);
      digitalWrite(linePin[lineA].ring, LOW);
      digitalWrite(linePin[lineA].negative, LOW);
      digitalWrite(linePin[lineA].positive, HIGH);
      
      digitalWrite(linePin[lineB].selector, HIGH);
      digitalWrite(linePin[lineB].ring, LOW);
      digitalWrite(linePin[lineB].negative, HIGH);
      digitalWrite(linePin[lineB].positive, LOW);
    break;
  }
}


int getDialedDigit(int caller){
  bool done=false;
  long tps;
  int number=0;

  //Wait until begin of dialing
  while(!digitalRead(linePin[caller].state));
  delay(PULSE_WIDTH/8); //Force blind to avoid bouncing detection
  
  while(not done){
    tps=millis();
    
    number++;

    //HIGH state
    while(digitalRead(linePin[caller].state) && not done)
      done = millis() > (tps+PULSE_PAUSE);
    delay(PULSE_WIDTH/8); //Force blind to avoid bouncing detection

    //LOW state
    while(!digitalRead(linePin[caller].state) && not done)
      done = millis() > (tps+PULSE_PAUSE);
    delay(PULSE_WIDTH/8); //Force blind to avoid bouncing detection
  }

  if(number == 10) number=0;
  
  return number;
}

int searchPhoneBook(int number[NUM_SIZE]){
  for(int line=0; line<N_LINE; line++){
    
    bool match=true;
    for(int i=0; i<NUM_SIZE; i++){
      if(number[i] != linesNum[line][i]){
        match=false;
        break;
      }
    }
  
    if(match)
      return line;  
  }
  
  return -1;
}

int getDialedLine(int caller){
  int called = -1;
  int number[NUM_SIZE] = {0};

  //called = (caller+1)%N_LINE;
  for(int i=0; i<NUM_SIZE; i++){
    number[i] = getDialedDigit(caller);
    Serial.print(number[i]);
  }
  Serial.println();

  called = searchPhoneBook(number);

  return called;
}


void ringUntilResponse(int line){
  bool hasChanged=false;
  bool ringing=false;
  long tps = millis();
  
  //refreshLinesState();
  setLineMode(RINGING, line);
  while(not hasChanged)
  {
    ringing = millis() < (tps+RING_ON_TIME);
    if(millis() > (tps+RING_ON_TIME+RING_OFF_TIME))
      tps = millis();

    hasChanged = refreshLinesState();
    
    digitalWrite(MASTER_RING, ringing);
  }
  digitalWrite(MASTER_RING, LOW);
  setLineMode(WAITING, line);
}
