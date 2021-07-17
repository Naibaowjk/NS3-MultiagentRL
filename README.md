<!-- vscode-markdown-toc -->
* 1. [ToDo list](#ToDolist)
* 2. [NS3](#NS3)
	* 2.1. [Running Information](#RunningInformation)
		* 2.1.1. [NS3](#NS3-1)
		* 2.1.2. [Python](#Python)
	* 2.2. [Code Information](#CodeInformation)
		* 2.2.1. [ UAV](#UAV)
		* 2.2.2. [ UserEquipment](#UserEquipment)
		* 2.2.3. [ Sceanrio](#Sceanrio)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --># NS3-MultiagentRL
##  1. <a name='ToDolist'></a>ToDo list
- [x] 2021/7/14 任务：优化输出信息，将Application移植到NodeUE中
- [x] 2021/7/15 任务: 测试Application的参数设置，测试uav的setdown和setup
- [x] 2021/7/16 任务: 测试命令行输入参数，构建静态场景，编辑readme
- [ ] 2021/7/17 任务：python绘图，让ns3调用python程序

##  2. <a name='NS3'></a>NS3
###  2.1. <a name='RunningInformation'></a>Running Information
####  2.1.1. <a name='NS3-1'></a>NS3
There are some useful command in `scratch\sa_jiakang\command.txt` 
- Log Setting:

    We recommand to use the level_info to see the total log

    ` export NS_LOG="scenario=level_info|prefix_func|prefix_time:nodehelper=level_info|prefix_func|prefix_time"
    `

    For just running we can set the NS_LOG with

    ` export NS_LOG=`
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

    '../netanim-3.108/NetAnim' 

    then load the `.xml` file.
####  2.1.2. <a name='Python'></a>Python
**Still working on with it**

###  2.2. <a name='CodeInformation'></a>Code Information
This code is in `scratch\sa_jiakang`
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

####  2.2.3. <a name='Sceanrio'></a> Sceanrio
##### Basic Information
- Necessary Value about Sceanrio.

| Name | Comment | Value |
| :-----:| :----: | :----: |
| num_uavNodes | numbers of UAV (Not Recommand to change)| 4 |
| num_ueNodes | numbers of UE | 10 |
| num_crNodes | numbers of Control Remote (Not Recommand to change)| 1 |
| topy_type   | test, static_dynamic_energy, static_full_energy | test|


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

##### Topo_type
- Test

    in this topo_type, UAV and UE don't move, just for test the running of NS3.
        - Animation: `test.xml`
        - LogData: `test.txt`
        - Throughput Data: `throughput-test.csv`

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

    - Full_Energy: 

        In this case we don't consider the energy of UAV.
        
        - Animation: `static_full_energy.xml`
        - LogData: `static_full_energy.txt`
        - Throughput Data: `throughput-static_full_energy.csv`

    - Dynamic_Energy (still working on): 

        In this case we considered the energy of UAV.
       
        - Animation: `static_dynamic_energy.xml`
        - LogData: `static_dynamic_energy.txt`
        - Throughput Data: `throughput-static_dynamic_energy.csv`









