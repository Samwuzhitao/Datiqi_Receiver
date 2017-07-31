# ChangeLog:
*******************************************************************************
* **版　　本：V0.1.0**
* **开发目的** ：此版本为自由协议的初始版本，主要是要解决在天喻协议版本上
　　　　　发现的不足.
* **特性描述** : 
	* 在串口上产看白名单不在分包发送，改大协议包场支持较长的包。
	* 增加协议的版本设备识别功能，能自适应答题器设备，采用不同的协议进行通讯。
	* 优化设置指令的分配，相关功能分配为一组。
	* 增加通讯信道参数发送功率等参数的配置接口。

*******************************************************************************
* **时间**：2017-02-25
* **描述**:
> 1. 创建初始版本。
> 2. 完成串口协议的移植。
> 3. 添加接收器自有协议文档。
> 4. 完成对发送数据对旧版协议的移植。

* **时间**：2017-02-27
* **描述**:
> 1.完成发送功能的编写，基本调通接收功能。

* **时间**：2017-02-28
* **描述**:
> 1. 完成接收器与答题器之间的数据收发功能
> 2. 完成接收器与上位机之间的数据格式解析。
> 3. 修改指令的格式，及文档。

* **时间**：2017-03-01
* **描述**:
> 1. 优化无线发送函数的接口，使程序易于维护。
> 2. 完成控制指令，关机和清屏。

* **时间**：2017-03-02
* **描述**:
> 1. 完成答题器发送过滤控制指令与白名单控制指令。
> 2. 完成答题器发送参数设置指令。

 * **时间**：2017-03-03
* **描述**:
> 1. 完成刷卡配置答题器参数，并修改刷卡参数的存储方式，方便维护。

* **时间**：2017-03-06
* **描述**:
> 1. 完善协议，剔除无用文件。
> 2. 将信道设置参数支持掉电存储。

* **时间**：2017-03-08
* **描述**:
> 1. 完成答题器状态查询指令。
> 2. 完成近距离考勤与学生信息指令。
> 3. 完善指令及协议文档及用例。

* **时间**：2017-03-09
* **描述**:
> 1. 优化缓存空间的使用，串口包长最大支持750byte。

* **时间**：2017-03-10
* **描述**:
> 1. 修改文档，增加系统框图与UID的解析算法说明。

*******************************************************************************
* **版　　本:V0.1.1**
* **开发目的** : 主要解决在对接过程中的协议晦涩难懂的问题.
* **特性描述** : 
	* 修改使用JSON格式的指令与上位机交互.

*******************************************************************************
* **时间**：2017-03-23
* **描述**:
> 1. 完成串口底层接收与JSON格式的解析

* **时间**：2017-03-27
* **描述**:
> 1. 完成题目指令的下发与答案的解析
> 2. 完成上报数据->打印缓冲区->打印进程的接口
> 3. 完成发送数据进程的代码重构，简化发送逻辑

* **时间**：2017-03-28
* **描述**:
> 1. 完成20题的发送与解析格式.
> 2. 完成参数设置相关指令.

* **时间**：2017-03-29
* **描述**:
> 1. 优化函数，优化学号设置.
> 2. 剔除JSON格式化字符.
> 3. 优化格式化剔除逻辑，修改初始默认**时间**.
> 4. 修复偶数题目数据长度BUG.
> 5. 编写自己的JSON解析函数，完成80道题目的支持.
> 6. 修改打印方式，去除缓存打印，使之能支持80道题目.

* **时间**：2017-03-30
* **描述**:
> 1. 修复多选无法显示F的问题，增加对问题的错误返回.
> 2. 发送题目-3的返回.
> 3. 增加一键关机指令.
> 4. 修改答题器提交数据格式，增加RSSI显示，并提交修改文档.

* **时间**：2017-03-31
* **描述**:
> 1. 增加“查看配置”和“查看白名单”指令.
> 2. 合并“查看配置”和“查看白名单”指令为“查看配置”指令，增加导入配置指令.
> 3. 支持120个UID一次性导入，串口缓冲区改为4500，SPI接收缓冲改为5K.

* **时间**：2017-04-1
* **描述**:
> 1. 修改克隆地址逻辑，一旦导入配置，掉电有效，直至用户主动清除白名单，才会使
     用自己物理地址.

* **时间**：2017-04-5
* **描述**:
> 1. 修改answer_start,去掉total这个item,将发送功率的设置交给答题器自己处理：直接写
     入数字1~5.

* **时间**：2017-04-06
* **描述**:
> 1. 修改get_device_info,增加配置信息内容，更新文档.

* **时间**：2017-04-08
* **描述**:
> 1. 修改文档名方便git追踪.

* **时间**：2017-04-13
* **描述**:
> 1. 修复数字作答是范围解析的BUG.

* **时间**：2017-04-20
* **描述**:
> 1. 修复发送80题只能收到79题的BUG
> 2. 修改题目数超过之后的处理逻辑，返回-2错误，只截取前80题发送
> 3. 将报错调试信息改为JSON格式错误信息。
>> 增加接收缓存满错误信息
>> 增加白名单满错误信息
>> 增加json语法检查错误
>> 增加未识别指令错误

> 4. 修改设置学号的返回，分为执行返回与写数据返回
```
{'fun':'set_student_id','result':'0'}
{'fun':'set_student_id','card_id':'1340735750','student_id':'1234'}
```
> 5. 修改寻卡逻辑间隔:100ms->300ms,解决一直刷卡一直叫的问题。

*******************************************************************************
* **版　　本:V0.1.2**
* **开发目的** : 主要解决在对接过程中的协议晦涩难懂的问题.
* **特性描述** : 
	* 增加bootloder功能，支持串口程序更新.
*******************************************************************************
* **时间**：2017-04-21
* **描述**:
> 1. 增加bootloder功能，支持串口程序更新

*******************************************************************************
* **版　　本:V0.1.3**
* **开发目的** : 主要解决在对接过程中的协议晦涩难懂的问题.
* **特性描述** : 
	* 增加远距离考勤功能，答题器调试指令.
*******************************************************************************
* **时间**：2017-05-02
* **描述**:
> 1. 修改JSON格式，使用双引号风格
> 2. 增加绑定过程信息的返回
> 3. 增加2.4G考勤设置指令
> 4. 增加答题器自检指令

* **时间**：2017-05-03
* **描述**:
> 1. 修复自检测试指令中返回包号不变的BUG

* **时间**：2017-05-08
* **描述**:
> 1. 重新建立工程解决工程不能调试的问题
> 2. 实现UID的FIFO的滑动存储逻辑，减少清除白名单的麻烦

* **时间**：2017-05-09
* **描述**:
> 1. 升级自有协议支持举手和签到和通用题型

* **时间**：2017-05-10
* **描述**:
> 1. 修改软件版本

* **时间**：2017-05-11
* **描述**:
> 1. 修改通用题型解析的BUG，无法解析判断

* **时间**：2017-05-12
* **描述**:
> 1. 修复信道不能设置为11和1的BUG

* **时间**：2017-05-15
* **描述**:
> 1. 为信道设置增加反馈功能，设置信道之后有3次校验过程，都失败才算是失败

* **时间**：2017-05-17
* **描述**:
> 1. 修改自检指令需要绑定的BUG，设置学号重复返回的BUG
> 2. 修改2.4G考勤设置的字符串数据开头的BUG

* **时间**：2017-05-18
* **描述**:
> 1. 修复刷卡绑定过程中，使用FIFO机制，无法替换第120个UID的BUG

* **时间**：2017-05-23
* **描述**:
> 1. 优化接收数据过滤逻辑，支持双引号内部带有空格
> 2. 从发送题目中分离举手和签到功能，单独设置
> 3. 在发送的数据域增加逻辑包号
> 4. 修复发送题目的前导帧，指针越界的BUG

* **时间**：2017-05-24
* **描述**:
> 1. 修复发送自检指令会有异常RSSI值的BUG
> 2. 增加调试信息设置指令
> 3. 修改逻辑包号增加逻辑，只有再收到发送题目指令时才会增加

* **时间**：2017-06-05
* **描述**:
> 1. 修复没有发送题目指令直接发送举手指令，无法发送的BUG
> 2. 剥离发送题目与举手和签到之间关联，使之互不影响

* **时间**：2017-06-06
* **描述**:
> 1. 修复发送导入配置信息引起发送题目指令无法识别的BUG
> 2. 修复发送停止作答导致，举手签到信息清零BUG
> 3. 修复串口接收数据是JSON格式过滤的BUG
> 4. 修改啥卡过程JSON数据格式，默认使用单引号输出
> 5. 修改程序查询版本号
> 6. 调整2.4G考勤的item含义将信道改为协议索引

*******************************************************************************
* **版　　本:V0.1.4**
* **开发目的** : 支持新版硬件RP552.
* **特性描述** : 
	* 支持新版硬件RP552.
*******************************************************************************
* **时间**：2017-05-10
* **描述**:
> 1. 修改工程设置，开始新硬件的支持工作

* **时间**：2017-05-11
* **描述**:
> 1. 修改工程设置，完成宏支持RP552的框架的搭建

* **时间**：2017-05-12
* **描述**:
> 1. 修改工程，完成SPI层逻辑的编写，能正常完成对MF1702数据的编写

* **时间**：2017-05-15
* **描述**:
> 1. 发现SPI接口读取返回数据的时候，会有检验出错的BUG
> 2. 解决SPI读数据校验出错的BUG，完成13.56Mhz驱动的移植
> 3. 剔除调试信息

*******************************************************************************
* **版　　本:V0.1.5**
* **开发目的** : 优化软件代码，为了分离硬件驱动代码，减少硬件移植的难度
* **特性描述** : 
	* 支持新版硬件RP552.
*******************************************************************************
* **时间**：2017-05-17
* **描述**:
> 1. 开始驱动的分离,基本完成框架的搭建

* **时间**：2017-06-09
* **描述**:
> 1. 同步V0103最新的版本，工程设置输出固件加上日期

* **时间**：2017-06-12
* **描述**:
> 1. 细化初始化信息逻辑，修改MCU为103RC
> 2. 完成发送端SPI的移植
> 3. 将发送端的宏配置接口，在头文件中定义
> 4. 与答题器之间约定数字不按的默认返回解决无法区0分是否为分为选择的问题
> 5. 在发送数据前发送一次设置信道指令，避免发生信道不同步的情况
> 6. 修复自检指令解析RSSI出错的BUG

* **时间**：2017-06-13
* **描述**:
> 1. 增加RF状态检查与复位逻辑
> 2. 修复答题器刷卡不成功，单写入白名单，第二次刷卡不发中断的BUG
> 3. 修改软件版本号为V0.2.0
> 4. 修改射频失效的错误提示
> 5. 完善发送与接收端的复位逻辑

* **时间**：2017-06-14
* **描述**:
> 1. 修改硬件名，修改对2.4G考情的设置格式
> 2. 将之前去掉RF设置的检测加回来

* **时间**：2017-06-15
* **描述**:
> 1. 修改2.4G考勤的JSON指令格式

* **时间**：2017-06-19
* **描述**:
> 1. 修改答题器设置学号的返回格式，增加对发送中断结果返回的判断
> 2. 修改查询的软件版本号为2.1

* **时间**：2017-06-26
* **描述**:
> 1. 修复设置学号指令返回JSON格式错误的BUG

* **时间**：2017-07-11
* **描述**:
> 1. 修改软件版本号为V0.22
> 2. 修复RX与TX的程序，增加SPI检测程序是否下载正确的检查

* **时间**：2017-07-31
* **描述**:
> 1. 修复RX与TX的程序下错，有种错误无上报的BUG
