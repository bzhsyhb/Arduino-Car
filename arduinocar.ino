#include <IRremote.h>
#include <IRremoteInt.h>

int left1 = A0, left2 = A1, right1 = A2, right2 = A3;
int spd[3];  //three kinds of speed for manu
int tmp=0;   //speed number
int nowspd=spd[0];
int trig = 8, echo = 9;
int Auto=0;
static int a[4];

const long manu0=0xFF6897, auto1=0xFF30CF, speedup=0xFFA857, speeddown=0xFFE01F;
const long go=0XFF18E7, back=0XFF4AB5, trnleft=0xFF10EF, trnright=0xFF5AA5;

int RECV_PIN = 3;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
    irrecv.enableIRIn();
    pinMode(left1, OUTPUT);
    pinMode(left2, OUTPUT);
    pinMode(right1, OUTPUT);
    pinMode(right2,OUTPUT);
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

void loop() {
    
    if (irrecv.decode(&results))
    {
        if (results.value == manu0) Auto = 0;
        else if (results.value == auto1) Auto = 1;
    }
    irrecv.resume();
    delay(200);

    if (Auto) auto_f();
    else manual_f();

}

void auto_f()
{
    //choose direction
    //A0，A1:left;  A2,A3:right
    int i, num;
    
    for (i=0;i<4;i++)
    {
    	a[i] = judge_dis();
    	rotate();
    }

    num = maxx();
    for (i=0;i<num;i++) rotate();

    while (true)
    {
    	analogWrite(left1,spd[1]);
    	analogWrite(right1, spd[1]);
    	int dd = judge_dis();
    	if (dd <= 30) 
    	{
    		analogWrite(left1, 0);
    		analogWrite(right1, 0);
    		auto_f();
    	}
    }

}

void manual_f()
{
    while (true)
    {
        if (irrecv.decode(&results))  //press, change or keep state
        {
            if (results.value == go) 
            {
                left1 = nowspd;
                right1 = nowspd;
                left2 = 0;
                right2 = 0;
            }
            else if (results.value == back)
            {
                left1 = 0;
                right1 = 0;
                left2 = nowspd;
                right2 = nowspd;
            }
            else if (results.value == trnleft)
            {
                left1 = 0;
                right1 = spd[0];
                left2 = 0;
                right2 = 0;
            }
            else if (results.value == trnright)
            {
                left1 = spd[0];
                right1 = 0;
                left2 = 0;
                right2 = 0;
            }
            else if (results.value == speedup)
            {
                tmp++;
                if (tmp >= 3) tmp = 2;
            }
            else if (results.value == speeddown)
            {
                tmp--;
                if (tmp < 0) tmp = 0;
            }

        }
        else                               //else, stop
        {
            left1 = 0;
            right1 = 0;
            left2 = 0;
            right2 = 0;
        }
        irrecv.resume();
    }
}

int judge_dis()
{
    //super
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
	int once; //time of rotating once
	analogWrite(left1, spd[1]);
	analogWrite(left2, 0);
	analogWrite(right1, 0);
	analogWrite(right2, spd[1]);
	delay(once);

	analogWrite(left1, 0);
	analogWrite(right2, 0);

}

int maxx()
{
	int m = a[0];
	int i;
	for (i=1;i<=3;i++)
	    if (a[i] > m) m = a[i];

	return m;
}
