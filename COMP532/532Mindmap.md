# 机器学习与强化学习全面关键词导图

## 机器学习 (Machine Learning)
### 监督学习 (Supervised Learning)
- **标签 (Labels)**: 数据的正确答案，指导模型学习。
### 无监督学习 (Unsupervised Learning)
- **聚类 (Clustering)**: 将数据分组，组内相似度高，组间相似度低。
### 强化学习 (Reinforcement Learning, RL)
- **策略 (Policy)**: 决定行动选择的规则。
- **奖励 (Reward)**: 对行动的评价，引导学习方向。
- **环境 (Environment)**: 智能体进行学习的场景。

## 强化学习技术 (Reinforcement Learning Techniques)
### 探索和利用 (Exploration vs Exploitation)
- **epsilon-greedy (ε-greedy)**: 一种简单的策略，以ε概率探索新行动。
### Markov决策过程 (Markov Decision Processes, MDP)
- **状态转换图 (State Transition Graph)**: 描述状态间转换关系的图。
- **贝尔曼方程 (Bellman Equation)**: 递归定义价值函数的方程。

## 深度学习 (Deep Learning)
### 神经网络基础 (Neural Networks Basics)
- **感知器 (Perceptron)**: 最早的神经网络模型，基于阈值判断输出。
- **激活函数 (Activation Functions)**
  - **Sigmoid**: 将输入压缩至(0,1)区间，常用于输出层。
  - **ReLU (Rectified Linear Unit)**: 提供非线性模型能力，加快训练。
### 网络结构 (Network Architectures)
- **卷积神经网络 (Convolutional Neural Networks, CNNs)**
  - **池化层 (Pooling Layer)**: 减少数据维度，保留重要信息。
- **循环神经网络 (Recurrent Neural Networks, RNNs)**
  - **长短期记忆网络 (LSTM)**: 解决RNN短期记忆问题，捕捉长期依赖。

## 训练与优化 (Training & Optimization)
### 损失函数 (Loss Functions)
- **均方误差 (Mean Squared Error, MSE)**: 衡量预测值与实际值差异的常用方法。
### 优化算法 (Optimization Algorithms)
- **梯度下降 (Gradient Descent)**: 通过迭代减少损失函数值，更新模型权重。
- **反向传播 (Backpropagation)**: 计算梯度，用于神经网络的训练过程。

## 多智能体学习 (Multi-Agent Learning)
### 博弈论基础 (Game Theory Basics)
- **纳什均衡 (Nash Equilibrium)**: 无一方能单独改变策略以提高自身利益的状态。
### 独立学习方法 (Independent Learning Methods)
- **独立Q学习 (Independent Q-Learning)**: 每个智能体独立更新其Q值。

## 高级话题 (Advanced Topics)
### 神经网络进阶 (Advanced Neural Networks)
- **自编码器 (AutoEncoders)**: 通过编码和解码过程学习数据压缩表示。
- **转移学习 (Transfer Learning)**: 利用一个任务学到的知识应用于另一个相关任务。
### 强化学习算法 (Reinforcement Learning Algorithms)
- **SARSA (State-Action-Reward-State-Action)**: 考虑下一步行动的Q学习算法。
- **Q学习 (Q-Learning)**: 直接从最优Q值学习策略，不依赖模型。

## 其他重要概念 (Other Important Concepts)
### 梯度下降变体 (Variants of Gradient Descent)
- **动量 (Momentum)**: 加速梯度下降，避免陷入局部最小值。
- **Adagrad, Adam**: 自适应学习率优化算法，提高训练效率。
### 博弈论进阶 (Advanced Game Theory)
- **重复博弈 (Repeated Games)**: 玩家重复进行相同的博弈，策略可能随时间变化。

