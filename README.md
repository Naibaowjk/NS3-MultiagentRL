<!-- vscode-markdown-toc -->
* 1. [ToDo list](#ToDolist)
* 2. [NS3](#NS3)
	* 2.1. [ UAV](#UAV)
	* 2.2. [ UserEquipment](#UserEquipment)
	* 2.3. [ Sceanrio](#Sceanrio)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --># NS3-MultiagentRL
##  1. <a name='ToDolist'></a>ToDo list
- [x] 2021/7/14 任务：优化输出信息，将Application移植到NodeUE中
- [x] 2021/7/15 任务: 测试Application的参数设置，测试uav的setdown和setup
- [ ] 2021/7/16 任务: 测试命令行输入参数,构建静态场景, 编辑readme

##  2. <a name='NS3'></a>NS3

This code is in `Scratch\Scenario`
###  2.1. <a name='UAV'></a> UAV
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
###  2.2. <a name='UserEquipment'></a> UserEquipment
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

###  2.3. <a name='Sceanrio'></a> Sceanrio
- Necessary Value about construction Site. Not recommand to change.

| Name | Comment | Value |
| :-----:| :----: | :----: |
| num_uavNodes | numbers of UAV | 4 |
| num_ueNodes | numbers of UE | 10 |
| num_crNodes | numbers of Control Remote | 1 |

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

- Static Topo: `static-case.xml`







