#include <EEPROM.h>
#include "instruction_set.h"
#include "test_programs.h"

#define STACKSIZE 32

byte stack [ STACKSIZE ];       // Stack
byte memory[256];               //RAM - werkgeheugen
byte noOfVars = 0;
int systemPid = 0;
int localPid = 0;
byte sp = 0;
byte startpos = 0;

String screenBorder = "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■";

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

// Memory struct and table
typedef struct {
  byte name;
  byte type; // Char, Int, Float, String
  byte address;
  byte size;
  int pid;
} MEM;

static MEM fileMem [] {             // Voor een variable van een proces hier worden de gegevens opgeslagen voor de variablen die in de ram (memory) staan
  // Name type address size pid
  {'a', '\0', '\0', 0, 0},          // Name type address size pid
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
  byte lp;
  byte fp;
  int pid;
  int startPos;
  byte stack [ STACKSIZE ];
} ProcessTable;

static ProcessTable processes [] {
  // name, state, pc, sp, fp, pid, startPos
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
  {"", 0, 0, 0, 0, 0, 0, 0},
};

void setup() {
  Serial.begin(9600);
  Serial.println(F(" "));
  Serial.println(screenBorder);
  Serial.println(F("■ Starting...\t\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
  sync();
  //  help(); // Print available commands
  //   writeFATEntryCustomByte("prog1", sizeof(prog1), prog1);
  //   writeFATEntryCustomByte("prog2", sizeof(prog2), prog2);
  //   writeFATEntryCustomByte("prog3", sizeof(prog3), prog3);
  //   writeFATEntryCustomByte("prog4", sizeof(prog4), prog4);
  //   writeFATEntryCustomByte("loopt", sizeof(loopt), loopt);

}

void loop () {
  readInput();
  for (int i = 0; i < 10; i++ ) {
    if (processes[i].state == 'r') {  // Is process is running, execute it
      execute(i);
    }
  }
  delay(100);
}

void sync () { // Syncs data from eeprom with data from struct
  noOfFiles = 0;
  FAT fileS;
  int address = 0;
  for (int t = 0; t < 10; t++) {
    EEPROM.get(address, fileS);
    file[t] = fileS;
    if (strcmp(file[t].name, "empty")) {
      noOfFiles++;
    }
    address += 16;
  }
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

void execute(int i) {
  int  tempLocation = 0;
  int  tempIndex = 0;
  byte byteArray[128] = "";
  byte tempbyte = 0;
  int n = 0;
  for (int x = processes[i].pc; x < file[i].fileLength; x++) {
    EEPROM.get(file[i].startPos + x, byteArray[x]);
  }


  switch (byteArray[processes[i].pc]) {
    case CHAR:
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      pushByte(CHAR, i);
      break;
    case INT:
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      pushByte(INT, i);
      break;
    case STRING:
      tempbyte = byteArray[processes[i].pc];
      while (tempbyte != 0) {
        pushByte(byteArray[processes[i].pc], i);
        processes[i].pc++;
        tempbyte = byteArray[processes[i].pc];
        n++;            // Stack position counter - Length of string
      }
      pushByte(byteArray[processes[i].pc], i);
      pushByte(n, i);
      pushByte(STRING, i);

      break;
    case FLOAT:
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      processes[i].pc++;
      pushByte(byteArray[processes[i].pc], i);
      pushByte(FLOAT, i);
      break;
    case SET:
      //pop de bovenste voor het type

      tempbyte = popByte(i);
      if (tempbyte == STRING) {
        processes[i].pc++;
        tempbyte = popByte(i);
        tempLocation = designateeMemoryfileLoc(tempbyte);
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);
        fileMem[tempIndex].size = tempbyte;
        fileMem[tempIndex].type = STRING;
        fileMem[tempIndex].address = tempLocation;
        for (int p = 0; p < tempbyte; p++) {
          memory[tempLocation + p] = popByteString(i, tempbyte - p);
        }
      }
      else if (tempbyte == INT) {
        processes[i].pc++;
        tempbyte = popByte(i);
        tempLocation = designateeMemoryfileLoc(tempbyte);
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);
        fileMem[tempIndex].size = 2;
        fileMem[tempIndex].type = INT;
        fileMem[tempIndex].address = tempLocation;
        memory[tempLocation + 1] =  tempbyte;
        memory[tempLocation] =  popByte(i);
      }


      else if (tempbyte == CHAR) {        //aanpassen
        tempbyte = popByte(i);
        processes[i].pc++;
        tempLocation = designateeMemoryfileLoc(tempbyte);
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);
        fileMem[tempIndex].size = 1;
        fileMem[tempIndex].type = CHAR;
        fileMem[tempIndex].address = tempLocation;
        memory[tempLocation] =  tempbyte;
      }


      else if (tempbyte == FLOAT) {    //aanpassen
        processes[i].pc++;
        tempbyte = popByte(i);
        tempLocation = designateeMemoryfileLoc(tempbyte);
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);
        fileMem[tempIndex].size = 4;
        fileMem[tempIndex].type = FLOAT;
        fileMem[tempIndex].address = tempLocation;
        memory[tempLocation + 3] =  tempbyte;
        memory[tempLocation + 2] =  popByte(i);
        memory[tempLocation + 1] =  popByte(i);
        memory[tempLocation] =  popByte(i);
      }
      break;
    case GET:
      processes[i].pc++;
      tempbyte = byteArray[processes[i].pc];
      tempLocation = returnMemoryIndex(byteArray[processes[i].pc], processes[i].pid);
      tempIndex = returnFtIndex((char)byteArray[processes[i].pc], processes[i].pid);
      tempbyte = fileMem[tempIndex].type;
      //tempbyte bevat het type

      if (tempbyte == STRING) { //

        for (int l = 0; l < fileMem[tempIndex].size; l++ ) {
          pushByte(memory[fileMem[tempIndex].address + l + 1], i);
        }
        pushByte(fileMem[tempIndex].size, i);
        pushByte(fileMem[tempIndex].type, i);
      } else if (tempbyte == INT) {
        pushByte(memory[fileMem[tempIndex].address], i);
        pushByte(memory[fileMem[tempIndex].address + 1], i);
        pushByte(fileMem[tempIndex].type, i);
      } else if (tempbyte == CHAR) {
        pushByte(memory[fileMem[tempIndex].address], i);
        pushByte(fileMem[tempIndex].type, i);
      } else if (tempbyte == FLOAT) {
        pushByte(memory[fileMem[tempIndex].address], i);
        pushByte(memory[fileMem[tempIndex].address + 1], i);
        pushByte(memory[fileMem[tempIndex].address + 2], i);
        pushByte(memory[fileMem[tempIndex].address + 3], i);
        pushByte(fileMem[tempIndex].type, i);
      }

      break;
    case INCREMENT:
      Serial.println("INCREMENT");
      break;
    case DECREMENT:
      Serial.println("DECREMENT");
      break;
    case DELAYUNTIL:
      Serial.println("DELAYUNTIL");
      break;
    case MILLIS:
      Serial.println("MILLIS");
      break;
    case PRINT:
      char x;
      tempbyte = popByte(i);
      if (tempbyte == STRING) {
        tempbyte = popByte(i);
        for (int p = 0; p < tempbyte; p++) {
          x = popByteString(i, tempbyte - p);
          if ((x >= 65 && x <= 90) || (x >= 97 && x <= 122) || x == 32 )
            Serial.print(x);
        }
      } else if (tempbyte == INT) {
        tempbyte = popByte(i);
        Serial.print(tempbyte + popByte(i) * 256);
      } else if (tempbyte == CHAR) {
        Serial.print((char)popByte(i));
      } else if (tempbyte == FLOAT) {
        b[0] = popByte(i);
        b[1] = popByte(i);
        b[2] = popByte(i);
        b[3] = popByte(i);
        Serial.print(*pf);
      }

      processes[i].sp = processes[i].sp - tempbyte - 1;
      break;
    case PRINTLN:
      tempbyte = popByte(i);
      if (tempbyte == STRING) {
        tempbyte = popByte(i);
        for (int p = 0; p < tempbyte; p++) {
          x = popByteString(i, tempbyte - p);
          if ((x >= 65 && x <= 90) || (x >= 97 && x <= 122) || x == 32 )
            Serial.print(x);
        }
      } else if (tempbyte == INT) {
        tempbyte = popByte(i);
        Serial.print(tempbyte + popByte(i) * 256);
      } else if (tempbyte == CHAR) {
        Serial.print((char)popByte(i));
      } else if (tempbyte == FLOAT) {
        b[0] = popByte(i);
        b[1] = popByte(i);
        b[2] = popByte(i);
        b[3] = popByte(i);
        Serial.print(*pf);
      }

      processes[i].sp = processes[i].sp - tempbyte - 1;
      Serial.println("");
      break;
    case LOOP:
      processes[i].lp = processes[i].pc;
      break;
    case ENDLOOP:
      processes[i].pc = processes[i].lp;
      processes[i].state = 'r';
      break;
    case STOP:

      erasePidFiles(processes[i].pid);
      processes[i].state = 0;
      processes[i].pid = 0;
      processes[i].fp = 0;
      processes[i].sp = 0;

      localPid--;
      processes[i].pc = 0;
      processes[i].startPos = 0;
      for (int o = 0; o < 12; o++)        //kan wss weg
        processes[i].name[o] = '\0';

      for (int o = 0; o < STACKSIZE; o++) {       //kan wss weg
        processes[i].stack[ o ] = '\0';
      }
      break;
  }
  processes[i].pc++;
}
// #########################################################
// ##########              TestFunctions          ##########
// #########################################################

void help() {
  delay(20);
  Serial.println();
  Serial.println(F("■■■■■ List of command ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"));
  Serial.println(F("■\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ store\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ retrieve\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ erase\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ files\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ freespace\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ list\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ run \t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ suspend \t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ resume \t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ kill \t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■ pe \t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(F("■\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
}

void ping() {
  Serial.println();
  Serial.println(screenBorder);
  Serial.println(F("■ pong\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
}

void pong() {
  Serial.println();
  Serial.println(screenBorder);
  Serial.println(F("■ ping\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
}
void numberOfFiles() {
  Serial.println();
  Serial.println(screenBorder);
  Serial.print(F("■ "));
  Serial.print(noOfFiles);
  Serial.println(F("\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
}

void clearEEPROM () {
  FAT fileN;
  //clears the eeprom
  Serial.println(F("■ please wait\t\t\t\t\t\t\t\t\t■"));
  for (int y = 0 ; y < EEPROM.length() ; y++) {
    EEPROM.write(y, 0);
  }
  eeAddress = 0;

  for (int y = 0 ; y < 11 ; y++) {
    for (int s = 0; s < 12; s++) {
      file[y].name[s] = empty[s];
    }
    file[y].startPos = 0;
    file[y].fileLength = 16;
    fileN = file[y];
    EEPROM.put(eeAddress, fileN);
    eeAddress += 16;
  }
  clearBuffers();
  Serial.println(F("■ Cleared EEPROM\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
  noOfFiles = 0;
  customAddress = 0;
}

void printE() {
  int tempValue;
  Serial.print(F("■"));
  for (int i; i < 1024; i++) {
    tempValue = EEPROM.read(i);
    Serial.print(tempValue);
    Serial.print(" ");
    if (((i + 1) % 24) == 0) {
      Serial.print(F("\n■ "));
    }
    else if (((i + 1) % 8) == 0) {
      Serial.print(F("\t"));
    }
  }
  Serial.println(screenBorder);
}

// #########################################################
// ##################   InputController   ##################
// #########################################################

typedef struct {
  char name [ 16 ];
  void * func ;
} commandType ;

static commandType command [] = { // All commands
  {"help", &help },
  {"store" , &store },
  {"erase" , &erase },
  {"retrieve" , &retrieve },
  {"files" , &files },
  {"ls" , &files },
  {"freespace" , &freespace }, // Show available space
  {"run" , &runProgram },
  {"list" , &list },
  {"suspend" , &suspendProces },
  {"resume" , &resumeProces },
  {"kill" , &killProcess},
  {"ce" , &clearEEPROM },
  {"no" , &numberOfFiles },
  {"pe", &printE }, // Print all EEprom values
};

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
  Serial.println();
  Serial.println(screenBorder);
  Serial.println(F("■ command NOT recognised, type help for commands.\t\t\t\t■"));
  Serial.println(screenBorder);
  clearBuffers();
}

// #########################################################
// ##################      Functions      ##################
// #########################################################

void store () { //opslaan van een bestand
  if (message1[0] != '\0' && message2[0] != '\0') {
    if (readFATEntry(message1) == -1) {
      //Serial.println(atoi(message2));
      writeFATEntryCustom(message1, atoi(message2), message3);
    } else {
      Serial.println(F("■ file name already exists\t\t\t\t\t\t\t■"));
    }
  } else {
    Serial.println(F("■ store [name] [size] [data]\t\t\t\t\t\t\t■"));
  }
  Serial.println(screenBorder);
}

void erase () { //Get value from EEPROM
  Serial.println();
  Serial.println(screenBorder);
  if (message1[0] != '\0')
    for (int y = 0; y < noOfFiles; y++) {
      if (!strcmp(file[y].name, message1)) { // When given name is equal to the name in the struct
        Serial.print(F("■ Erased: "));
        Serial.println(message1);
        Serial.println(screenBorder);
        for (int s = 0; s < 12; s++) {
          file[y].name[s] = empty[s];
        }
        file[y].startPos = 0;
        file[y].fileLength = 16;
        EEPROM.put(y * 16, file[y]);
        sync();
        return;
      }
    }
  else {
    Serial.println(F("■ erase [fileName]\t\t\t\t\t\t\t\t■"));
    Serial.println(screenBorder);
  }
}

void retrieve () {   // retrieved een bestand (werkt alleen niet op byte arrays dus de test programmas)
  Serial.println();
  Serial.println(screenBorder);
  if (message1[0] != '\0') {
    Serial.print(F("■ file: "));
    Serial.print(message1);
    Serial.println(", contains:");
    for (int index = 0; index < noOfFiles; index++) {
      if (!strcmp(file[index].name, message1)) {
        clearBuffers();
        EEPROM.get(file[index].startPos, message3);
        Serial.print(F("■ "));
        Serial.println(message3);
        Serial.println(screenBorder);
        return;
      }
    }
    Serial.println(F("■ Filename not found"));
    Serial.println(screenBorder);
  }
  else {
    Serial.println(F("■ retrieve [fileName]\t\t\t\t\t\t\t\t■"));
    Serial.println(screenBorder);
  }
}

void files () { //print de files die op de eeprom staan
  Serial.println();
  Serial.println(screenBorder);
  Serial.print(F("■ files: \t"));
  Serial.print(noOfFiles);
  Serial.println(F("\t\t\t\t\t\t\t\t■"));
  FAT fileX;
  for (int s = 0; s < 10; s++) {
    if (strcmp(file[s].name, "empty")) {
      Serial.print(F("■ "));
      Serial.print( file[s].name);
      Serial.print(F("\tSize (byte): "));
      Serial.print( file[s].fileLength);
      Serial.print(F("\t"));
      Serial.println( file[s].startPos);
    }
  }
  Serial.println(F("■\t\t\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
}

void list () {
  Serial.println();
  Serial.println(screenBorder);
  Serial.print(F("■ active processes: "));
  Serial.println(localPid);
  for (int index = 0; index < 10; index++) {
    if (strcmp(processes[index].name, "")) {
      Serial.print(F("■ |\t"));
      Serial.print( processes[index].name);
      Serial.print(F("\t pid: "));
      Serial.print( processes[index].pid);
      Serial.print(F("\t state: "));
      //      Serial.println(processes[index].state);
      if (processes[index].state == 'r')
        Serial.println(F("Running"));
      else if (processes[index].state == 'p')
        Serial.println(F("Paused"));
      else if (processes[index].state == '0')
        Serial.println(F("Terminated"));
      else {
        Serial.print(F("'error:"));
        Serial.print(processes[index].state);
        Serial.println(F("'"));
      }
    }
  }
  Serial.println(screenBorder);
}

void freespace () {     //print hoeveel ruimte er over is
  int freespace = 1024 - 160;
  for (int s = 9; s > 0; s--) {
    //Serial.println(s);
    if (strcmp(file[s].name, "empty")) {
      freespace = freespace - file[s].fileLength;
    }
  }
  Serial.print(F("■ "));
  Serial.print(freespace);
  Serial.println(F("(byte) available\t\t\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
}

void runProgram () {
  int h;
  boolean isNameCorrect = false;
  char tempName[16] = "";
  int tempPos = 0;
  if (message1[0] != '\0' ) {
    for (int t = 0; t < 10; t++) {
      if (!strcmp(file[t].name, message1)) { // If file exists
        for (int i = 0; i < strlen(file[t].name); i++)
          tempName[i] = file[t].name[i];
        isNameCorrect = true;
        h = t;
        tempPos = file[t].startPos;
      }
    }
    for (int t = 0; t < 10; t++) {
      if (processes[t].pid == 0 && isNameCorrect) { // If name is correct and value is empty
        systemPid++;
        localPid++;
        for (int i = 0; i < 12; i++) {
          processes[t].name[i] = tempName[i];
        }
        processes[h].state = 'r';
        processes[h].startPos = file[h].startPos;
        processes[h].pc = tempPos; // Startpos of fat-item
        processes[h].sp = 0;
        processes[h].fp = t;
        processes[h].pid = systemPid;
        Serial.print(F("■ "));
        Serial.print(processes[t].name);
        Serial.println(F(" - Started"));
        Serial.println(screenBorder);
        return;
      }
    }
    Serial.println(F("■ an Error occured please retry"));
    Serial.println(screenBorder);
  }
  else {
    Serial.println(F("■ run [Program]\t\t\t\t\t\t\t\t\t■"));
    Serial.println(screenBorder);
  }
}

void suspendProces () {
  Serial.println();
  Serial.println(screenBorder);
  if (message1[0] == '\0' || message2[0] == '\0') {
    Serial.println(F("■ suspend [processName] [pid]\t\t\t\t\t\t\t■"));
    Serial.println(screenBorder);
    return;
  }
  for (int t = 0; t < sizeof(processes); t++) { // For every item in processes
    if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'p' && processes[t].pid == atoi(message2)) {
      Serial.print(F("■ "));
      Serial.print(message1);
      Serial.println(F(" already suspended"));
      Serial.println(screenBorder);
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'r' && processes[t].pid == atoi(message2)) { // && processes[t].pid == message2[0]
      processes[t].state = 'p';
      Serial.print(F("■ "));
      Serial.print(message1);
      Serial.println(F(" suspended"));
      Serial.println(screenBorder);
      return;
    }
  }
  Serial.println(F("■ Name or pid combination non existent\t\t\t\t\t\t■"));
  Serial.println(screenBorder);
  return;
}

void resumeProces () {
  Serial.println();
  Serial.println(screenBorder);
  if (message2[0] == '\0') {
    Serial.println(F("■ suspend [processName] [pid]\t\t\t\t\t\t\t■"));
    Serial.println(screenBorder);
    return;
  }
  for (int t = 0; t < 10; t++) {
    if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'r' && processes[t].pid == atoi(message2)) {
      Serial.print(F("■ "));
      Serial.print(message1);
      Serial.println(F(" already running"));
      Serial.println(screenBorder);
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'p' && processes[t].pid == atoi(message2)) {
      processes[t].state = 'r';
      Serial.print(F("■ "));
      Serial.print(message1);
      Serial.println(F(" Resumed"));
      Serial.println(screenBorder);
      return;
    }
  }
  Serial.println(F("name and pid combination not extitent"));
  return;
}

void killProcess () {
  Serial.println();
  Serial.println(screenBorder);
  if (!strcmp(message1, "all")) {
    for (int t = 0; t < 10; t++) {
      processes[t].state = '0';
    }
    Serial.println(F("■ All processes terminated"));
    Serial.println(screenBorder);
    return;
  }
  if (message2[0] == '\0') {
    Serial.println(F("■ suspend [processName] [pid]\t\t\t\t\t\t\t■"));
    Serial.println(screenBorder);
    return;
  }
  for (int t = 0; t < 10; t++) {
    if (!strcmp(processes[t].name, message1 ) && processes[t].state == '0' && processes[t].pid == atoi(message2)) {
      Serial.print(F("■ "));
      Serial.print(message1);
      Serial.println(F(" already terminated"));
      Serial.println(screenBorder);
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state != '0' && processes[t].pid == atoi(message2)) {
      erasePidFiles(processes[i].pid);
      processes[t].state = 0;
      processes[t].pid = 0;
      processes[t].fp = 0;
      processes[t].sp = 0;

      localPid--;
      processes[t].pc = 0;
      processes[t].startPos = 0;
      for (int o = 0; o < 12; o++)        //kan wss weg
        processes[t].name[o] = '\0';

      for (int o = 0; o < STACKSIZE; o++) {       //kan wss weg
        processes[t].stack[ o ] = '\0';
      }
      Serial.print(F("■ "));
      Serial.print(message1);
      Serial.println(F(" terminated"));
      Serial.println(screenBorder);
      erasePidFiles(processes[t].pid);
      return;
    }
  }
  Serial.println("name or pid combination not extitent");
  Serial.println(screenBorder);
  return;
}

// #########################################################
// ##################    HelpFunctions    ##################
// #########################################################

// Memory functions
int designateeMemoryfileLoc (int fileSize) {                                            //returns location of empty space in EEprom
  int locCounter = 0;
  for (int t = 0; t < 25; t++) {
    //Serial.println(file[t + 1].startPos - memory[t - 1].fileLength - memory[t - 1].startPos - fileSize);
    if (fileMem[t].pid == '\0') { // Is empty
      if (fileMem[t + 1].address - fileMem[t - 1].size - fileMem[t - 1].address - fileSize  >= 0 || fileMem[t + 1].address == '\0') {
        return fileMem[t - 1].address + fileMem[t - 1].size;
      }
    }
  }
  return 99;
}

void pushByte ( byte b, int index ) {
  processes[index].stack [ processes[index].sp++] = b ;
}

byte popByte (int index) {
  return processes[index].stack [--processes[index].sp];
}

byte popByteString (int index, int n) {
  return processes[index].stack [processes[index].sp - n - 1];
}


int designateMemFile () { //returns location of empty file
  for (int t = 0; t < 10; t++) {
    if (!strcmp(file[t].name, "empty")) {
      return t;
    }
  }
  return 0;
}

int designateMemLoc (int fileSize) { // Returns location of empty space in EEprom
  int locCounter = 160;
  for (int t = 0; t < 10; t++) {
    if (file[t].startPos == 0) { // When struct does not hold a value
      if (t == 0) {                     // kijk dan of het de eerste is dan geef 160 mee
        return 160;
      } else if (file[t + 1].startPos - file[t - 1].fileLength - file[t - 1].startPos - fileSize  >= 0 || file[t + 1].startPos <= 160) {
        return file[t - 1].startPos + file[t - 1].fileLength;
      } else {
        return 160;
      }
    }
  }
}
// ???
int designateMemoryfileNr (byte givenName, int givenPid) {
  for (int t = 0; t < 25; t++) {                                    //verander naar variables
    if (fileMem[t].name == givenName && fileMem[t].pid == givenPid) {
      // Serial.println("name w/ pid already exists, overwriting now");
      //Serial.println(t);
      return t;
    }
  }
  for (int t = 0; t < 25; t++) {
    if (fileMem[t].address == 0 && fileMem[t].type == '\0') {
      //Serial.println("name w/ pid does not exist yet, creating now");
      fileMem[t].name = givenName;
      fileMem[t].pid = givenPid;
      return t;
    }
  }
  return 0;
}

void erasePidFiles(int givenPid) {
  for (int i = 0; i < 25; i++) {
    if (fileMem[i].pid == givenPid) {
      fileMem[i].name = 97 + i;   //alphabetische volgorde
      fileMem[i].type = '\0';
      //fileMem[i].address = '\0';
      fileMem[i].size = 0;

      noOfVars--;
    }
  }
  fileMem[i].pid = 0;
}

void eraseMemFiles(int givenPid, byte giveName) {
  for (int i = 0; i < 25; i++) {
    if (fileMem[i].pid == givenPid && fileMem[i].name == giveName) {
      fileMem[i].name = 97 + i;   //alphabetische volgorde
      fileMem[i].type = '\0';
      fileMem[i].address = '\0';
      fileMem[i].size = 0;

      noOfVars--;
      erasePidFiles(givenPid);
      return;
    }
  }
}

// File allocation table functions
void writeFATEntryCustom (char customName[], int customSize, char customData[]) {
  if (strlen(customData) <= customSize && strlen(customName) <= 12 && customSize <= 128 && customSize > 0) {
    FAT fileC;
    int emptyfileloc = designateMemFile(); // Returns which file is empty
    int memloc = designateMemLoc(strlen(customName)); // Returns location in eeprom which is available
    for (int c = 0; c < 11 ; c++)
      file[emptyfileloc].name[c] = '\0';
    for (int c = 0; c < strlen(customName) ; c++)
      file[emptyfileloc].name[c] = customName[c];
    file[emptyfileloc].startPos = memloc;
    file[emptyfileloc].fileLength = customSize;

    EEPROM.put(emptyfileloc * 16, file[emptyfileloc]); // Header file alloceren

    for (int i = 0; i < strlen(customData); i++) {
      EEPROM.put(memloc + i, customData[i]);
      //Serial.println(customData[i]);
    }
    Serial.print(F("■ Name: "));
    Serial.println(message1);
    Serial.print(F("■ Size: "));
    Serial.println(customSize);
    Serial.print(F("■ Data: "));
    Serial.println(customData);
    Serial.println(F("■ STORED\t\t\t\t\t\t\t\t\t■"));
    noOfFiles++;
    clearBuffers();
  } else {
    Serial.println(F("■ Given size is smaller then given data\t\t\t\t\t\t■"));
    Serial.println(F("■ Or given name is bigger than 12\t\t\t\t\t\t■"));
    Serial.println(F("■ Or given size is bigger than 16\t\t\t\t\t\t■"));
  }
}

int readFATEntry(char givenName[]) {
  FAT fileX;
  for (int s = 0; s < noOfFiles; s++) {
    EEPROM.get(AddressList, fileX);
    if (strcmp(fileX.name, givenName)) {
      AddressList += fileX.fileLength;
    } else {
      s = 21;
      return fileX.startPos;
    }
  }
  AddressList = 0;
  return -1;
}


// !!!
int returnMemoryIndex (byte givenName, int givenPid) {      //geeft address terug van de file in de memory
  //Serial.println((char)givenName);
  //Serial.println(givenPid);
  int memnr = 0;
  for (int i = 0; i < 25; i++) {
    if (fileMem[i].name == givenName && fileMem[i].pid == givenPid) {   //nu weten we de index van de file table
      memnr = fileMem[i].address;
    }
  }
  return memnr;
}

// !!!
int returnFtIndex (byte givenName, int givenPid) {      // uReturns index of fileTable
  int memnr = 0;
  for (int i = 0; i < 25; i++) {
    if (fileMem[i].name == givenName && fileMem[i].pid == givenPid) {   //nu weten we de index van de file table
      memnr = i;
    }
  }
  return memnr;
}
// !!!
void writeFATEntryCustomByte(char customName[], int customSize, byte customData[]) {
  int emptyfileloc = designateMemFile();         // Returns which file is empty
  int memloc = designateMemLoc(customSize);  //?????????
  //Serial.println("323232");
  //Serial.println(memloc);
  for (int c = 0; c < strlen(customName) ; c++)
    file[emptyfileloc].name[c] = customName[c];
  file[emptyfileloc].startPos = memloc;
  file[emptyfileloc].fileLength = customSize;
  EEPROM.put(emptyfileloc * 16, file[emptyfileloc]);      // Allocate header file
  for (int i = 0; i < customSize; i++ )
    EEPROM.put(memloc + i, customData[i]);
  //put data
  //
  noOfFiles++;
}
