
#include <TMRpcm.h>
#include "PinChangeInterrupt.h"
#include <SD.h>          //使用SD.h函數庫
#include <SPI.h>         //使用SPI函數庫
#include <Timer.h>
Timer timer_1s;
File myFile;
File logFile;

//=====================================================================================
// Define buttons variable or settings
//=====================================================================================
char R_number[50] = {"0123456789"};  //設定R端電話(無SD卡狀態下)
char P_number[50] = {"45044"};  //設定語音電話(無SD卡狀態下)
char L_number[20] = {"314"};    //設定地區號碼(無SD卡狀態下)
short debounceTime = 10; // debounce 10ms * 10 = 100ms
byte debounceCounter = 0; 
boolean debounceMode = false;
byte pressedButton = 0;

boolean trigger = false;
boolean triggerInit = true;
boolean logRecording = false;
unsigned long triggerStartTime;

const byte buttonMask = B01111111;
byte buttonMinderBuffer = buttonMask;
byte buttomTempRegister = buttonMask;
byte buttomSenseXorData = buttonMask;

const byte buttonPin1 = A8;   // button1
const byte buttonPin2 = A9;   // button2
const byte buttonPin3 = A10;  // button3
const byte buttonPin4 = A11;  // button4
const byte buttonPin5 = A12;  // button5
const byte buttonPin6 = A13;  // button6
const byte buttonPin7 = A14;  // button7

/*=====================================================================================
  SDCARD   ARDUINO     
   CS       53
   SDK      52
   MOSI     51
   MISO     50  
====================================================================================== */
TMRpcm tmrpcm;
const int  chipSelect = 53; // 常數chipSelect型態為整數，值為53

const int  telestate=A0;    // detect if there is a voltage-in
const int  teleout=22;      // Relay K1, control connect the telephone wire
const byte teleoutLed=41;   // the Led that means the telephone wire is in input/output condition

const byte fdecode=47;      // frequency detect port
const byte tone1Pin=10;     // pin for tone 1
const byte tone2Pin=11;     // pin for tone 2

const byte alarmOut=30;     // Relay K5, pin to control 12V output for the alarm
const byte alarmDuration=25;// the duration that the alarm will ring when trigger alarm system
boolean alarmState=false;
byte alarmPassedTime=0;



char sdinput;
char input;
byte hangup=0;
byte number[50];
byte number1[50];           // R端電話1
byte number2[50];           // R端電話2
byte number3[50];           // R端電話3
byte number4[50];           // R端電話4
byte number5[50];           // R端電話5
byte number6[50];           // R端電話6
byte number7[50];           // R端電話7

byte police[50];          
byte police1[50];           // 警察局電話
byte police2[50];           // 警察局電話
byte police3[50];           // 警察局電話
byte police4[50];           // 警察局電話
byte police5[50];           // 警察局電話
byte police6[50];           // 警察局電話
byte police7[50];           // 警察局電話

byte Dnum[5];               // debounce time
byte readDindex=0;
int temp123=0;

int  position1[20];         // 區域訊息(4碼 digital-->hexa)
byte positionmess[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};  //區域加迴路(位置訊息13碼)
byte audioloop=0;
byte sdataread=10;
byte rnumber=0;
byte mnumber=0;
byte countfind=0;
byte index=0;
byte index1=0;
byte index2=0;
byte index3=0;
byte index4=0;
byte index5=0;
byte index6=0;
byte index7=0;

byte indexp=0;
byte indexp1=0;
byte indexp2=0;
byte indexp3=0;
byte indexp4=0;
byte indexp5=0;
byte indexp6=0;
byte indexp7=0;

byte loop1L=0;
byte loop2L=0;
byte loop3L=0;
byte loop4L=0;
byte loop5L=0;
byte loop6L=0;
byte loop7L=0;

byte loop1M=0;
byte loop2M=0;
byte loop3M=0;
byte loop4M=0;
byte loop5M=0;
byte loop6M=0;
byte loop7M=0;

byte redstatus=1;
byte greenstatus=1;
unsigned long p=0;
byte button=0;
byte dialpos=0;
byte audioin;

int  mid=0;
int  d;
int  power=0;

byte p1=0;
byte p2=0;
byte p3=0;
unsigned long presstime;
unsigned long presenttime;
unsigned long count;
unsigned long counttime;

int inputFrequency=0;             // the tone frequency that is from R port 
boolean fdecodeRead=0;            // to save the previous read state
boolean getInputFrequency=false;  // to enable get input frequency function
boolean enableCntInputFrequency=false;

int DTMF [ 17 ] [ 2 ] = {
  {941,1336}, // frequencies for touch tone 0 /0
  {697,1209}, // frequencies for touch tone 1 /1
  {697,1336}, // frequencies for touch tone 2 /2
  {697,1477}, // frequencies for touch tone 3 /3
  {770,1209}, // frequencies for touch tone 4 /4
  {770,1336}, // frequencies for touch tone 5 /5
  {770,1477}, // frequencies for touch tone 6 /6
  {852,1209}, // frequencies for touch tone 7 /7
  {852,1336}, // frequencies for touch tone 8 /8
  {852,1477}, // frequencies for touch tone 9 /9
  {941,1209}, // frequencies for touch tone * /10
  {941,1477}, // frequencies for touch tone # /11
  {697,1633}, // frequencies for touch tone A /12
  {770,1633}, // frequencies for touch tone B /13
  {852,1633}, // frequencies for touch tone C /14
  {941,1633}, // frequencies for touch tone D /15
  {0,0}       // delay 2 seconds              /16 
} ;
byte temp = 0;
//=========================================================設定系統初始狀態及參數====================================================
void setup() {
  timer_1s.every(1000, analyzeFrequency); // 每經過1000毫秒，就會呼叫analyzeFrequency
  tmrpcm.speakerPin = 11;

  setupTimer4Interrupt(); // initialize timer4
 
  pinMode(53, OUTPUT);
  pinMode(11,OUTPUT); // Output for audio message
  pinMode(tone1Pin,OUTPUT); // Output for Tone 1
  pinMode(tone2Pin,OUTPUT); // Output for Tone 2
  pinMode(buttonPin1,INPUT);  // Button1
  pinMode(buttonPin2,INPUT);  // Button2
  pinMode(buttonPin3,INPUT);  // Button3
  pinMode(buttonPin4,INPUT);  // Button4
  pinMode(buttonPin5,INPUT);  // Button5
  pinMode(buttonPin6,INPUT);  // Button6
  pinMode(buttonPin7,INPUT);  // Button7
  pinMode(fdecode, INPUT);
  pinMode(telestate,INPUT);          //pinA0讀電話48V在不在
  pinMode(alarmOut,OUTPUT);          //pin30 OUTPUT 12V voltage for the alarm
  digitalWrite(alarmOut,LOW);
  pinMode(teleout,OUTPUT);           //pin22 接平時的一般電話
  digitalWrite(teleout,LOW);
  pinMode(teleoutLed,OUTPUT);        //pin41 觸發按鈕
  digitalWrite(teleoutLed,LOW);
  pinMode(48, OUTPUT);
  digitalWrite(48,HIGH);  //provide SDcard module 3.3V
  pinMode(43,OUTPUT);
  pinMode(45,OUTPUT);
    
  Serial.begin(115200);   // 開啟通訊串列埠開啟
  while (!Serial) {  ;  // 等待串列埠連線
  } 
 
  Serial.print( "\nWaiting for SD card ready...");
  if (!SD.begin(53)) {
    Serial.println("Fail!");
    //=======================================================將預設(無SD卡狀態下)設定R端電話及語音電話start=====================================
    byte locationindex = 0;
    Serial.println("無法讀到SD卡，使用預設的資料");
    audioloop = 7;
    index = strlen(R_number);//設定R端電話有幾個數字
    index1 = index;
    indexp = strlen(P_number);//設定語音電話有幾個數字
    indexp1 = indexp;
    rnumber = 1;              //設定R端有幾組電話
    mnumber = 1;              //設定語音有幾組電話
    locationindex = strlen(L_number);//設定"地區"中有幾個數字，ex.314就是3個數字
    for(int i=0;i<index;i++)
    {
      number[i] = (int)R_number[i]-48;
      number1[i] = number[i];
    } 
    for(int i=0;i<indexp;i++)
    {
      police[i] = (int)P_number[i]-48;
      police1[i] = police[i];
    }
    Serial.print("audio loop=");
    Serial.println(audioloop);
    area(locationindex);
    Serial.print("number1=");
    for(int i=0;i<13;i++){
      Serial.print(number1[i]);
    }
    Serial.println("");
    Serial.print("index1=");
    Serial.println(index1);
    Serial.print("police1=");
    for(int i=0;i<13;i++){
      Serial.print(police1[i]);
    }
    Serial.println("");
    Serial.print("rnumber=");
    Serial.println(rnumber);
    Serial.print("mnumber=");
    Serial.println(mnumber);
    Serial.print("Button Debounce time=");
    Serial.print(debounceTime*10);
    Serial.println(" ms"); 
    //=======================================================將預設(無SD卡狀態下)設定R端電話及語音電話end=====================================
    //return;
  }
  if (SD.begin(53)) 
  {  
    Serial.println("Success!");
    myFile = SD.open("pei.txt");  //打开pei.txt文件
 //=========================================================讀取SD card內容start====================================================
    if (myFile) {
      while (myFile.available()) {                      //檢查是否 myaFile是否有數據
        sdinput=myFile.read();                          //如果有數據則把數據發送到串口
        if(sdataread==10){                              // --------------- --------------------------------------
          if(sdinput!='#'){       
           audioloop=sdinput-48;                        //sdinput is ASCII code
          }
          else{                                         //讀 audio loop
            sdataread=20; 
            Serial.print("audio loop=");
            Serial.println(audioloop);
          }
        }                                              // -----------------------------------------------------
        else if(sdataread==20){
         readarea(sdinput);
        }
        else if(sdataread==30){                        //往下一行找有沒有R或M，如果'#'後三個位元沒找到就關檔案
         //Serial.println("judging R or M");       
         countfind++;
          if(sdinput=='R'){                            //如果找到R 就進到讀R端電話的function
         // Serial.println("find R");
          number[20]=0;
          index=0;
          sdataread=40;
          }
          else if(sdinput=='M'){
          //Serial.println("find M");
          police[20]=0;
          indexp=0;
          sdataread=50;
          }
          else if(sdinput=='D'){
          //debounceTime = 10;
          sdataread=60; 
          indexp=0; 
          }
        }                                                //  ------------------------------------------------------------
        else if(sdataread==40){
          readr(sdinput);                           //讀R端電話的function 程式在下面
          if(sdinput=='#'){
            countfind=0;       //用來數有幾個R端電話和幾個語音電話
          }
        }                                                   // -----------------------------------------------------                                                 
        else if(sdataread==50){
          readmobile(sdinput);                      //讀語音電話的function 程式在下面
          if(sdinput=='#'){ 
            countfind=0;       //用來數有幾個R端電話和幾個語音電話
          }
        }
        else if(sdataread==60){
          readDnum(sdinput);
          if(sdinput=='#'){
            countfind=0;
          }
        }
        if(countfind>3){
          myFile.close();  //關閉 myFile
        }
      } 
    }   
    else {
      Serial.println("error opening card 1.txt");  //如果文件无法打开串口发送信息error opening datalog.txt
    }
    Serial.println("");
    Serial.print("number1=");
    for(int i=0;i<13;i++){
      Serial.print(number1[i]);
    }
    Serial.println("");
    Serial.print("index1=");
    Serial.println(index1);
 /* Serial.print("number2=");
  for(int i=0;i<13;i++){
      Serial.print(number2[i]);
  }
  Serial.println("");
  Serial.print("index2=");
  Serial.println(index2);
  Serial.print("number3=");
  for(int i=0;i<13;i++){
      Serial.print(number3[i]);
  }
  Serial.println("");
  Serial.print("index3=");
  Serial.println(index3);
  */
  Serial.print("police1=");
  for(int i=0;i<13;i++){
      Serial.print(police1[i]);
  }
  Serial.println("");
  /*Serial.print("police2=");
  for(int i=0;i<13;i++){
      Serial.print(police2[i]);
  }
  Serial.println("");
  Serial.print("police3=");
  for(int i=0;i<13;i++){
      Serial.print(police3[i]);
  }
  Serial.println("");*/
  Serial.print("rnumber=");
  Serial.println(rnumber);
  Serial.print("mnumber=");
  Serial.println(mnumber);
  Serial.print("Button Debounce time=");
  Serial.print(debounceTime*10);
  Serial.println(" ms");
  logFile = SD.open("logFile.txt", FILE_WRITE);
  writeLog("initial");
  logFile.close();
  }
  attachPCINT(digitalPinToPCINT(buttonPin1), buttonChange, RISING);
  attachPCINT(digitalPinToPCINT(buttonPin2), buttonChange, RISING);
  attachPCINT(digitalPinToPCINT(buttonPin3), buttonChange, RISING);
  attachPCINT(digitalPinToPCINT(buttonPin4), buttonChange, RISING);
  attachPCINT(digitalPinToPCINT(buttonPin5), buttonChange, RISING);
  attachPCINT(digitalPinToPCINT(buttonPin6), buttonChange, RISING);
  attachPCINT(digitalPinToPCINT(buttonPin7), buttonChange, RISING);
  Serial.println("button setting finish");
  
  
}
 //==========================================================讀取SD card內容 end=====================================================
void loop() {
 //=========================================================偵測哪個按鈕被按start====================================================
  if(pressedButton!=0){
      /*
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin1)); //暫時取消pin interrput避免多次觸發當機
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin2));
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin3));
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin4));
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin5));
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin6));
      detachPinChangeInterrupt(digitalPinToPCINT(buttonPin7));
      */
      digitalWrite(45,LOW);
      mid=0;
      delay(100);
      
      hangup=0;
      presstime=millis();       //儲存按按鈕時間
      counttime=presstime+2000; //按完過2秒
      presenttime=millis();
  
      alarmState=true;
      alarmPassedTime=0;
      digitalWrite(alarmOut,HIGH);
      
      while(presenttime<=counttime){
          presenttime=millis();
      }   
      button=2;
  }
  else{
      digitalWrite(43,LOW);   //turn the red LED off 
      digitalWrite(45,HIGH);  //turn the green LED on
  }
 //=========================================================偵測哪個按鈕被按end====================================================
 //========================================================撥打緊急求救電話start===================================================
  if(button==2){
   digitalWrite(teleout,HIGH);
   digitalWrite(teleoutLed,HIGH);
   delay(500);
   for(d=1;d<=rnumber;d++){
     assignr(d);
     Serial.println("call the r");
     for(int hangup=1;hangup<=3;hangup++){
        digitalWrite(teleout,HIGH);
        digitalWrite(teleoutLed,HIGH);
        delay(500);                      
        dialNumber(number,index);               ////////////////////////////////////////////////////////////
        Serial.print("call r for the ");
        Serial.print(hangup);
        Serial.println(" time");
        dialpos=0;
        mid=0;
        Serial.println("dialed number");

 //========================================================撥打緊急求救電話 end ===================================================
 //=====================================================偵測是否有聲音訊號進入start================================================
        count=millis()+10000;                       //count=current time + 10s，播完電話用10s來判斷是否在10秒內有收到R端的接通聲
        while(dialpos==0){
          Serial.println("Detect voice enter...");
          presenttime=millis();
          getInputFrequency=true;
          enableCntInputFrequency=false;
          inputFrequency=0;
          
          while(getInputFrequency==true){
            timer_1s.update();
            cntInputFrequency();
            redstatus=digitalRead(fdecode);         // red LED
            digitalWrite(43,redstatus);            
          }
          presenttime=millis();
          
          if((presenttime<count)&&(inputFrequency>=500)){         //有聲音訊號 則撥打位置訊息
               dialpos=1;
               delay(4500);
           }
          
          if(presenttime>count||presenttime==count){             //沒有聲音訊號則掛掉電話重播
               delay(500);
               digitalWrite(teleout,LOW);                        //掛掉電話
               digitalWrite(teleoutLed,LOW);
               delay(1200);           
               dialpos=2;
               Serial.println("hang out and recall again ");
               button=2;
           }
        }
 //=====================================================偵測是否有聲音訊號進入end================================================
 //=========================================================撥打位置訊息start====================================================
     if(dialpos==1){
            digitalWrite(43,HIGH);                               //turn the red LED on
            delay(50);    
            positionmess[0]=10;                           //10是*
            positionmess[1]=position1[1];                 //低位元 (為區域碼的低位元, 0x0~0xF)
            positionmess[2]=3;                            //固定碼3
            positionmess[3]=position1[3];                 //中位元 (為區域碼的高位元, 0x0~0xF)
            positionmess[4]=3;                            //固定碼3
            positionmess[5]=loop4L+loop5L+loop6L+loop7L;
            positionmess[6]=loop1L+loop2L+loop3L;
            positionmess[7]=loop4L+loop5L+loop6L+loop7L;
            positionmess[8]=loop1L+loop2L+loop3L;
            if(position1[9]!=0){
              positionmess[9]=position1[9];                 //高位元 (為區域碼的擴充位元，原先只能到0xFF，擴充後可到0xFFF)
              positionmess[10]=3;                           //固定碼3 
            }     
            dialNumber(positionmess,13);                  /////////////////////////////////////////////////////////////////////
            hangup=4;         //播完位置訊息後跳出迴圈 才不會再繼續撥電話
            digitalWrite(teleout,LOW);       //掛掉電話
            digitalWrite(teleoutLed,LOW);
            delay(1200);
       }
     }
   }
   resetlocation();              //set loop1L~loop7L to be 0;
   pressedButton = 0;
   button=3;
  }  
 //=========================================================撥打位置訊息end====================================================
 //=========================================================發送語音檔start====================================================
  if(button==3){
    digitalWrite(43,HIGH);   //turn the red LED on
    for(d=1;d<=mnumber;d++){
      assignpolice(d);    //根據第幾通電話修改police的值
      if(loop1M!=0||loop2M!=0||loop3M!=0||loop4M!=0||loop5M!=0||loop6M!=0||loop7M!=0){
        delay(500);
        digitalWrite(teleout,HIGH);
        digitalWrite(teleoutLed,HIGH);
        delay(1000);
        dialNumber(police,indexp);
        Serial.println("calling police");
        delay(1000);
        delay(4500);
        tmrpcm.setVolume(6);        // 0 to 7. Set volume level
        delay(500);
        if(audioloop==7){           //play 7 kinds of different audio message        
          for(mid=0;mid<2;mid++){
            if(loop1M==4){
              delay(50); 
              Serial.println("play a wav");
              tmrpcm.play("a.wav");  //the audio plays from the first second of the track
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
                if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                }
              }
              Serial.println("end playing");
              temp = 0;
              delay(50); 
            }
            if(loop2M==2){
              delay(50);
              Serial.println("play b wav");
              tmrpcm.play("b.wav");
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
               if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                } 
              }
              Serial.println("end playing");
              temp = 0;
              delay(50);
            }
            if(loop3M==1){
              delay(50);
              Serial.println("play c wav");
              tmrpcm.play("c.wav");
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
               if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                } 
              }
              Serial.println("end playing");
              temp = 0;
              delay(50); 
            }
            if(loop4M==8){
              delay(50);
              Serial.println("play d wav");
              tmrpcm.play("d.wav");
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
               if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                } 
              }
              Serial.println("end playing");
              temp = 0;
              delay(50);
            }
            if(loop5M==4){
              delay(50);
              Serial.println("play e wav");
              tmrpcm.play("e.wav");
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
               if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                } 
              }
              Serial.println("end playing");
              temp = 0;
              delay(50);                            
            }
            if(loop6M==2){
              delay(50);
              Serial.println("play f wav");
              tmrpcm.play("f.wav");
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
               if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                }
              }
              Serial.println("end playing");
              temp = 0;
              delay(50); 
            }
            if(loop7M==1){
              delay(50);
              Serial.println("play g wav");
              tmrpcm.play("g.wav");
              Serial.println("end wav");
              delay(100);
              while(tmrpcm.isPlaying()==1){
               if(temp == 0)
                {
                  Serial.println("in isplaying");
                  temp = 1;
                } 
              }
              Serial.println("end playing");
              temp = 0;
              delay(50);  
            }
          }
          tmrpcm.disable();
          delay(500);
        }
        else if(audioloop==1){      //play only 1 audio message
          for(mid=0;mid<2;mid++){
            Serial.println("play h wav");
            tmrpcm.play("h.wav");
            Serial.println("end wav");
            delay(100);
            while(tmrpcm.isPlaying()==1){}
              delay(50);
              Serial.println("music h played");
          } 
        }
        if(d==mnumber){   
          resetaudio();
        }
        Serial.println("hang out phone");
      }
      digitalWrite(teleout,LOW);
      digitalWrite(teleoutLed,LOW);
    }
    digitalWrite(teleout,LOW);
    digitalWrite(teleoutLed,LOW);
    button=0;
    
//    attachPCINT(digitalPinToPCINT(buttonPin1), buttonChange, RISING); // 重啟pin interrupt
//    attachPCINT(digitalPinToPCINT(buttonPin2), buttonChange, RISING);
//    attachPCINT(digitalPinToPCINT(buttonPin3), buttonChange, RISING);
//    attachPCINT(digitalPinToPCINT(buttonPin4), buttonChange, RISING);
//    attachPCINT(digitalPinToPCINT(buttonPin5), buttonChange, RISING);
//    attachPCINT(digitalPinToPCINT(buttonPin6), buttonChange, RISING);
//    attachPCINT(digitalPinToPCINT(buttonPin7), buttonChange, RISING);
    
  }
 //=========================================================發送語音檔end====================================================
  timer_1s.update();
}
void readarea(char sdinput){                                  //讀位置訊息
     if(sdinput!='#'){
         position1[index1]=sdinput-48;   
         if(position1[index1]==-35||position1[index1]==-38){  //抓到空白 
          return;
         } 
         else{                                                      
             index1++; 
         }
      }
      else{
         sdataread=30;
         area(index1);                                        //把位置訊息轉成16進位
      }
}

void area(byte s){    //轉16進位
    if(s!=0){
      p1=s;
      p2=s-1;
      p3=s-2;
      while(p1>1){
        position1[0]=position1[0]*10;
        p1--;
      }
      while(p2>1){
        position1[1]=position1[1]*10;
        p2--;
      }
      while(p3>1){
        position1[2]=position1[2]*10;
        p3--;
      }
      int k=0;
      int loc=0;
      for(int i=0;i<20;i++){
        k=k+position1[i];
      }
      Serial.print("location=");
      Serial.println(k);
 
      position1[1]=k%16;        //低位元 , ex: 1023=3FF(hex) --> 低位元=15=F(hex),中位元=15=F(hex),高位元=3=3(hex)
      k=k/16;
      position1[3]=k%16;        //中位元
      k=k/16;
      position1[9]=k%16;        //高位元
    }
  }
  
void readr(char sdinput){                   //讀R端電話的function
           if(sdinput!='#'){
            number[index]=sdinput-48;
              if(number[index]==-3){        //輸入'-'則delay 1秒, add '-' to transfer from switchboard to extension
                number[index]=16;
                Serial.print('-');
                index++;
              }
              else if(number[index]==221||number[index]==218){
                 Serial.print(""); 
              }
              else{                                   //讀駐警隊R端電話
                 index++; 
               }       
           } 
           else{
              sdataread=30;                    //如果讀到#就回去重新判斷是R端還是手機電話
              rnumber++;                       //往下一個變數儲存號碼
              if(rnumber==1){
                for(int n=0;n<index;n++){
                   number1[n]=number[n];                
                }
                index1=index;
                }
             else if(rnumber==2){
                for(int n=0;n<index;n++){
                   number2[n]=number[n];
                }
                index2=index;
                }
              else if (rnumber==3){
                for(int n=0;n<index;n++){
                   number3[n]=number[n];
                }
                index3=index;
                }
              else if (rnumber==4){
                for(int n=0;n<index;n++){
                   number4[n]=number[n];
                }
                index4=index;
                }
              else if (rnumber==5){
                for(int n=0;n<index;n++){
                   number5[n]=number[n];
                }
                index5=index;
                }
              else if (rnumber==6){
                for(int n=0;n<index;n++){
                   number6[n]=number[n];
                }
                index6=index;
                }
              else if (rnumber==7){
                for(int n=0;n<index;n++){
                   number7[n]=number[n];
                }
                index7=index; 
                }
              else{}
           }
  }
  void readmobile(char sdinput){                //讀語音電話的function
            if(sdinput!='#'){
            police[indexp]=sdinput-48;
              if(police[indexp]==-3){        //輸入'-'則delay 1秒
                police[indexp]=16;
                Serial.print('-');
                indexp++;
              }
              else if(police[indexp]==221||police[indexp]==218){
                 Serial.print(""); 
              }
              else{                                   //讀駐警隊R端電話
                 indexp++; 
               }       
           } 
           else{
              sdataread=30;                    //如果讀到#就回去重新判斷是R端還是手機電話
              mnumber++;                       //往下一個變數儲存號碼
              //Serial.print("mnumber=");
              //Serial.println(mnumber);
              if(mnumber==1){
                for(int n=0;n<indexp;n++){
                   police1[n]=police[n];                
                }
                indexp1=indexp;
                }
             else if(mnumber==2){
                for(int n=0;n<indexp;n++){
                   police2[n]=police[n];
                }
                indexp2=indexp;
                }
              else if (mnumber==3){
                for(int n=0;n<indexp;n++){
                   police3[n]=police[n];
                }
                indexp3=indexp;
                }
              else if (mnumber==4){
                for(int n=0;n<indexp;n++){
                   police4[n]=police[n];
                }
                indexp4=indexp;
                }
              else if (mnumber==5){
                for(int n=0;n<indexp;n++){
                   police5[n]=police[n];
                }
                indexp5=indexp;
                }
              else if (mnumber==6){
                for(int n=0;n<indexp;n++){
                   police6[n]=police[n];
                }
                indexp6=indexp;
                }
              else if (mnumber==7){
                for(int n=0;n<indexp;n++){
                   police7[n]=police[n];
                }
                indexp7=indexp;
                }
              else{}
           }
}
void readDnum(char sdinput){
       //int temp123=0;
       //char test1[] = "0010";
       if(sdinput!='#'){
        Dnum[readDindex] = sdinput-48;
        readDindex++;
       }
       else{
        sdataread=30; 
        for(int z=0;z<readDindex;z++){
          if(readDindex == 4)
          {
            if(z == 0)
              temp123 = temp123 + Dnum[z] * 1000;
            else if(z == 1)
              temp123 = temp123 + Dnum[z] * 100;
            else if(z == 2)
              temp123 = temp123 + Dnum[z] * 10;
            else if(z == 3)
              temp123 = temp123 + Dnum[z] * 1;
            else{}
          }
          else if(readDindex == 3)
          {
            if(z == 0)
              temp123 = temp123 + Dnum[z] * 100;
            else if(z == 1)
              temp123 = temp123 + Dnum[z] * 10;
            else if(z == 2)
              temp123 = temp123 + Dnum[z] * 1;
            else{}
          }
          else if(readDindex == 2)
          {
            if(z == 0)
              temp123 = temp123 + Dnum[z] * 10;
            else if(z == 1)
              temp123 = temp123 + Dnum[z] * 1;
            else{}
          }
          else if(readDindex == 1)
          {           
            temp123 = temp123 + Dnum[z] * 1;
          }
          else{}
        }
        debounceTime = temp123; //將從SD卡讀到的debounce數值覆蓋過初始宣告的10
       }

  
}
void assignr(int d){
       if(d==1){
            for(int n=0;n<index1;n++){
                number[n]=number1[n];                
             }
             index=index1;
        }
       else if(d==2){
             for(int n=0;n<index2;n++){
                 number[n]=number2[n];                
             }
             index=index2;
        }
       else if(d==3){
             for(int n=0;n<index3;n++){
                 number[n]=number3[n];                
             }
             index=index3;
        }
      else if(d==4){
             for(int n=0;n<index4;n++){
                 number[n]=number4[n];                
             }
             index=index4;
      }
      else if(d==5){
             for(int n=0;n<index5;n++){
                 number[n]=number5[n];                
             }
             index=index5;
        }
      else if(d==6){
           for(int n=0;n<index6;n++){
               number[n]=number6[n];                
           }
           index=index6;
      }
      else if(d==7){
           for(int n=0;n<index7;n++){
               number[n]=number7[n];                
           }
           index=index7;
      }
 }
void assignpolice(int d){
               if(d==1){
                for(int n=0;n<indexp1;n++){
                   police[n]=police1[n];                
                }
                indexp=indexp1;
                }
               else if(d==2){
                for(int n=0;n<indexp2;n++){
                   police[n]=police2[n];                
                }
                indexp=indexp2;
                }
               else if(d==3){
                for(int n=0;n<indexp3;n++){
                   police[n]=police3[n];                
                }
                indexp=indexp3;
                }
                else if(d==4){
                for(int n=0;n<indexp4;n++){
                   police[n]=police4[n];                
                }
                indexp=indexp4;
                }
                else if(d==5){
                for(int n=0;n<indexp5;n++){
                   police[n]=police5[n];                
                }
                indexp=indexp5;
                }
                else if(d==6){
                for(int n=0;n<indexp6;n++){
                   police[n]=police6[n];                
                }
                indexp=indexp6;
                }
                else if(d==7){
                for(int n=0;n<indexp7;n++){
                   police[n]=police7[n];                
                }
                indexp=indexp7;
                }
  
  }
void resetlocation(){
     loop1L=0;               //location message
     loop2L=0;
     loop3L=0;
     loop4L=0;
     loop5L=0;
     loop6L=0;
     loop7L=0;
}
void resetaudio(){
     loop1M=0;                //audio message
     loop2M=0;
     loop3M=0;
     loop4M=0;
     loop5M=0;
     loop6M=0;
     loop7M=0;
}

void playDTMF(byte digit, int duration){
  boolean tone1state=false;
  boolean tone2state=false;
  int tone1delay=(500000/DTMF[digit][0])-10-3; // calculate delay (in microseconds) for tone 1 (half of the period of one cycle). 10 is a fudge factor to raise the frequency due to sluggish timing.
  int tone2delay=(500000/DTMF[digit][1])-10-3; // calculate delay (in microseconds) for tone 2 (half of the period of one cycle). 10 is a fudge factor to raise the frequency due to sluggish timing.
  unsigned long tone1timer=micros();
  unsigned long tone2timer=micros();
  unsigned long timer=millis(); // for timing duration of a single tone
  if(digit==253){
    delay(1000); // one second delay if digit is 16 ('-')
  } 
  else {
    while(millis()-timer<duration){
      if(micros()-tone1timer>tone1delay){
        tone1timer=micros(); // reset the timer
        tone1state=!tone1state; // toggle tone1state
        digitalWrite(tone1Pin, tone1state);
      }
      if(micros()-tone2timer>tone2delay){
        tone2timer=micros(); // reset the timer
        tone2state=!tone2state; // toggle tone2state
        digitalWrite(tone2Pin, tone2state);
      }
    }
    digitalWrite(tone1Pin,LOW);
    digitalWrite(tone2Pin,LOW);
  }
}
 
void dialNumber(byte number[],byte len){
  for(int i=0;i<len;i++){
    playDTMF(number[i], 300);  // 300 msec duration of tone
    delay(100); // 100 msec pause between tones
  }
}
//=========================================================分析頻率====================================================
void cntInputFrequency(){
  if(enableCntInputFrequency==true){
    if(fdecodeRead!=digitalRead(fdecode)){
      inputFrequency = inputFrequency + 1;
      fdecodeRead=digitalRead(fdecode);
    }
  }
}
void analyzeFrequency(){
  if(getInputFrequency==true){
    if(enableCntInputFrequency==false){
      inputFrequency=0;                 // initial input frequency counter
      enableCntInputFrequency=true;     // enable to get input frequency for 1s
      Serial.println("Start count frequency...");
    }
    else{
      enableCntInputFrequency=false;    // after 1s, disable to get input frequency to analyze the input frequency
      getInputFrequency=false;
      Serial.print("Stop count frequency... ");
      Serial.print("Get input frequency ");Serial.print(inputFrequency);Serial.println("Hz");
      delay(10);
    }
  }

  if(alarmState==true){
    alarmPassedTime=alarmPassedTime+1;
    if(alarmPassedTime>=alarmDuration){
      alarmState=false;
      alarmPassedTime=0;
      digitalWrite(alarmOut,LOW);   
    }
  }
}

//======================================================================================
// Function : Buttons debounce
// 
// Input : Buttons change PIN
// 
// Output : Debounced button
//======================================================================================
void buttonsScan(){
  if (debounceMode==true){
    debounceCounter = debounceCounter + 1;
    
    if (debounceCounter <= debounceTime){
      buttomTempRegister = buttonLineScan();
 
      if (buttomTempRegister){
        if (buttomTempRegister ^ buttomSenseXorData){
          nonAnyButtonChange();
        }
        else{
          return;
        }
      }
      else{
        nonAnyButtonChange();
      }
    }
    else{
      // debounce time out, check if button change
      buttomTempRegister = buttonLineScan();
      if (buttomTempRegister){
        buttonChange();
      }
    }
  }    
  else{
    debounceCounter = 0;
  }
}

byte buttonLineScan(){
  // PINK == A8 ~ A15
  return (PINK & buttonMask) ^ buttonMinderBuffer;
}

void buttonChange(){
  if (debounceMode==false){
    buttomTempRegister = buttonLineScan();
    if (buttomTempRegister){
      buttomSenseXorData = buttomTempRegister;
      debounceCounter = 0;
      debounceMode = true;
    }
  }
  else if (debounceMode==true){
    if (debounceCounter >= debounceTime){
      debouncing();
      setPressedButton();
      trigger = true;
    }
  }
}

void nonAnyButtonChange(){
  Serial.println("nonAnyButtonChange");
  debounceCounter = 0;
  debounceMode = false;
}

void debouncing(){
  if (buttomTempRegister ^ buttomSenseXorData){
    nonAnyButtonChange();
  }
  else{
    // srart check from button1 until button7
    byte checkPin = 0x01;
    for(byte n=1; n<=7; n=n+1){
      if (buttomTempRegister & checkPin){
        // button number "n" pressed
        buttonMinderBuffer = buttonMinderBuffer ^ checkPin;
      }
      checkPin = checkPin << 1;
    }
    debounceCounter = 0;
    debounceMode = false;
  }
}
//======================================================================================
void setPressedButton(){
  Serial.println("================================================");
  byte checkPin = 0x01;
  for(byte n=1; n<=7; n=n+1){
    // check if button is pressed or release, only pressed has action
    if (!(buttonMinderBuffer & checkPin)){
      pressedButton = n;
      d=8;  //index for the number of phone calls
      button=1;

      Serial.print("Pressed Button: ");
      Serial.println(pressedButton);
      
      if (n==1){
        loop1L=4;
        loop1M=4;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin1)); //暫時取消pin interrput避免多次觸發當機
      }
      else if (n==2){
        loop2L=2;
        loop2M=2;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin2)); //暫時取消pin interrput避免多次觸發當機
      }
      else if (n==3){
        loop3L=1;
        loop3M=1;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin3)); //暫時取消pin interrput避免多次觸發當機
      }
      else if (n==4){
        loop4L=8;
        loop4M=8;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin4)); //暫時取消pin interrput避免多次觸發當機
      }
      else if (n==5){
        loop5L=4;
        loop5M=4;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin5)); //暫時取消pin interrput避免多次觸發當機
      }
      else if (n==6){       
        loop6L=2;
        loop6M=2;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin6)); //暫時取消pin interrput避免多次觸發當機
      }
      else if (n==7){
        loop7L=1;
        loop7M=1;
        detachPinChangeInterrupt(digitalPinToPCINT(buttonPin7)); //暫時取消pin interrput避免多次觸發當機
      }
    }
    checkPin = checkPin << 1;
  }
}

/*======================================================================================
 * Arduino MEGA 2560 Timer/Counters:
 * OCR0A, OCR1A, OCR2A are compare match registers
 * 8-bit Timer/Counters: Timer0 和 Timer2, OCR0A for Timer0, OCR2A for Timer2 must < 256
 * 16-bit Timer/Counter: Timer1, Timer4 OCR1A for Timer1 must < 65536
 * Clock Speed:16 MHz
 * timer speed (Hz) = (Arduino clock speed (16MHz)) / prescaler
======================================================================================*/
void setupTimer4Interrupt(){
  cli();                                // stop interrupts

  // set timer0 interrupt at 2kHz
  TCCR4A = 0;                           // set entire TCCR4A register to 0
  TCCR4B = 0;                           // same for TCCR4A
  TCNT4 = 0;                            // initialize counter value to 0

  // set compare match register for 100Hz increments (~=10ms)
  OCR4A = 156;                          // compare match register = 16MHz/prescaler/set frequency = 16M/64/100Hz
  TCCR4B |= (1 << WGM12);               // turn on CTC mode
  TCCR4B |= (1 << CS12) | (1 << CS10);  // Set CS12 and CS10 bits for 1024 prescaler
  TIMSK4 |= (1 << OCIE4A);              // enable timer compare interrupt
  
  sei();                                // allow interrupts
}

ISR(TIMER4_COMPA_vect){//timer4 interrupt 100Hz = 10ms
  buttonsScan();
  voltageRead();
}

void voltageRead(void){
  if (debounceMode){
    logRecording = true;
    logFile = SD.open("logFile.txt", FILE_WRITE);
    writeVoltageLog();
  }
  else if (trigger){
    if (triggerInit){
      writeLog("Trigger");
      triggerStartTime = millis();
      triggerInit = false;
    }
    else if (millis()-triggerStartTime < 200){
      writeVoltageLog();
    }
    else{
      writeLog("End");
      trigger = false;
      logRecording = false;
      triggerInit = true;
      logFile.close();
    }
  }
  else if (logRecording){
    writeLog("End");
    logRecording = false;
    logFile.close();
  }
}

void writeVoltageLog(void){
  if (logFile) {
    logFile.print(analogRead(A8));
    logFile.print(" ");
    logFile.print(analogRead(A9));
    logFile.print(" ");
    logFile.print(analogRead(A10));
    logFile.print(" ");
    logFile.print(analogRead(A11));
    logFile.print(" ");
    logFile.print(analogRead(A12));
    logFile.print(" ");
    logFile.print(analogRead(A13));
    logFile.print(" ");
    logFile.print(analogRead(A14));
    logFile.print(" ");
    logFile.println("");
  }
}

void writeLog(String string){
  if (logFile) {
    logFile.print("================================== ");
    logFile.print(string);
    logFile.print(" ================================== ");
    logFile.println("");
  }
}
