- **Machine Learning - 机器学习**
  - **基本概念 - Basic Concepts**
    - **Linear Separability - 线性可分**
 数据集能够通过一个线性边界被完美划分。
    - **Probabilistic Classifiers - 概率分类器**
 基于概率理论进行分类的模型。
      - **Discriminative - 判别式模型**
 直接从输入到输出的条件概率分布建模。
      - **Generative - 生成式模型**
 通过建模数据的生成过程来进行分类。
      - **Bayes Optimal Classifier - 贝叶斯最优分类器**
 理论上最优的分类器，基于所有数据的分布信息。
      - **Maximum Likelihood Estimation - 最大似然估计**
 参数估计方法，通过最大化观测数据的可能性来估计模型参数。

  - **聚类问题 - Clustering Problems**
    - **Clustering as Classification - 聚类作为分类**
 一种视角，将聚类看作没有预定义标签的分类问题。
    - **K-Means - K均值**
 将数据分为K个聚类，以聚类内的平均值（质心）最小化每个点到质心的距离。
    - **K-Medians - K中位数**
 类似于k-means，但使用中位数而非平均值作为聚类中心。
    - **K-Medoids - K中心对象**
 选取实际存在于数据中的点作为聚类中心。
    - **Silhouette Coefficient - 轮廓系数**
 评估聚类质量的指标，测量聚类的紧密度和分离度。

  - **距离度量 - Distance Measures**
    - **Cosine Similarity - 余弦相似度**
 通过测量两个向量夹角的余弦值来评估它们的相似度。
    - **Manhattan Distance - 曼哈顿距离**
 两点在标准坐标系上绝对轴距之和。
    - **Euclidean Distance - 欧几里得距离**
 两点间的直线距离。
    - **Jaccard Similarity - 杰卡德相似度**
 用于衡量集合相似度的指标。
    - **Hamming Distance - 汉明距离**
 两个等长字符串之间的汉明距离是两个字符串对应位置的不同字符的个数。

  - **模型评估和选择 - Model Evaluation and Selection**
    - **Cross-Validation - 交叉验证**
 一种模型评估方法，通过分割数据集来估计模型的泛化能力。
    - **Parameter Tuning - 参数调优**
 调整模型参数以优化性能。
    - **Evaluation Metrics - 评估指标**
 用于衡量模型性能的各种指标，如准确率、召回率等。

  - **特征工程 - Feature Engineering**
    - **Feature Scaling - 特征缩放**
 调整不同特征的尺度以促进模型训练，包括归一化和标准化。
      - **Gaussian Normalization - 高斯归一化**
      - **Z-Score Normalization - Z-分数归一化**

  - **优化算法 - Optimization Algorithms**
    - **Gradient Descent - 梯度下降**
 通过迭代减少误差来寻找函数的最小值。
      - **Online Gradient Descent - 在线梯度下降**
 每次迭代仅使用一个样本来更新参数。
    - **Regularization - 正则化**
 添加正则项以防止模型过拟合，如L1和L2正则化。

  - **神经网络和深度学习 - Neural Networks and Deep Learning**
    - 未提及

- **数据挖掘 - Data Mining**
  - **Association Pattern Mining - 关联模式挖掘**
    - **Apriori - 预剪枝算法**
 寻找频繁项集的算法。
    - **Frequent Itemsets - 频繁项集**
 在数据集中经常一起出现的项的集合。

- **社会网络分析 - Social Network Analysis (SNA)**
  - **Centrality Measures - 中心性度量**
    - **Degree Centrality - 度中心性**
    - **Closeness Centrality - 紧密中心性**
    - **Betweenness Centrality - 介数中心性**
  - **Clustering Algorithms - 聚类算法**
    - **Hierarchical Clustering - 层次聚类**
 数据点基于相似度逐渐聚合成树状结构的聚类方法。
  - **Community Detection - 社区发现**
    - **Girvan-Newman Algorithm - Girvan-Newman算法**
 通过不断移除介数最高的边来发现社区结构。
    - **Kernighan-Lin Algorithm - Kernighan-Lin算法**
 图分割算法，旨在最小化分割后各部分之间的边。
