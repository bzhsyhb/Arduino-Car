int left1 = A0, left2 = A1, right1 = A2, right2 = A3;
int slow, mid, quick;  //three kinds of speed
int trig = 8, echo = 9;
int Auto=0;
static int a[4];

void setup() {
    pinMode(left1, OUTPUT);
    pinMode(left2, OUTPUT);
    pinMode(right1, OUTPUT);
    pinMode(right2,OUTPUT);
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

void loop() {
    

    if (Auto) auto_f();


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
    	analogWrite(left1,mid);
    	analogWrite(right1, mid);
    	int dd = judge_dis();
    	if (dd <= 30) 
    	{
    		analogWrite(left1, 0);
    		analogWrite(right1, 0);
    		auto_f();
    	}
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
	analogWrite(left1, mid);
	analogWrite(left2, 0);
	analogWrite(right1, 0);
	analogWrite(right2, mid);
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
