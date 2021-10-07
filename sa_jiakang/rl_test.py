#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__author__ = "Weng Jiakang"
__copyright__ = "Copyright (c) 2021, Technische Universität Dresden"
__version__ = "0.1.0"
__email__ = "jiakang.weng@mailbox.tu-dresden.de"

import gym
import argparse
from ns3gym import ns3env
import torch
import torch.nn as nn
from torch.autograd import Variable
import torch.nn.functional as F
import numpy as np
import gym.spaces.box as box


#-------------- Self Methods----------------
def get_obs_array(obs):
    obs_array=[]
    for name,s in obs.items():
        obs_array.extend(obs[name])
    return obs_array

#-------------- NS3-GYM arguments--------------
startSim = False
iterationNum = 2
port = 5555
simTime = 10 # seconds
stepTime = 0.1  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--stepTime": stepTime,
           "--testArg": 123}
debug = False

num_uavNodes = 2
num_ueNodes = 5

#-------------- DQN arguments ---------------

# 1. Define some Hyper Parameters
BATCH_SIZE = 32     # batch size of sampling process from buffer
LR = 0.01           # learning rate
EPSILON = 0.9       # epsilon used for epsilon greedy approach
GAMMA = 0.9         # discount factor
TARGET_NETWORK_REPLACE_FREQ = 100       # How frequently target netowrk updates
MEMORY_CAPACITY = 2000                  # The capacity of experience replay buffer


#---------------- NS3env ------------------
env = gym.make('ns3-v0')
# env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim, simSeed=seed, simArgs=simArgs, debug=debug)
obs = env.reset()


obs_array = get_obs_array(obs)

ob_space = env.observation_space
ac_space = env.action_space


N_ACTIONS = 6      # 6 actions
N_STATES = 0                                  # 9 + 3* num_uav + 1* num_uav +1  states
for name,s in ob_space.spaces.items():
    N_STATES += s.shape[0]
print("type action sample : ", ac_space.sample())
ENV_A_SHAPE = 0 if isinstance(env.action_space.sample(), int) else env.action_space.sample().shape  # to confirm the shape


# print("Observation space: ", ob_space,  ob_space.dtype)
# print("Action space: ", ac_space, ac_space.dtype)

stepIdx = 0
currIt = 0

 



try:
    while True:
        print("Start iteration: ", currIt)
        obs = env.reset()
        print("Step: ", stepIdx)
        print("---obs: ", obs)

        while True:
            stepIdx += 1
            # action = ac_space.sample()
            action = [0,0,0,0]
            print("---action: ", action)

            print("Step: ", stepIdx)
            obs, reward, done, info = env.step(action)
            print("---obs, reward, done, info: ", obs, reward, done, info)
            # myVector = obs["myVector"]
            # myValue = obs["myValue"]
            # print("---myVector: ", myVector)
            # print("---myValue: ", myValue)

            # input("press enter....")

            if done:
                stepIdx = 0
                if currIt + 1 < iterationNum:
                    env.reset()
                break

        currIt += 1
        if currIt == iterationNum:
            break

except KeyboardInterrupt:
    print("Ctrl-C -> Exit")
finally:
    env.close()
    print("Done")  

