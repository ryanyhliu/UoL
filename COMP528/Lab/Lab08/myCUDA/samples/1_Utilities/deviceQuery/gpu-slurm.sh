#!/bin/bash -l

#SBATCH -p gpu
#SBATCH --gres=gpu:1
#SBATCH -D ./
#SBATCH --export=ALL

./deviceQuery
