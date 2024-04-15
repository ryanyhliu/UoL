import gymnasium as gym
from stable_baselines3 import PPO
import torch

env = gym.make('LunarLander-v2', render_mode='human')
env.reset()
model_path = "models/0.zip"
model = PPO.load(model_path, env=env)
vec_env = model.get_env()

for episode in range(10):
    obs = vec_env.reset()
    done = False
    while not done:
        action, _ = model.predict(obs)
        obs, reward, done, info = vec_env.step(action)
        env.render()
        print(reward)