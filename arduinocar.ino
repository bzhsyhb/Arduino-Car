#include <IRremote.h>
#include <IRremoteInt.h>

int left1 = 2, left2 = 3, right1 = 4, right2 = 5;   //四个电机引脚，控制方向用 
int lastl1, lastl2, lastr1, lastr2;                 //用来保存状态
int spd[3]={200, 500, 1000};  //设置三种速度
int tmp=0;                    //当前速度的编号
int nowspd=spd[0];            //速度初始化为最低速度，手动用
int spd1=A0, spd2=A1;         //控制两个电机速度：两个变量spd1,spd2
int trig = 8, echo = 9;       //超声波测距模块
int Auto=-1;                  //手动/自动,初始化为等待接收信号状态
static int a[4];              //自动模式下，记录四个方向的距离
int i, num;
int led = 10; 

//自动模式：1；手动模式：0，加速：+，减速：-，前进：2，后退：8，左转：4，右转：6.
const long manu0=0xFF6897,      //遥控器键的值
           auto1=0xFF30CF, 
           speedup=0xFFA857, 
           speeddown=0xFFE01F,
           go=0XFF18E7, 
           back=0XFF4AB5, 
           trnleft=0xFF10EF, 
           trnright=0xFF5AA5;

int RECV_PIN = 6;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
    irrecv.enableIRIn();
    pinMode(left1, OUTPUT);
    pinMode(left2, OUTPUT);
    pinMode(right1, OUTPUT);
    pinMode(right2,OUTPUT);
    pinMode(spd1, OUTPUT);
    pinMode(spd2, OUTPUT);
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    pinMode(led, OUTPUT);
    Serial.begin(9600);
}

void loop() {

    if (irrecv.decode(&results))      //确定&改变模式
    {
        irrecv.resume();
        Serial.println(results.value,HEX);

        if (results.value == manu0) Auto = 0;
        else if (results.value == auto1) Auto = 1;
    }
    delay(200);   //去扰动


    if (Auto == 1)            //自动避障模式
    {
        for (i=0;i<4;i++)
        {
        a[i] = judge_dis();   //测好当前距离之后旋转90度
        Serial.println(a[i]);
        rotate();
        delay(500);       //停一下（为了看起来不是很蠢
        }

        num = maxx();             //选出距离最大值的编号

        for (i=0;i<num;i++) rotate();    //旋转到距离最大的方向

        Serial.println(num);

        while (true)
        {
            analogWrite(spd1,spd[1]);    //自动模式下速度固定
            analogWrite(spd2, spd[1]);

            digitalWrite(left1, HIGH);   //前进
            digitalWrite(left2, LOW);
            digitalWrite(right1, HIGH);
            digitalWrite(right2, LOW);
            delay(800);                  //延时，设置每次距离判断间隔时间

            int dd = judge_dis();    
            if (dd <= 30) 
            {
                digitalWrite(left1, LOW);
                digitalWrite(right1, LOW);
                break;
            }
        }
        

    }
    else if (Auto == 0)
    {
        //Serial.println("in");

        if (irrecv.decode(&results))  //检测到按键，改变或保持当前状态
        {
            //Serial.println("read");
            //Serial.println("manual_true");
            if (results.value == go)               //前进
            {
                analogWrite(spd1, nowspd);         //设置速度
                analogWrite(spd2, nowspd);

                digitalWrite(left1, HIGH);
                digitalWrite(left2, LOW);
                digitalWrite(right1, HIGH);
                digitalWrite(right2, LOW);

                lastl1 = 1;              //保存状态
                lastl2 = 0;
                lastr1 = 1;
                lastr2 = 0;

                if (judge_dis() <= 4)          //前进防撞功能
                {
                    digitalWrite(left1, LOW);
                    digitalWrite(left2, LOW);
                    digitalWrite(right1, LOW);
                    digitalWrite(right2, LOW);

                    digitalWrite(led, HIGH);       //同时有警示灯亮
                }
                else
                {
                    digitalWrite(led,LOW);
                }

            }
            else if (results.value == back)    //后退
            {
                digitalWrite(led, LOW);

                analogWrite(spd1, nowspd);         //设置速度
                analogWrite(spd2, nowspd);

                digitalWrite(left1, LOW);
                digitalWrite(left2, HIGH);
                digitalWrite(right1, LOW);
                digitalWrite(right2, HIGH);
                
                lastl1 = 0;              //保存状态
                lastl2 = 1;
                lastr1 = 0;
                lastr2 = 1;

            }
            else if (results.value == trnleft)     //左转
            {
                digitalWrite(led, LOW);

                analogWrite(spd1, nowspd);         //设置速度
                analogWrite(spd2, nowspd);

                digitalWrite(left1, LOW);
                digitalWrite(left2, HIGH);
                digitalWrite(right1, HIGH);
                digitalWrite(right2, LOW);
                
                lastl1 = 0;
                lastl2 = 1;
                lastr1 = 1;
                lastr2 = 0;

            }
            else if (results.value == trnright)    //右转
            {
                digitalWrite(led, LOW);

                analogWrite(spd1, nowspd);         //设置速度
                analogWrite(spd2, nowspd);

                digitalWrite(left1, HIGH);
                digitalWrite(left2, LOW);
                digitalWrite(right1, LOW);
                digitalWrite(right2, HIGH);
                
                lastl1 = 1;
                lastl2 = 0;
                lastr1 = 0;
                lastr2 = 1;

            }
            else if (results.value == speedup)     //加速，改变速度只能短按不能长按
            {
                tmp++;
                if (tmp >= 3) tmp = 2;             //设置边界
                nowspd = spd[tmp];
            }
            else if (results.value == speeddown)     //减速
            {
                tmp--;
                if (tmp < 0) tmp = 0;              //边界
                nowspd = spd[tmp];
            }
            else                                  //如果是0xFFFFFFFF,延续上次状态
            {
                analogWrite(spd1, nowspd);         //设置速度
                analogWrite(spd2, nowspd);

                digitalWrite(left1, lastl1);
                digitalWrite(left2, lastl2);
                digitalWrite(right1, lastr1);
                digitalWrite(right2, lastr2);

                if (judge_dis() <= 4 && judgego(lastl1,lastl2,lastr1,lastr2))  //前进防撞
                {
                    digitalWrite(left1, LOW);
                    digitalWrite(left2, LOW);
                    digitalWrite(right1, LOW);
                    digitalWrite(right2, LOW);

                    digitalWrite(led, HIGH);
                }
                else
                {
                    digitalWrite(led, LOW);
                }
            }

            delay(100);                             //当前状态延时
        }
        else
        {
            digitalWrite(left1, LOW);
            digitalWrite(left2, LOW);
            digitalWrite(right1, LOW);
            digitalWrite(right2, LOW);
        }
        
    
    }


}

int judgego(int s1, int s2, int s3, int s4)
{
    if (s1 == 1 && s2 == 0 && s3 == 1 && s4 == 0)
        return 1;
    else return 0;
}

int judge_dis()
{
    //超声波测距
    //trig:8, echo:9

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);

    int distance = pulseIn(echo, HIGH)/58.00;   //cm
    return distance;
}

void rotate()
{
	int once=750;                //旋转90度的时间

    analogWrite(spd1,spd[1]);               //旋转使用速度spd[1]
    analogWrite(spd2, spd[1]);

    digitalWrite(left1, LOW);
    digitalWrite(left2, HIGH);
    digitalWrite(right1, HIGH);
    digitalWrite(right2, LOW);
    delay(once);

    digitalWrite(left2, LOW);
    digitalWrite(right1, LOW);

}

int maxx()
{
	int m = a[0];
	int i, t=0;
	for (i=1;i<=3;i++)
        if (a[i] > m)
        {
            m = a[i];
            t = i;
        }

   return t;
}
