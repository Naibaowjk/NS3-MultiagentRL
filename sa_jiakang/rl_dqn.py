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
simTime = 100 # seconds
stepTime = 4  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--stepTime": stepTime,
           "--testArg": 123}
debug = False

num_uavNodes = 4
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


N_ACTIONS = 6       # 6 actions
N_STATES = 0                                  # 9 + 3* num_uav + 1* num_uav +1  states
for name,s in ob_space.spaces.items():
    N_STATES += s.shape[0]
print("type action sample : ", ac_space.sample())
ENV_A_SHAPE = 0 if isinstance(env.action_space.sample(), int) else env.action_space.sample().shape  # to confirm the shape


# print("Observation space: ", ob_space,  ob_space.dtype)
# print("Action space: ", ac_space, ac_space.dtype)

stepIdx = 0
currIt = 0


#----------------DQN Class------------------
# 2. Define the network used in both target net and the net for training
class Net(nn.Module):
    def __init__(self):
        # Define the network structure, a very simple fully connected network
        super(Net, self).__init__()
        # Define the structure of fully connected network
        self.fc1 = nn.Linear(N_STATES, 64)  # layer 1
        self.fc1.weight.data.normal_(0, 0.1) # in-place initilization of weights of fc1
        self.fc2 = nn.Linear(64,32)
        self.fc2.weight.data.normal_(0, 0.1)
        self.out = nn.Linear(32, N_ACTIONS*num_uavNodes) # layer out
        self.out.weight.data.normal_(0, 0.1) # in-place initilization of weights of fc2
        self.device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
        
        
    def forward(self, x):
        # Define how the input data pass inside the network
        x = self.fc1(x)
        x = self.fc2(x)
        x = F.relu(x)
        actions_value = self.out(x)
        return actions_value
        
# 3. Define the DQN network and its corresponding methods
class DQN(object):
    def __init__(self):
        # -----------Define 2 networks (target and training)------#
        self.eval_net, self.target_net = Net(), Net()
        # Define counter, memory size and loss function
        self.learn_step_counter = 0 # count the steps of learning process
        self.memory_counter = 0 # counter used for experience replay buffer
        
        # ----Define the memory (or the buffer), allocate some space to it. The number 
        # of columns depends on 4 elements, s, a, r, s_, the total is N_STATES*2 + 2---#
        self.memory = np.zeros((MEMORY_CAPACITY, N_STATES * 2 + num_uavNodes+1)) 
        
        #------- Define the optimizer------#
        self.optimizer = torch.optim.Adam(self.eval_net.parameters(), lr=LR)
        
        # ------Define the loss function-----#
        self.loss_func = nn.MSELoss()
        
    def  choose_action(self, x):
        # This function is used to make decision based upon epsilon greedy
        
        x = torch.unsqueeze(torch.FloatTensor(x), 0) # add 1 dimension to input state x
        # input only one sample
        if np.random.uniform() < EPSILON:   # greedy
            # use epsilon-greedy approach to take action
            actions_value = self.eval_net.forward(x)
            #print(torch.max(actions_value, 1)) 
            # torch.max() returns a tensor composed of max value along the axis=dim and corresponding index
            # what we need is the index in this function, representing the action of cart.
            actions_value_numpy = actions_value.detach().numpy()
            action = []
            for i in range(num_uavNodes):
                actions_value_i = actions_value[0, 0 + N_ACTIONS * i: 6 + N_ACTIONS * i]
                # print(torch.max(actions_value_i, 0))
                action_i = torch.max(actions_value_i, 0)[1].data.numpy()
                action.append(action_i+0)
            # action = action[0] if ENV_A_SHAPE == 0 else action.reshape(ENV_A_SHAPE)  # return the argmax index
        else:   # random
            action=[]
            for i in range(num_uavNodes):
                action_i = np.random.randint(0, N_ACTIONS)
                action.append(action_i)
            # action = action if ENV_A_SHAPE == 0 else action.reshape(ENV_A_SHAPE)
        return action
    
        
    def store_transition(self, s, a, r, s_):
        # This function acts as experience replay buffer        
        # horizontally stack these vectors
        transition = np.hstack((s, a, [r], s_))
        # if the capacity is full, then use index to replace the old memory with new one
        index = self.memory_counter % MEMORY_CAPACITY
        self.memory[index, :] = transition
        self.memory_counter += 1
        
    
    def learn(self):
        # Define how the whole DQN works including sampling batch of experiences,
        # when and how to update parameters of target network, and how to implement
        # backward propagation.
        
        # update the target network every fixed steps
        if self.learn_step_counter % TARGET_NETWORK_REPLACE_FREQ == 0:
            # Assign the parameters of eval_net to target_net
            self.target_net.load_state_dict(self.eval_net.state_dict())
        self.learn_step_counter += 1
        
        # Determine the index of Sampled batch from buffer
        sample_index = np.random.choice(MEMORY_CAPACITY, BATCH_SIZE) # randomly select some data from buffer
        # extract experiences of batch size from buffer.
        b_memory = self.memory[sample_index, :]
        # extract vectors or matrices s,a,r,s_ from batch memory and convert these to torch Variables
        # that are convenient to back propagation
        b_s = Variable(torch.FloatTensor(b_memory[:, :N_STATES]))
        # convert long int type to tensor
        b_a = Variable(torch.LongTensor(b_memory[:, N_STATES:N_STATES+num_uavNodes].astype(int)))
        b_r = Variable(torch.FloatTensor(b_memory[:, N_STATES+num_uavNodes:N_STATES+num_uavNodes+1]))
        b_s_ = Variable(torch.FloatTensor(b_memory[:, -N_STATES:]))
        
        # calculate the Q value of state-action pair
        q_eval = self.eval_net(b_s).gather(1, b_a) # (batch_size, 1)
        #print(q_eval)
        # calculate the q value of next state
        q_next = self.target_net(b_s_).detach() # detach from computational graph, don't back propagate
        # select the maximum q value
        #print(q_next)
        # q_next.max(1) returns the max value along the axis=1 and its corresponding index
        q_target = b_r + GAMMA * q_next.max(1)[0].view(BATCH_SIZE, 1) # (batch_size, 1)
        loss = self.loss_func(q_eval, q_target)
        
        self.optimizer.zero_grad() # reset the gradient to zero
        loss.backward()
        self.optimizer.step() # execute back propagation for one step
 


""" 
try:
    while True:
        print("Start iteration: ", currIt)
        obs = env.reset()
        print("Step: ", stepIdx)
        print("---obs: ", obs)

        while True:
            stepIdx += 1
            action = env.action_space.sample()
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

"""

'''
--------------Procedures of DQN Algorithm------------------
'''
# create the object of DQN class
dqn = DQN()

# Start training
print("\nCollecting experience...")
for i_episode in range(400):
    print("episode : ", i_episode)

    # play 400 episodes of cartpole game
    s_array = get_obs_array(env.reset())
    
    ep_r = 0
    while True:
        env.render()
        # take action based on the current state
        a = dqn.choose_action(s_array)
        # obtain the reward and next state and some other information
        s_, r, done, info = env.step(a)

        print("env step finish: ", s_, r, done ,info)

        s_array_ = get_obs_array(s_)
        
        # store the transitions of states
        dqn.store_transition(s_array, a, r, s_array_)
        
        ep_r += r
        # if the experience repaly buffer is filled, DQN begins to learn or update
        # its parameters.       
        if dqn.memory_counter > MEMORY_CAPACITY:
            dqn.learn()
            if done:
                print('Ep: ', i_episode, ' |', 'Ep_r: ', round(ep_r, 2))
        
        if done:
            # if game is over, then skip the while loop.
            break
        # use next state to update the current state. 
        s_array = s_array_  
