// DFPlayer Mini with ESP32 - ArduinoYard
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define LED_PIN_LOOP 2
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
  "Aiobahn +81 feat. KOTOKO - INTERNET OVERDOSE (Official Music Video) [Theme for NEEDY GIRL OVERDOSE] ",
  "DJ OKAWARI feat. Kaori Sawada 「Everything's Alright」 ",
  "DJ OKAWARI 「Perfect Blue -Strings Ver.-」 ",
  "Eclipse ",
  "Harmonious ",
  "Koino Uta ",
  "Lost ",
  "Medieval Music - Cobblestone Village ",
  "Mili - Bulbel / ENDER LILIES: Quietus of the Knights ",
  "Mili - Skin-Deep Comedy (＂魔法使いの約束＂ Part 2 theme song) ",
  "Mili - YUBIKIRI-GENMAN -special edit- ",
  "Moonline ",
  "Natsu No Ao ",
  "RIDDLE JOKER OP 主題歌「astral ability」/橋本みゆき・佐咲紗花【Full】【歌詞あり】 ",
  "ReoNa 『ないない』-Music Video- ",
  "ReoNa 『シャル・ウィ・ダンス？』-Music Video- (TVアニメ「シャドーハウス 2nd Season」OPテーマ) ",
  "ReoNa 『生命線』-Music Video-（月姫 -A piece of blue glass moon- THEME SONG E.P.) ",
  "SawanoHiroyuki[nZk]:Jean-Ken Johnny & TAKUMA『PROVANT』x TVアニメ『Fate/strange Fake』Collaboration Movie ",
  "Seimeisen ",
  "TWRP - Atomic Karate ",
  "Tsuki No Kioku ",
  "Tsukihime: re ",
  "VITA -The Days- ",
  "Warm and Small Light ",
  "Y1K ",
  "Yosuga no Sora Opening [FULL] ",
  "[SUB] Arknights EP『Heavenly Me』vo. RINA from TOGENASHI TOGEARI ┃ Exusiai the New Covenant ",
  "unknown ",
  "「英雄の詩篇 / Eiyuu no shihen」(歌：ダズビー / DAZBEE) ",
  "【IA】Six Trillion Years and Overnight Story【VOCALOID-PV】 ",
  "【千恋＊万花】キズナヒトツ【ムラサメ】 ",
  "いのちの名前 (木村弓) arranged 2021ver.／ダズビー COVER ",
  "すーぱー☆あふぇくしょん ",
  "つきひめ! - Tsu! Ki! Hi! Me! - Tsukihime 月姫 Remake OST ",
  "とある竜の恋の歌 - 中文字幕 ",
  "むかしむかし、あるところに - Once Upon a Time - Tsukihime 月姫 Remake OST ",
  "めんたいコズミック (feat. nicamoq) ",
  "インドア系ならトラックメイカー ",
  "ダンスフロアの果実 ",
  "ダ・カーポII 〜あさきゆめみし君と〜 ",
  "ハッピーエンド (「生命線」 piano ver.) - Happy End (Lifeline piano Ver.) - Tsukihime 月姫 Remake OST ",
  "ヨスガノソラ メインテーマ-記憶- ",
  "ヨスガノソラ メインテーマ-遠い空へ- ",
  "ロボティックガール (feat. nicamoq) ",
  "今はもう遠く - Now So Far Away - Tsukihime 月姫 Remake OST ",
  "刹那の誓い ",
  "千恋＊万花 主題歌「恋ひ恋ふ縁」/ KOTOKO【Full】【日中歌詞翻譯】 ",
  "忘れじの言の葉 (安次嶺希和子)  ／ダズビー COVER ",
  "輝く君の詩 ",
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

    pinMode(LED_PIN_LOOP, OUTPUT);
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
      PauseUnpauseLoop();
  }
}

unsigned long pressStartTime = 0;
bool longPressDetected = false;
//0 no loop, 1 = loop song, 2 = loop whole playlist
int loopState = 0;
unsigned long lastPressTime = 0;

void LoopLED(){
  digitalWrite(LED_PIN_LOOP, HIGH);
  delay(500);
  digitalWrite(LED_PIN_LOOP, LOW);
}

void PauseUnpauseLoop() {
    currentStatePause = digitalRead(PIN_PAUSE);

    if (lastStatePause == HIGH && currentStatePause == LOW) {
        pressStartTime = millis();
        longPressDetected = false;
    }

    if (currentStatePause == LOW &&
        !longPressDetected &&
        millis() - pressStartTime >= 1000) {

        longPressDetected = true;

        if (loopState == 0) {
            loopState = 1;
            myDFPlayer.enableLoop();
            Serial.println("Looping current song");
        }
        else if (loopState == 1) {
            loopState = 2;
            myDFPlayer.disableLoop();
            myDFPlayer.enableLoopAll();
            Serial.println("Looping playlist");
        }
        else {
            loopState = 0;
            myDFPlayer.disableLoopAll();
            Serial.println("Looping disabled");
        }

        LoopLED();
    }

    if (lastStatePause == LOW && currentStatePause == HIGH) {
        if (!longPressDetected) {
            pause_music = !pause_music;

            if (pause_music) {
                myDFPlayer.pause();
                Serial.println("Paused Music");
                delay(200);
            }
            else {
                myDFPlayer.start();
                Serial.println("Resuming Music");
                delay(200);
            }
        }
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
        if (type == DFPlayerPlayFinished && loopState != 1) {
            currentTrack++;
            //if last track been played then stop the DFPLayer
            //and if it's looping the playlist
            if (currentTrack > totalTracks && loopState != 2) {

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