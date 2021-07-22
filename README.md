<!-- vscode-markdown-toc -->
* 1. [ToDo list](#ToDolist)
* 2. [NS3](#NS3)
	* 2.1. [Running Information](#RunningInformation)
		* 2.1.1. [NS3](#NS3-1)
		* 2.1.2. [Python](#Python)
	* 2.2. [Node Information](#NodeInformation)
		* 2.2.1. [ UAV](#UAV)
		* 2.2.2. [ UserEquipment](#UserEquipment)
	* 2.3. [ Sceanrio](#Sceanrio)
		* 2.3.1. [Basic Information](#BasicInformation)
		* 2.3.2. [Topo_type](#Topo_type)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --># Sa_jiakang_NS3-MutiRL
##  1. <a name='ToDolist'></a>ToDo list
- [x] 2021/7/14 任务：优化输出信息，将Application移植到NodeUE中
- [x] 2021/7/15 任务: 测试Application的参数设置，测试uav的setdown和setup
- [x] 2021/7/16 任务: 测试命令行输入参数，构建静态场景，编辑readme
- [x] 2021/7/17 任务：python绘图，让ns3调用python程序(使用ns3-gym的解决方案)
- [x] 2021/7/21 任务：学习使用ns3-gym，让scenario打印更多信息，cite:
[ns3-gym](https://github.com/tkn-tub/ns3-gym)
- [ ] 2021/7/22 任务：测试数据，更新文档

##  2. <a name='NS3'></a>NS3
###  2.1. <a name='RunningInformation'></a>Running Information

Some useful command in `scratch\sa_jiakang\command.md` 

####  2.1.1. <a name='NS3-1'></a>NS3


- Log Setting:

    We recommand to use the level_info to see the total log

    ` export NS_LOG="scenario=level_info|prefix_func|prefix_time:nodehelper=level_info|prefix_func|prefix_time"
    `

    For just running we can set the NS_LOG with

    ` export NS_LOG= `
- Running arg setting

    Here is running Help
    ```
    sa_jiakang [Program Options] [General Arguments]

    Program Options:
        --numUAVs:    number of UAVs [4]
        --numUEs:     number of UEs [10]
        --topo_type:  type of Topo [test]
        --time_step:  timestep in simulator [4]

    General Arguments:
        --PrintGlobals:              Print the list of globals.
        --PrintGroups:               Print the list of groups.
        --PrintGroup=[group]:        Print all TypeIds of group.
        --PrintTypeIds:              Print all TypeIds.
        --PrintAttributes=[typeid]:  Print all attributes of typeid.
        --PrintHelp:                 Print this help message.
    ```
- Topo Animation

    We can check the topo with NetAnim, for my work enviroment by running

    '../netanim/NetAnim' 

    then load the `.xml` file.
####  2.1.2. <a name='Python'></a>Python

We use vitrual env to run python and installed the opengym module directly
`source ../../.venv/bin/activate`

Installed Module:
- matplotlib
- pybindgen
- pygccxml
- opengym(ns3)

###  2.2. <a name='NodeInformation'></a>Node Information
This code is in `scratch\sa_jiakang\nodehelper.h`
####  2.2.1. <a name='UAV'></a> UAV
This is a node. with two Device, one for Adhoc, one for Ap.

- Network Information
    - Device Adhoc:

    |Name|Value|
    |:---:|:---|
    |Ip|192.168.**X**.**X**|
    |Mask|255.255.255.0|
    |Route|AODV|

- Device Ap:

    |Name|Value|
    |:---:|:---|
    |Ip|172.16.**X**.**X**|
    |Mask|255.255.255.0|
    |Ssid| wifi-UAV**X**|
    |Max-Connect-Num|255|

- Necessary Value we can change 
    - Postion
    - Battery
    - Up/Down 
####  2.2.2. <a name='UserEquipment'></a> UserEquipment
This is a node with one device for Sta.
- Network Information

    - Device Sta

    |Name|Value|
    |:---:|:---|
    |Ip|Control by UAV|
    |Mask|255.255.255.0|

- Application:
    - OnoffApplication, which send Package through `UDPSocket`.
    - Necessary Value

    |Name|Value|
    |:---:|:---|
    |ParketSize|1024 byte|
    |DataRate|2048 bps|
    |Ontime|1|
    |Offtime|0|

- Necessary Value we can change 
    - Postion
    - Connect to specfic UAV 
    - DataRate
    - Switch On/Off

###  2.3. <a name='Sceanrio'></a> Sceanrio
####  2.3.1. <a name='BasicInformation'></a>Basic Information
- Necessary Value about Sceanrio.

| Name | Comment | Value |
| :-----:| :----: | :----: |
| num_uavNodes | numbers of UAV | 4 |
| num_ueNodes | numbers of UE | 10 (Default) |
| num_crNodes | numbers of Control Remote (Not Recommand to change)| 1 (Default)|
| topy_type   | test, static_dynamic_energy, static_full_energy | test (Default)|


- Construction Size: 300*300 .

- PhysicLayer:
    - PcapDataLinkType: DLT_IEEE802_11_RADIO
    - Channel: 
        - ConstantSpeedPropagationDelayModel
        - RangePropagationLossModel(Range 100)
        - Width: 20Mhz
- Wifi 
    - RateManage: ConstantRateWifiManager
    - Rate: OfdmRate54Mbps
    - Protocol: 802.11a

####  2.3.2. <a name='Topo_type'></a>Topo_type
- Test

In this topo_type, UAV and UE don't move, just for test the running of NS3.

We set 4 UAV in a Grid Position `[100,100] [100,200],[200,100],[200,200]`. And also a lot of UE in this UAVBounds. CR is in `[0,0]`.

We set all UE sent packet to CR with UDPSocket, from 15s - 70s, then off the application until program end.
    

- Static

Here is topo information in LOG_INFO.

        ```
        ----------------Basic Information------------
        ---------------------------------------------
        ------------------Postion UAV----------------
        --- UAV0:50:50:10
        --- UAV1:185:85:10
        --- UAV2:100:135:10
        --- UAV3:170:205:10
        ---------------------------------------------
        ------------------Postion UE-----------------
        --- Area1: NormalRandom
        --- mean = [200,50]
        --- variance = 20
        --- 
        --- Area2: NormalRandom
        --- mean = [100,200]
        --- variance = 20
        --- 
        --- Area3: NormalRandom
        --- mean = [250,250]
        --- variance = 20
        --- 
        ---------------------------------------------
        ------------------Postion CR-----------------
        --- position: [0,0,0]
        ```
UAV is in constant position, UE move random in this construction site.

Both full_energy and dynamic energy use same topo and same application. In this topo we set all ue sent packet with default setting.

At 30s all UE choose a random DataRate in this rate list.
```
vector<string> rate_list = {"2048bps", 
                            "4096bps", 
                            "10240bps", 
                            "102400bps", 
                            "204800bps"};
```
At 80s all UE sent packet with MAX_SPEED 204800bps.

#### Output Data
We save some neccessary data in the file, which folder pass to the topo_Type.

- throughoutput.csv

    this file save the packet/byte number in each time step and the total packet/byte.

- folder: sender
    
    this folder save the UE information with
    - Simulation Time
    - Connect with UAV index
    - Current ip 
    - Current postion
    - Current block
    - Sender State
    - Sender Rate

- folder: recevier

    this folder save the recevied package information. base on IpAddress from the socket, and check with the `ip.temp`, we can know this IpAddress in from which UE. then save the data to specfic file with
    - UE_id
    - UE_ip_current 
    - Recv time[s] 
    - Sequence num
    - Parket size[byte]
    - Sent time[s] 
    - Delay[ms]

    And there is a special case that UE sent a packet in *IP1* and then connect to another UAV and get a new *IP2*, so the receive can't find this UE with the *IP1* in socket. Base on this case ,we have `recv_from_ue_9999.csv` to save the Packet from Unknown.

- ip.temp

    as montioned in front, we don't need to consider this file, it's just for check ip for which UE.

- log.txt

    Here is the log information to check and debug, we can know how this program run by setting different log level, see Chapter.1

- .xml file

    This file is used for NetAnim to get the Visual Inforamtion for position











