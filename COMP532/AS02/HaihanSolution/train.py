import gymnasium as gym
from stable_baselines3 import PPO
import torch
import os

models_path = "models"
if not os.path.exists(models_path):
    os.makedirs(models_path)

env = gym.make('LunarLander-v2')
env.reset()

model = PPO("MlpPolicy", env, verbose=1)
timesteps_per_iteration = 100
iterations = 0
total_timesteps = 0

while True:
    iterations += 1
    model.learn(total_timesteps=timesteps_per_iteration, reset_num_timesteps=False)
    if total_timesteps % 1000 == 0:
        model.save(f"{models_path}/{total_timesteps}")