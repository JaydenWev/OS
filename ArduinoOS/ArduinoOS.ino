#include <EEPROM.h>
#include "Arduino.h"

#define STACKSIZE 32

byte stack [ STACKSIZE ];       // Stack
byte sp = 0;                    // Stack position
byte memory[256];               //werkgeheugen        tijdelijke variablen
byte noOfVars = 0;
byte startpos = 0;
int systemPid = 0;
int localPid = 0;

typedef struct {
  byte name;
  byte type;
  byte address;
  byte size;
  int pid;
} MEM;

static MEM fileMem [] {           //memory table  voor een variable van een proces hier worden de gegevens opgeslagen voor de variablen die in de ram (memory) staan
  {'a', '\0', '\0', 0, 0},        //name type address size pid
  {'b', '\0', '\0', 0, 0},
  {'c', '\0', '\0', 0, 0},
  {'d', '\0', '\0', 0, 0},
  {'e', '\0', '\0', 0, 0},
  {'f', '\0', '\0', 0, 0},
  {'g', '\0', '\0', 0, 0},
  {'h', '\0', '\0', 0, 0},
  {'i', '\0', '\0', 0, 0},
  {'j', '\0', '\0', 0, 0},
  {'k', '\0', '\0', 0, 0},    
  {'l', '\0', '\0', 0, 0},
  {'m', '\0', '\0', 0, 0},
  {'n', '\0', '\0', 0, 0},
  {'o', '\0', '\0', 0, 0},
  {'p', '\0', '\0', 0, 0},
  {'q', '\0', '\0', 0, 0},
  {'r', '\0', '\0', 0, 0},
  {'s', '\0', '\0', 0, 0},
  {'t', '\0', '\0', 0, 0},
  {'u', '\0', '\0', 0, 0},
  {'v', '\0', '\0', 0, 0},
  {'w', '\0', '\0', 0, 0},
  {'x', '\0', '\0', 0, 0},
  {'y', '\0', '\0', 0, 0},
  {'z', '\0', '\0', 0, 0},        //25
};

typedef struct {
  char name [12];
  char state;
  byte pc;
  byte sp;
  byte fp;
  int pid;
  int startPos;
  byte stack [ STACKSIZE ];
} ProcessTable;

static ProcessTable processes [] {
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0},
};

char message[16] = "";      //wordt gebruikt om de cli waarde op te slaan
char message1[16] = "";
char message2[16] = "";

char message3[128] = "";
char empty [] = "empty";    //gebruikt om makkelijk array te vullen met empty
int p = 0;
char byteRead;

byte b[4];
float *pf = (float *)b;
int storagenr = 0;
int i = 0;
int eeAddress = 0;
int customAddress = 0;
int AddressList = 0;

EERef noOfFiles = EEPROM[1000];

typedef struct {
  char name [12];
  int startPos;     //start positie van de werkelijk data
  int fileLength;   //hoe groot de data is
} FAT;

static FAT file [] {
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
  {"empty", 0, 16},
};

typedef struct {
  char name [ 16 ];
  void * func ;
} commandType ;

static commandType command [] = { // All availlable commands
//  {"ping" , &ping } ,
//  {"pong" , &pong } ,
//  {"store" , &store } ,
//  {"retrieve" , &retrieve } ,
//  {"erase" , &erase } ,
//  {"files" , &files } ,
//  {"freespace" , &freespace } ,
//  {"run" , &runProgram } ,
//  {"list" , &list } ,
//  {"suspend" , &suspendProces } ,
//  {"resume" , &resumeProces } ,
//  {"kill" , &killProcess },
//  {"ce" , &clearEEPROM },
//  {"help" , &help }

};

void setup(){
  Serial.begin(9600);
  Serial.println("ARDUINO READY!");
  sync();
}


void loop (){
  readInput();
  for (int i = 0; i < 10; i++ ) {     //voert per instructie uit voor ieder process dat runnend is
    if (processes[i].state == 'r') {

      execute(i);
    }
  }
}

// #########################################################
// ##################   InputController   ##################
// #########################################################


String checkInput() {
  storagenr = 0;
  for (int n = 0; n <= 13; n++) {
    if (strcmp(command[n].name, message) == 0 && message != "") {
      void (*func)() = command[n].func;
      func();
      p = 0;
      clearBuffers();
      return;
    }
  }
  Serial.println("command NOT recognised, type help for commands.");
  clearBuffers();
}


void readInput() {
  int availableBytes = Serial.available();
  if (availableBytes > 0) {
    byteRead = Serial.read();
    for (int i = 0; i < availableBytes; i++) {
      if (byteRead == ' ') {
        storagenr++;
        p = 0;
      }
      else if (byteRead == '\n') {
        checkInput();
        storagenr = 0;
        clearBuffers();
      } else {
        switch (storagenr) {
          case 0:
            message[p] = byteRead;
            message[p + 1] = '\0';
            break;
          case 1:
            message1[p] = byteRead;
            message1[p + 1] = '\0'; // Append a null
            break;
          case 2:
            message2[p] = byteRead;
            message2[p + 1] = '\0'; // Append a null
            break;
          default:
            message3[p] = byteRead;
            message3[p + 1] = '\0'; // Append a null
            //Serial.println("switch2");
            break;
        }
        p++;
      }
    }
  }
}


void clearBuffers() {
  p = 0;
  for (int l = 0; l <= 15; l++) {
    message[l] = '\0';
    message1[l] = '\0';
    message2[l] = '\0';
  }
  for (int l = 0; l <= 127; l++) {
    message3[l] = '\0';
  }
}

// #########################################################


void pushByte ( byte b, int index ) {
  processes[index].stack [ processes[index].sp++] = b ;
}

byte popByte (int index) {
  return processes[index].stack [--processes[index].sp];
}

byte popByteString (int index, int n) {
  return processes[index].stack [processes[index].sp - n - 1];
}

void execute(int i) {
  int  tempLocation = 0;
  int  tempIndex = 0;
  byte byteArray[128] = "";
  byte tempbyte = 0;
  int n = 0;
  for (int x = processes[i].pc; x < file[i].fileLength; x++) {
    EEPROM.get(file[i].startPos + x, byteArray[x]);
  }
}

void sync () {                                               //syncs data from eeprom with data from struct
  noOfFiles = 0;
  FAT fileS;
  int address = 0;
  for (int t = 0; t < 10; t++) {
    EEPROM.get(address, fileS);
    file[t] = fileS;
    if (strcmp(file[t].name, "empty")) {
      noOfFiles++;
      //Serial.println(noOfFiles);
    }
    address += 16;
  }
}


// #########################################################
// ##########              TestFunctions          ##########
// #########################################################

void help() {
  delay(20);
  Serial.println("");
  Serial.println("---- List of commands ----");
  Serial.println("store\t\t\t#");
  Serial.println("retrieve");
  Serial.println("erase");
  Serial.println("files");
  Serial.println("freespace");
  Serial.println("run [Program]");
  Serial.println("list");
  Serial.println("suspend [Process]");
  Serial.println("resume [Process]");
  Serial.println("kill [Process]");
}
