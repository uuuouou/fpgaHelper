本目录下的fpgaHelper12是在ubuntu10.10 server版下用gcc和g++编译好的可执行程序（见本目录下的makefile文件），此版的fpgaHelper12相对前一版的fpgaHelper10变动如下：
（1）修正了utc时间的显示bug
（2）采用了readline库，使得用户可以类似在shell底下一样输入命令行，有了命令记录，TAB命令补全，以及系统文件路径的TAB补全等等
（3）命令均采用小写，且对输入的命令行大小写敏感


***************************************************************************************

fpgaHelper11的更新（注意，fpgaHelper11是在fpgaHelper12上的更新）是将update函数里的magnitude由2^23-1变为2^23，同时将configureAudioSignal中每个信号的发送尝试都设为3


***************************************************************************************

fpgaHelper13的更新（注意，fpgaHelper13是在fpgaHelper12上的更新）：
（1）将audio.c中configureAudioSignal函数中每个信号的发送尝试都设为1，即configureAudioSignal仅当所有信号都一次性更新成功才返回SUCCESS
（2）对update命令的参数文件进行了限幅操作，即当参数文件中所有大于(2^23-1)/2^23的数，均被限幅为(2^23-1)/2^2，其他不变，然后将此小数乘以2^23转为24bit的有符号整数
（3）makefile中的目标文件名也顺更为fpgaHelper13
（4）本目录下还增加了对接收帧的解析程序Convert391和解析取反程序Convert392Negate（一行帧 = 2Byte帧头 + 2Byte标号 + 1Byte增益 + 3Byte信号点*128 + 2Byte校验），使用时：
Convert391[Negate] <SourceFile> <Line> <TargetFile>
以及一个产生24bit-sin信号的信号发生程序GenerateSin（程序里已经将正数限幅为(2^23-1)/2^23），使用时：
GenerateSin <PointsOfAPeriod> <TargetFile>
注意：这三个程序的exe都是x86-windows下的可执行文件


***************************************************************************************

fpgaHelper14的更新（注意，fpgaHelper14是在fpgaHelper13上的更新）：
（1）为update命令增加一个num倍数参数，即在update命令中的file后可以给定一个倍数num（不给出则默认倍数为1），在读取文件各个信号值时，会先将此值乘以倍数再将其限幅在[-1.0, (2^23-1)/2^23]范围内，最后转化成24bit有符号数传给FPGA


***************************************************************************************

fpgaHelper15的更新（注意，fpgaHelper15是在fpgaHelper14上的更新）：
（1）增加了aDIO的状态显示命令：aDIO
（2）增加了aDIO的控制命令，即对bit可控制port0和byte可控制port1两个aDIO口的控制命令：
bd0 val：控制port0的bit方向，val取值为0x00~0xFF的256个值，实现P0.0~P0.7管脚方向的任意组合;
port0 val：val取值为0x00~0xFF，控制port0各个管脚的输出值，对于输入的管脚，val对应bit是0是1都不影响;
pd1 val：控制port1的byte方向，val取值为0或1，且val=1时，port1的8个pin为输出，val=0时，port1的8个pin为输入;
port1 val：当port1为输出时，控制，各个引脚的输出值，对应bit为1时，输出为高电平
（3）makefile中相应增加了aDIO.c的编译，和fpgaHelper15对aDIO.o、librtd-aDIO.a的链接