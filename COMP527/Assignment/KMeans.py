
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import random
import os




def load_dataset(filename = 'dataset'):
    """
    parameter: filename (format: string, the name of the file)
    return: data (format: DataFrame, index is the word, column is the vector of the word)
    function: load the dataset from the file, which is in the same folder with this script
    """
    
    # path of the current script
    script_dir = os.path.dirname(__file__)
    print(script_dir)
    # full path of the file
    filepath = os.path.join(script_dir, filename)
    # read the file, the separator is ' ', and the first row is not the header
    data = pd.read_csv(filepath, sep=' ', header=None)
    # set the index to the first column
    data.set_index(0, inplace=True)
    return data

# test the function
data = load_dataset()
print(data.head())


def compute_distance(point1, point2):
    """
    parameter: point1 (format: numpy array, the first point), point2 (format: numpy array, the second point)
    return: distance (format: float, the Euclidean distance between two points)
    function: calculate the Euclidean distance between two points
    """
    # calculate the Euclidean distance between two points
    return np.sqrt(np.sum((point1 - point2) ** 2))

def initial_selection(data, k):
    """
    parameter: data (format: DataFrame, the dataset), k (format: int, the number of clusters)
    return: initial_centroids (format: DataFrame, the initial centroids)
    function: randomly select k points from the data as the initial centroids
    """
    # randomly select k points from the data as the initial centroids
    return data.sample(n = k)

def assign_cluster_IDs(data, centers):
    
    # store the cluster ID of each point
    cluster_IDs = []
    for index, point in data.iterrows():
        distances = []
        # calculate the distance between the point and each center
        for center in centers.values:
            distance = compute_distance(point, center)
            distances.append(distance)
        cluster_ID = np.argmin(distances)
        cluster_ID = np.argmin(distance)
        cluster_IDs.append(cluster_ID)
    return cluster_IDs