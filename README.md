# Arduino小车

##介绍
<br/>
我用Arduino和一些模块实现了一个可遥控和自动避障的小车，并且两种模式可以随时切换。主要功能描述如下：<br/>
自动模式：按“1”进入自动模式，测出四个方向的距离，选择距小车最近的一个方向前进，直到前面的障碍物到小车的距离小于10厘米，再重复判断，选择一个方向前进。<br/>
遥控模式：按“0”进入遥控模式，可以选择前进、后退、向左转、向右转、速度变大、速度变小等。由于控制者的疏忽可能导致小车前进时撞上障碍物,于是我加入了前进防撞功能，在前进时小车会判断前面障碍物到小车的距离，一旦距离小于4厘米，就会自动停车无法前进，并且亮红色警示灯，直到后退、转弯或前面的障碍物被移开时小车可动并且警示灯灭。<br/><br/>

<br/>

所用器件：
--
* Arduino Uno
* 移动电源和USB线
* mini遥控器
* IR Receiver红外接收模块
* L293D电机驱动模块
* 超声波测距模块
* 红色LED
* 小车底盘和直流减速电机两个
* 面包板、面包线和杜邦线等
<br/><br/>
用到的库:[IRremote](https://github.com/z3t0/Arduino-IRremote)

<br/>
放一张小车照片：<br/>
<img src="http://7xlrda.com1.z0.glb.clouddn.com/car-pic.png" width="500px" />
