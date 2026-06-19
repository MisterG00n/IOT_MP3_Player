// DFPlayer Mini with ESP32 - ArduinoYard
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define PIN_VOL_UP 13
#define PIN_VOL_DOWN 12
#define PIN_START 14
#define PIN_NEXT_SONG 26
#define PIN_PREVIOUS_SONG 27
#define PIN_PAUSE 25

int currentTrack = 1;
const int totalTracks = 49;
int shuffledList[totalTracks];

const char* songNames[] = {
    "",
  
    };

 
int lastStateStart = HIGH; 
int lastStateNext = HIGH;
int lastStatePrev = HIGH;
int lastStatePause = HIGH;


int currentStateNext;
int currentStatePrev;
int currentStateVolUp; 
int currentStateVolDown;     
int currentStateStart;
int currentStatePause;

int defaultVolume = 5;
bool music_start = false;
bool pause_music = false;

HardwareSerial mySerial(1);  // Use UART1
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(115200);
    delay(2000);
    mySerial.begin(9600, SERIAL_8N1, 16, 17);  // RX = 16, TX = 17

    if (!myDFPlayer.begin(mySerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (1)
        ;
    }

    Serial.println("DFPlayer Mini ready!");
    myDFPlayer.volume(defaultVolume);  // Volume level (0–30)
    pinMode(PIN_VOL_UP, INPUT_PULLUP);
    pinMode(PIN_VOL_DOWN, INPUT_PULLUP);
    pinMode(PIN_START, INPUT_PULLUP);
    pinMode(PIN_NEXT_SONG, INPUT_PULLUP);
    pinMode(PIN_PREVIOUS_SONG, INPUT_PULLUP);
    pinMode(PIN_PAUSE, INPUT_PULLUP);

}

void loop() {
  currentStateStart = digitalRead(PIN_START);
  CheckIfAllSongBeenPlayed();
  VolumeControl();

  if (lastStateStart == HIGH && currentStateStart == LOW){
    music_start = !music_start;
    if (music_start) {
      currentTrack = 1;
      myDFPlayer.play(1);
      Serial.print("Playing: ");
      Serial.println(songNames[currentTrack]);
      ShuffleTracks();
    } else {
      Serial.println("STOPPED");
      myDFPlayer.stop();
    }
    delay(500);
  }
  lastStateStart = currentStateStart;
  if (music_start){
      SongControl();
      PauseUnpause();
  }
}

void PauseUnpause(){
  currentStatePause = digitalRead(PIN_PAUSE);
  if (lastStatePause == HIGH & currentStatePause == LOW){
    if (!pause_music){
      myDFPlayer.pause();
      pause_music = true;
      Serial.print("Paused Music");
    }
    else{
      myDFPlayer.start();
      pause_music = false;
      Serial.print("Resuming Music");
    }
    delay(300);
  }
  lastStatePause = currentStatePause;
}

//next and previous song
void SongControl() {
    currentStateNext = digitalRead(PIN_NEXT_SONG);
    currentStatePrev = digitalRead(PIN_PREVIOUS_SONG);

    if (lastStateNext == HIGH && currentStateNext == LOW) {
        currentTrack++;
        // IF next track is not on range then go to first song 
        if (currentTrack > totalTracks) {
            currentTrack = 1;
        }

        myDFPlayer.play(currentTrack);
        Serial.println("NEXT SONG");
        Serial.print("Playing: ");
        Serial.println(songNames[currentTrack]);
        delay(300);
    }

    if (lastStatePrev == HIGH && currentStatePrev == LOW) {
        currentTrack--;
        // IF next track is not on range then back to last song
        if (currentTrack < 1) {
            currentTrack = totalTracks;
        }

        myDFPlayer.play(currentTrack);
        Serial.println("PREVIOUS SONG");
        Serial.print("Playing: ");
        Serial.println(songNames[currentTrack]);
        delay(300);
    }

    lastStateNext = currentStateNext;
    lastStatePrev = currentStatePrev;
}
void VolumeControl(){
  currentStateVolUp = digitalRead(PIN_VOL_UP);
  currentStateVolDown = digitalRead(PIN_VOL_DOWN);

  if (currentStateVolUp == LOW)  {
    if(defaultVolume <30){
      defaultVolume++;
      myDFPlayer.volume(defaultVolume);

      Serial.print("Volume Up: ");
      Serial.println(defaultVolume);
    }
    delay(200);
  }
  if(currentStateVolDown == LOW){
    if(defaultVolume >0){
      defaultVolume--;
      myDFPlayer.volume(defaultVolume);

      Serial.print("Volume Down: ");
      Serial.println(defaultVolume);
    }
    delay(200);
  }
}
void CheckIfAllSongBeenPlayed() {
    if (myDFPlayer.available()) {
        uint8_t type = myDFPlayer.readType();
        //if song finished
        if (type == DFPlayerPlayFinished) {
            currentTrack++;
            //if last track been played then stop the DFPLayer
            if (currentTrack > totalTracks) {

                myDFPlayer.stop();
                music_start = false;

                Serial.println("All songs have been played!");
                Serial.println("STOPPED");

                return;   // prevent playing song 1 again
            }
            //go to next song
            myDFPlayer.play(currentTrack);

            Serial.print("Playing: ");
            Serial.println(songNames[currentTrack]);
        }
    }
}

int currentIndex = 0;
void ShuffleTracks() {
    for (int i = 0; i < totalTracks; i++) {
        shuffledList[i] = i + 1; // 1..totalTracks
    }

    // Fisher-Yates shuffle
    for (int i = totalTracks - 1; i > 0; i--) {
        int j = random(i + 1);
        int temp = shuffledList[i];
        shuffledList[i] = shuffledList[j];
        shuffledList[j] = temp;
    }

    currentIndex = 0;
}

int GetNextRandomTrack() {
    if (currentIndex >= totalTracks) {
        ShuffleTracks();
    }

    return shuffledList[currentIndex++];
}