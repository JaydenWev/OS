#include <EEPROM.h>

#define STACKSIZE 32

byte stack [ STACKSIZE ];       // Stack
byte sp = 0;                    // Stack position
byte memory[256];               //werkgeheugen        tijdelijke variablen
byte noOfVars = 0;
byte startpos = 0;
int systemPid = 0;
int localPid = 0;

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

void setup(){
  Serial.begin(9600);
  Serial.println(" ");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  Serial.println("■ Starting...\t\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
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

void sync () { //syncs data from eeprom with data from struct
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
// #########################################################
// ##########              TestFunctions          ##########
// #########################################################

void help() {
  delay(20);
  Serial.println("");
  Serial.println("■■■■■ List of command ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  Serial.println("■\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■ store\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■ retrieve\t*\t\t\t\t\t\t\t\t■");
  Serial.println("■ erase\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■ files\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■ freespace\t\t\t\t\t\t\t\t\t■");
  Serial.println("■ run [Program]\t*\t\t\t\t\t\t\t\t■");
  Serial.println("■ list\t\t*\t\t\t\t\t\t\t\t■");
  Serial.println("■ suspend [Process]\t*\t\t\t\t\t\t\t■");
  Serial.println("■ resume [Process]\t*\t\t\t\t\t\t\t■");
  Serial.println("■ kill [Process]\t*\t\t\t\t\t\t\t■");
  Serial.println("■\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
}

void ping() {
  Serial.println("■ pong\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
}

void pong() {
  Serial.println("■ ping\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
}
void numberOfFiles() {
  Serial.print("■ ");
  Serial.print(noOfFiles);
  Serial.println("\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
}
// #########################################################
// ##################   InputController   ##################
// #########################################################

typedef struct {
  char name [ 16 ];
  void * func ;
} commandType ;

static commandType command [] = { // All availlable commands
  {"ping" , &ping } ,
  {"pong" , &pong } ,
  {"store" , &store } ,
  {"erase" , &erase } ,
  {"retrieve" , &retrieve } ,
  {"files" , &files } ,
  {"freespace" , &freespace } ,
//  {"run" , &runProgram } ,
//  {"list" , &list } ,
//  {"suspend" , &suspendProces } ,
//  {"resume" , &resumeProces } ,
//  {"kill" , &killProcess },
  {"ce" , &clearEEPROM },
  {"no" , &numberOfFiles },
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
  Serial.println("■ command NOT recognised, type help for commands.\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  clearBuffers();
}

void clearEEPROM () {
  FAT fileN;
  //clears the eeprom
  Serial.println("■ please wait\t\t\t\t\t\t\t\t\t■");
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
  Serial.println("■ Cleared EEPROM\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  noOfFiles = 0;
  customAddress = 0;
}
// #########################################################
// ##################      Functions      ##################
// #########################################################

void store() { //opslaan van een bestand
  if (message1[0] != '\0' && message2[0] != '\0') {
    if (readFATEntry(message1) == -1) {
      //Serial.println(atoi(message2));
      writeFATEntryCustom(message1, atoi(message2), message3);
      Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
    } else {
      Serial.println("■ file name already exists\t\t\t\t\t\t\t■");
      Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
    }
  } else {
    Serial.println("■ store [name] [size] [data]\t\t\t\t\t\t\t■");
    Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  }
}

void erase () {       //haalt een waarde van de eeprom
  if (message1[0] != '\0')
    for (int y = 0; y < noOfFiles; y++) {
      if (!strcmp(file[y].name, message1)) {      //wanneer de gegeven naam gelijk is aan een naam van de struct
        Serial.println("■ ERASED\t\t\t\t\t\t\t\t\t■");
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
  else {
    Serial.println("■ erase [fileName]\t\t\t\t\t\t\t\t■");
    Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  }
}

void retrieve () {   //retrieved een bestand (werkt alleen niet op byte arrays dus de test programmas)                                            
  if (message1[0] != '\0'){
    Serial.print("■ file: ");
    Serial.print(message1);
    Serial.println(", contains:");
    for (int s = 0; s < noOfFiles; s++) {
      if (!strcmp(file[s].name, message1)) {      //wanneer de gegeven naam gelijk is aan een naam van de struct
        clearBuffers();
        EEPROM.get(file[s].startPos, message3);
        Serial.print("■ ");
        Serial.println(message3);
        Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
      }
    }
  }
  else{
    Serial.println("■ retrieve [fileName]\t\t\t\t\t\t\t\t■");
    Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
  }
}

void files () { //print de files die op de eeprom staan
  Serial.print("■ files: \t");
  Serial.print(noOfFiles);
  Serial.println("\t\t\t\t\t\t\t\t■");
  FAT fileX;
  for (int s = 0; s < 10; s++) {
    if (strcmp(file[s].name, "empty")) {
      Serial.print("■ ");
      Serial.print( file[s].name);
      Serial.print("\tSize (byte): ");
      Serial.print( file[s].fileLength);
      Serial.print("\t");
      Serial.println( file[s].startPos);
    }
  }
  Serial.println("■\t\t\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
}

void freespace () {     //print hoeveel ruimte er over is
  int freespace = 1024 - 160;
  for (int s = 9; s > 0; s--) {
    //Serial.println(s);
    if (strcmp(file[s].name, "empty")) {
      freespace = freespace - file[s].fileLength;
    }
  }
  Serial.print("■ ");
  Serial.print(freespace);
  Serial.println("(byte) available\t\t\t\t\t\t\t\t■");
  Serial.println("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
}
// #########################################################
// ##################    HelpFunctions    ##################
// #########################################################
// 
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

void writeFATEntryCustom(char customName[], int customSize, char customData[]) {
  Serial.print("■ Size: ");
  Serial.print(customSize);
  Serial.println("\t\t\t\t\t\t\t\t\t■");
  Serial.print("■ Data: ");
  Serial.print(customData);
  Serial.println("\t\t\t\t\t\t\t\t■");
  if (strlen(customData) <= customSize && strlen(customName) <= 12 && customSize <= 128 && customSize > 0) {
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
    Serial.print("■ Name: ");
    Serial.println(message1);
    Serial.println("■ STORED\t\t\t\t\t\t\t\t\t■");
    noOfFiles++;
    clearBuffers();
  } else {
    Serial.println("■ Given size is smaller then given data\t\t\t\t\t\t■");
    Serial.println("■ Or given name is bigger than 12\t\t\t\t\t\t■");
    Serial.println("■ Or given size is bigger than 16\t\t\t\t\t\t■");
  }
}

int readFATEntry(char givenName[]) {
  FAT fileX;
  Serial.println("\"");
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
