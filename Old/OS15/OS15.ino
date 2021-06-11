#include <EEPROM.h>
#include "Arduino.h"
#include "instruction_set.h"
#include "test_programs.h"

#define STACKSIZE 32

byte stack [ STACKSIZE ];         //stack
byte sp = 0;
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

static MEM fileMem [] {             //memory table         voor een variable van een proces hier worden de gegevens opgeslagen voor de variablen die in de ram (memory) staan

  {'a', '\0', '\0', 0, 0},          //name type address size pid
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



void setup() {

  Serial.begin(9600);
  Serial.println("ARDUINO READY!");
  sync();
   //functie om byte array erop te zetten in de eeprom
//  writeFATEntryCustomByte("prog1", sizeof(prog1), prog1);
//  writeFATEntryCustomByte("prog2", sizeof(prog2), prog2);
//  writeFATEntryCustomByte("prog3", sizeof(prog2), prog2);
}

void loop() {
  readInput();



  for (int i = 0; i < 10; i++ ) {     //voert per instructie uit voor ieder process dat runnend is
    if (processes[i].state == 'r') {

      execute(i);
    }
  }
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
      while (tempbyte != 0) {       //zolang de tijdelijke byte opslag geen 0 is
        pushByte(byteArray[processes[i].pc], i);
        n++;            //teller voor wat er op de stack gepusht moet worden dus het aantal chars van de string
        processes[i].pc++;
        tempbyte = byteArray[processes[i].pc];
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
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid); //fileMem[t].name = givenName;//fileMem[t].pid = givenPid;
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
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);//fileMem[t].name = givenName;//fileMem[t].pid = givenPid;
        fileMem[tempIndex].size = 2;    //bij string eerst groote poppen
        fileMem[tempIndex].type = INT;
        fileMem[tempIndex].address = tempLocation;
        memory[tempLocation + 1] =  tempbyte;
        memory[tempLocation] =  popByte(i);
      }


      else if (tempbyte == CHAR) {        //aanpassen
        tempbyte = popByte(i);
        processes[i].pc++;
        tempLocation = designateeMemoryfileLoc(tempbyte);
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);//fileMem[t].name = givenName;//fileMem[t].pid = givenPid;
        fileMem[tempIndex].size = 1;    //bij string eerst grote poppen
        fileMem[tempIndex].type = CHAR;
        fileMem[tempIndex].address = tempLocation;
        memory[tempLocation] =  tempbyte;
      }


      else if (tempbyte == FLOAT) {    //aanpassen
        processes[i].pc++;
        tempbyte = popByte(i);
        tempLocation = designateeMemoryfileLoc(tempbyte);
        tempIndex = designateMemoryfileNr (byteArray[processes[i].pc], processes[i].pid);//fileMem[t].name = givenName;//fileMem[t].pid = givenPid;
        fileMem[tempIndex].size = 4;    //bij string eerst groote poppen
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
      tempbyte = byteArray[processes[i].pc];          //check name van variable die geset wordt
      tempLocation = returnMemoryIndex(byteArray[processes[i].pc], processes[i].pid);
      tempIndex = returnFtIndex((char)byteArray[processes[i].pc], processes[i].pid);
      tempbyte = fileMem[tempIndex].type;
      //tempbyte bevat het type

      if (tempbyte == STRING) {    //aanpassen

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

      break;
    case PLUS:

      break;
    case MINUS:

      break;
    case TIMES:

      break;
    case DIVIDEDBY:

      break;
    case MODULUS:

      break;
    case UNARYMINUS:

      break;
    case EQUALS:

      break;
    case NOTEQUALS:

      break;
    case LESSTHAN:

      break;
    case LESSTHANOREQUALS:

      break;
    case GREATERTHAN:

      break;
    case GREATERTHANOREQUALS:

      break;
    case LOGICALAND:

      break;
    case LOGICALOR:

      break;
    case LOGICALXOR:

      break;
    case LOGICALNOT:

      break;
    case BITWISEAND:

      break;
    case BITWISEOR:

      break;
    case BITWISEXOR:

      break;
    case BITWISENOT:

      break;
    case TOCHAR:

      break;
    case TOINT:

      break;
    case TOFLOAT:

      break;
    case ROUND:

      break;
    case FLOOR:

      break;
    case CEIL:

      break;
    case MIN:

      break;
    case MAX:

      break;
    case ABS:

      break;
    case CONSTRAIN:

      break;
    case MAP:

      break;
    case POW:

      break;
    case SQ:

      break;
    case SQRT:

      break;
    case DELAY:

      break;
    case DELAYUNTIL:
      Serial.println("DELAYUNTIL");
      break;
    case MILLIS:

      break;
    case PINMODE:

      break;
    case ANALOGREAD:

      break;
    case ANALOGWRITE:

      break;
    case DIGITALREAD:

      break;
    case DIGITALWRITE:

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
      Serial.println("");
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
    case OPEN:

      break;
    case CLOSE:

      break;
    case WRITE:

      break;
    case READINT:

      break;
    case READCHAR:

      break;
    case READFLOAT:

      break;
    case READSTRING:

      break;
    case IF:

      break;
    case ELSE:

      break;
    case ENDIF:

      break;
    case WHILE:

      break;
    case ENDWHILE:

      break;
    case LOOP:
      Serial.println("LOoOP");
      break;
    case ENDLOOP:
      Serial.println("ENDLOOP");
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
      return;

      break;
    case FORK:

      break;
    case WAITUNTILDONE:

      break;
  }
  processes[i].pc++;
}


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

int returnFtIndex (byte givenName, int givenPid) {      //geeft index terug van fileTable
  int memnr = 0;
  for (int i = 0; i < 25; i++) {
    if (fileMem[i].name == givenName && fileMem[i].pid == givenPid) {   //nu weten we de index van de file table
      memnr = i;
    }
  }
  return memnr;
}



void store() {        //opslaan van een bestand
  if (message1[0] != '\0' && message2[0] != '\0') {
    if (readFATEntry(message1) == -1) {
      //Serial.println(atoi(message2));
      writeFATEntryCustom(message1, atoi(message2), message3);
    } else {
      Serial.println("file name already exists");
    }
  } else {
    Serial.println("store -name -size -data");
  }
}

void retrieve () {   //retrieved een bestand (werkt alleen niet op byte arrays dus de test programmas)                                            

  Serial.print("file ");
  Serial.print(message1);
  Serial.println(" contains:");
  for (int s = 0; s < noOfFiles; s++) {
    if (!strcmp(file[s].name, message1)) {      //wanneer de gegeven naam gelijk is aan een naam van de struct
      clearBuffers();
      EEPROM.get(file[s].startPos, message3);
      Serial.println(message3);
    }
  }
}

void erase () {       //haalt een waarde van de eeprom
  for (int y = 0; y < noOfFiles; y++) {

    if (!strcmp(file[y].name, message1)) {      //wanneer de gegeven naam gelijk is aan een naam van de struct
      Serial.println("erased");
      Serial.println(message1);
      for (int s = 0; s < 12; s++) {
        file[y].name[s] = empty[s];
      }

      file[y].startPos = 0;
      file[y].fileLength = 16;
      EEPROM.put(y * 16, file[y]);
      sync();
    }
  }
}



void files () {     //print de files die op de eeprom staan
  Serial.print("files: \t");
  Serial.println(noOfFiles);
  FAT fileX;
  for (int s = 0; s < 10; s++) {
    if (strcmp(file[s].name, "empty")) {
      Serial.print(" |\t");
      Serial.print( file[s].name);
      Serial.print("\tSize (byte): ");
      Serial.print( file[s].fileLength);
      Serial.print("\t");
      Serial.println( file[s].startPos);
    }
  }
}


void freespace () {     //print hoeveel ruimte er over is
  int freespace = 1024 - 160;
  for (int s = 9; s > 0; s--) {
    //Serial.println(s);
    if (strcmp(file[s].name, "empty")) {
      freespace = freespace - file[s].fileLength;
    }

  }
  Serial.print(freespace);
  Serial.println("(byte) available");

}

void runProgram () {
  int h;
  boolean namecorrect = false;
  char tempname[16] = "";
  for (int t = 0; t < 10; t++) {
    if (!strcmp(file[t].name, message1)) {
      for (int i = 0; i < strlen(file[t].name); i++)
        tempname[i] = file[t].name[i];
      namecorrect = true;         //wanneer de naam bestaat in de files
      h = t;
    }
  }
  for (int t = 0; t < 10; t++) {

    if (processes[t].pid == 0 && namecorrect) {     //wanneer de naam klopt en de waarde leeg is
      systemPid++;
      localPid++;                               
      for (int i = 0; i < 12; i++) {
        processes[t].name[i] = tempname[i];
      }
      processes[h].state = 'p';
      processes[h].startPos = file[h].startPos;
      processes[h].pc = 0;
      processes[h].sp = 0;
      processes[h].fp = t;
      processes[h].pid = systemPid;
      Serial.print(processes[t].name);
      Serial.println(" - Started");

      return;
    }
  }

  Serial.println("an Error occured please retry");
}


void suspendProces () {
  Serial.println("--------------");
  for (int t = 0; t < 10; t++) {
    if (message2[0] == '\0') {
      Serial.println("No pid given, type: suspend processname pid");
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'p' && processes[t].pid == atoi(message2)) {

      Serial.println("Process already paused");
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'r' && processes[t].pid == atoi(message2)) { // && processes[t].pid == message2[0]
      Serial.println("Suspended process");
      processes[t].state = 'p';
      return;
    }

  }
  Serial.println("name or pid combination not extitent");
  return;
}


void resumeProces () {
  Serial.println("--------------");
  for (int t = 0; t < 10; t++) {
    if (message2[0] == '\0') {
      Serial.println("No pid given, type: suspend processname pid");
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'r' && processes[t].pid == atoi(message2)) {

      Serial.println("Process already running");
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == 'p' && processes[t].pid == atoi(message2)) { 
      Serial.println("Resumed process");
      processes[t].state = 'r';
      return;
    }

  }
  Serial.println("name or pid combination not extitent");
  return;
}

void killProcess () {
  Serial.println("--------------");
  for (int t = 0; t < 10; t++) {
    if (message2[0] == '\0') {
      Serial.println("No pid given, type: suspend processname pid");
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state == '0' && processes[t].pid == atoi(message2)) {

      Serial.println("Process already terminated");
      return;
    } else if (!strcmp(processes[t].name, message1 ) && processes[t].state != '0' && processes[t].pid == atoi(message2)) { // && processes[t].pid == message2[0]
      Serial.println("Terminated process");
      processes[t].state = '0';
      erasePidFiles(processes[t].pid);
      return;
    }
  }
  Serial.println("name or pid combination not extitent");
  return;
}





void list () {
  Serial.print("active processes: ");
  Serial.println(localPid);
  for (int s = 0; s < 10; s++) {
    if (strcmp(processes[s].name, "")) {
      Serial.print(" |\t");
      Serial.print( processes[s].name);
      Serial.print("\t pid: ");
      Serial.print( processes[s].pid);
      Serial.print("\t state: ");
      if (processes[s].state == 'r')
        Serial.println("Running");
      if (processes[s].state == 'p')
        Serial.println("Paused");
      if (processes[s].state == '0')
        Serial.println("Terminated");
    }
  }
}




void clearEEPROM () {
  FAT fileN;
  //clears the eeprom
  Serial.println("please wait");
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
  Serial.println("Cleared EEPROM");
  noOfFiles = 0;
  customAddress = 0;
}





void help() {
  delay(20);
  Serial.println("");
  Serial.println("---- List of commands ----");
  Serial.println("store ");
  Serial.println("retrieve ");
  Serial.println("erase ");
  Serial.println("files ");
  Serial.println("freespace ");
  Serial.println("run (Program) ");
  Serial.println("list ");
  Serial.println("suspend (Process) ");
  Serial.println("resume (Process) ");
  Serial.println("kill (Process)");
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



int designateMemFile () {                                           //returns location of empty file
  for (int t = 0; t < 10; t++) {
    if (!strcmp(file[t].name, "empty")) {
      return t;
    }
  }
  return 0;
}

int designateMemLoc (int fileSize) {                                            //returns location of empty space in EEprom
  int locCounter = 160;
  for (int t = 0; t < 10; t++) {
    if (file[t].startPos == 0) { //dus wanneer de struct geen waarde bevat
      if (t == 0) {                     //kijk dan of het de eerste is dan geef 160 mee
        return 160;
      } else if (file[t + 1].startPos - file[t - 1].fileLength - file[t - 1].startPos - fileSize  >= 0 || file[t + 1].startPos <= 160) {
        return file[t - 1].startPos + file[t - 1].fileLength;
      } else {
        return 160;
      }
    }
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



void ping() {
  Serial.println("pong");
}

void pong() {
  Serial.println("ping");
}



typedef struct {
  char name [ 16 ];
  void * func ;
} commandType ;

static commandType command [] = {
  {"ping" , &ping } ,
  {"pong" , &pong } ,
  {"store" , &store } ,
  {"retrieve" , &retrieve } ,
  {"erase" , &erase } ,
  {"files" , &files } ,
  {"freespace" , &freespace } ,
  {"run" , &runProgram } ,
  {"list" , &list } ,
  {"suspend" , &suspendProces } ,
  {"resume" , &resumeProces } ,
  {"kill" , &killProcess },
  {"ce" , &clearEEPROM },
  {"help" , &help }

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





void writeFATEntryCustom(char customName[], int customSize, char customData[]) {                               //nu mee bezig
  Serial.println(customSize);
  Serial.println(customData);
  if (strlen(customData) <= customSize && strlen(customName) <= 12 && customSize <= 128 && customSize > 0) {
    Serial.print("file: ");
    Serial.print(message1);
    Serial.println(" stored");
    FAT fileC;
    int emptyfileloc = designateMemFile();         //returned welk bestand nog empty is
    int memloc = designateMemLoc(strlen(customName));            //returned locatie van eeprom waar geschreven kan worden
    for (int c = 0; c < 11 ; c++)
      file[emptyfileloc].name[c] = '\0';
    for (int c = 0; c < strlen(customName) ; c++)
      file[emptyfileloc].name[c] = customName[c];
    file[emptyfileloc].startPos = memloc;
    file[emptyfileloc].fileLength = customSize;



    EEPROM.put(emptyfileloc * 16, file[emptyfileloc]);      //header file alloceren

    for (int i = 0; i < strlen(customData); i++) {
      EEPROM.put(memloc + i, customData[i]);
      //Serial.println(customData[i]);
    }
    noOfFiles++;
    clearBuffers();
  } else {
    Serial.println("given size is smaller then given data");
    Serial.println("or given name is bigger than 12");
    Serial.println("or given size is bigger than 16");

  }
}

void writeFATEntryCustomByte(char customName[], int customSize, byte customData[]) {
  int emptyfileloc = designateMemFile();         //returned welk bestand nog empty is
  int memloc = designateMemLoc(customSize);  //?????????
  //Serial.println("323232");
  //Serial.println(memloc);
  for (int c = 0; c < strlen(customName) ; c++)
    file[emptyfileloc].name[c] = customName[c];
  file[emptyfileloc].startPos = memloc;
  file[emptyfileloc].fileLength = customSize;
  EEPROM.put(emptyfileloc * 16, file[emptyfileloc]);      //header file alloceren
  for (int i = 0; i < customSize; i++ )
    EEPROM.put(memloc + i, customData[i]);
  //put data
  //
  noOfFiles++;
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
  //return -1;
}


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
    }
  }
}





int designateeMemoryfileLoc (int fileSize) {                                            //returns location of empty space in EEprom
  int locCounter = 0;
  for (int t = 0; t < 25; t++) {
    //Serial.println(file[t + 1].startPos - memory[t - 1].fileLength - memory[t - 1].startPos - fileSize);
    if (fileMem[t].pid == '\0') { //dus wanneer de struct geen waarde bevat
      if (fileMem[t + 1].address - fileMem[t - 1].size - fileMem[t - 1].address - fileSize  >= 0 || fileMem[t + 1].address == '\0') {
        return fileMem[t - 1].address + fileMem[t - 1].size;
      }
    }
  }
  return 99;
}
