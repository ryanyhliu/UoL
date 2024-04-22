import gymnasium as gym
from stable_baselines3 import PPO
import torch
import os
import csv

current_dir = os.path.dirname(__file__)
models_path = os.path.join(current_dir, "models")
if not os.path.exists(models_path):
    os.makedirs(models_path)

env = gym.make('LunarLander-v2')
env.reset()

model = PPO("MlpPolicy", env, verbose=1, tensorboard_log=f"{current_dir}/tensorboard")
# model = PPO("MlpPolicy", env, verbose=1)
model.learn(total_timesteps=17000, reset_num_timesteps=False)
model.save(f"{models_path}/ppo_lunar")