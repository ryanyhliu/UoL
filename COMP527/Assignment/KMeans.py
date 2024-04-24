
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import random
import os



def load_dataset(filename='dataset'):
    """
    Load a dataset from a file.

    Args:
        filename (str): The name of the file to load the dataset from. Defaults to 'dataset'.

    Returns:
        data (pandas.DataFrame): The dataset.
    """
    # path of the current script
    script_dir = os.path.dirname(__file__)
    # Rest of the code...
    print(script_dir)
    # full path of the file
    filepath = os.path.join(script_dir, filename)
    # read the file, the separator is ' ', and the first row is not the header
    data = pd.read_csv(filepath, sep=' ', header=None)
    # set the index to the first column
    data.set_index(0, inplace=True)
    return data


def compute_distance(point1, point2):
    """
    Compute the Euclidean distance between two points.

    Args:
        point1 (numpy.ndarray): The first point.
        point2 (numpy.ndarray): The second point.
    
    Returns:
        float: The Euclidean distance between the two points.
    """
    
    return np.sqrt(np.sum((point1 - point2) ** 2))

def initial_selection(data, k):
    """
    Randomly select k points from the data as the initial centroids.
    
    Args:
        data (pandas.DataFrame): The dataset.
        k (int): The number of clusters.
    
    Returns:
        pandas.DataFrame: The initial centroids.
    """
    
    return data.sample(n = k) # randomly select k points from the data as the initial centroids

def assign_cluster_IDs(data, centers):
    """
    Assign each point to the nearest centroid.
    
    Args:
        data (pandas.DataFrame): The dataset.
        centers (pandas.DataFrame): The centroids.
    
    Returns:
        cluster_IDs (list): The cluster ID of each point.
    """
    
    cluster_IDs = [] # store the cluster ID of each point
    for index, point in data.iterrows(): # iterrows() returns the index and the row of the DataFrame
        distances = []
        for center in centers.values: # calculate the distance between the point and each center
            distance = compute_distance(point, center)
            distances.append(distance)
        
        cluster_ID = np.argmin(distances) # get the index of the minimum distance, assign to the cluster ID
        cluster_IDs.append(cluster_ID)
    return cluster_IDs

def compute_cluster_representatives(data, cluster_IDs, k):
    """
    Compute the new centroids of the clusters.
    
    Args:
        data (pandas.DataFrame): The dataset.
        cluster_IDs (list): The cluster ID of each point.
        k (int): The number of clusters.
        
    Returns:
        new_centers (pandas.DataFrame): The new centroids.
    """
    
    new_centers = [] # store the new centroids
    for i in range(k): # for each cluster k
        members = data[np.array(cluster_IDs) == i] # all the points in the cluster i
        if not members.empty: # if the cluster is not empty
            new_center = members.mean(axis = 0) # calculate the mean of the points in the cluster, assign to the new center
            new_centers.append(new_center) 
        else: # if the cluster is empty
            new_center = data.sample(n = 1).iloc[0] # randomly select a point from the data as the new center
            new_centers.append(new_center)
    
    return new_centers

def kmeans(data, k, max_iter = 100):
    """
    Perform the k-means clustering algorithm.
    
    Args:
        data (pandas.DataFrame): The dataset.
        k (int): The number of clusters.
        max_iter (int): The maximum number of iterations. Defaults to 100.
        
    Returns:
        cluster_IDs (list): The cluster ID of each point.
        centers (pandas.DataFrame): The centroids.
    """
    
    centers = initial_selection(data, k) # randomly select k points from the data as the initial centroids
    for i in range(max_iter):
        cluster_IDs = assign_cluster_IDs(data, centers) # assign each point to the nearest centroid
        new_centers = compute_cluster_representatives(data, cluster_IDs, k) # compute the new centroids of the clusters
        centers = new_centers # update the centroids
    return cluster_IDs, centers

# implement the silhouette method
def compute_a(data, labels, i):
    """
    Compute the mean distance between the point i and all other points in the same cluster. (a(i))
    
    Args:
        data (pandas.DataFrame): The dataset.
        labels (list): The cluster ID of each point.
        i (int): The index of the point.
    
    Returns:
        mean_distance (float): The mean distance between the point i and all other points in the same cluster.
    """
    
    same_cluster = data[labels == labels[i]]
    if len(same_cluster) <= 1: # if there is only one point in the same cluster, return 0
        return 0
    else: # if there are more than one point in the same cluster, calculate the mean distance
        distances = []
        for points in range(len(data)): # calculate the distance between the point i and each point in the same cluster
            if labels[points] == labels[i] and points != i: # exclude the point i itself
                distance = compute_distance(data[i], data[points]) 
                distances.append(distance)
        mean_distance = np.mean(distances)
        return mean_distance

def compute_b(data, labels, i):
    """
    Compute the mean distance between the point i and all other points in the other clusters. (b(i))
    
    Args:
        data (pandas.DataFrame): The dataset.
        labels (list): The cluster ID of each point.
        i (int): The index of the point.
        
    Returns:
        min_distances (float): The minimum distance between the point i and all other points in the other clusters.
    """
    
    other_clusters = set(labels) - set([labels[i]]) # get the cluster IDs of other clusters
    min_distances = float('inf') # store the minimum distance, initialize as infinity
    for cluster in other_clusters:
        other_clusters_points = data[labels == cluster] # all the points in the other clusters
        
        # calculate the mean distance between the point i and all the points in the other clusters
        distances = []
        for points in range(len(other_clusters_points)):
            distance = compute_distance(data[i], other_clusters_points.iloc[points]) # iloc[] is used to access the row by index
            distances.append(distance)
        mean_distance = np.mean(distances)

        if mean_distance < min_distances:
            min_distances = mean_distance
            
    return min_distances

def calculate_silhouette(data, cluster_IDs):
    """
    Calculate the silhouette value of the clustering result.
    
    Args:
        data (pandas.DataFrame): The dataset.
        cluster_IDs (list): The cluster ID of each point.
        
    Returns:
        float: The silhouette value of the clustering result.
    """
    
    silhouette = [] # store the silhouette value of each point
    for i in range(len(data)):
        a_i = compute_a(data, cluster_IDs, i)
        b_i = compute_b(data, cluster_IDs, i)
        if a_i == 0 and b_i == 0: # if a(i) and b(i) are both 0, the silhouette value is 0
            silhouette.append(0) 
        else:
            silhouette.append((b_i - a_i) / max(a_i, b_i))
    return np.mean(silhouette) # return the mean silhouette value


def plot_silhouette(range_k, silhouette_scores):
    plt.figure(figsize=(10, 6))
    plt.plot(range_k, silhouette_scores, marker='o')
    plt.xlabel('Number of Clusters, k')
    plt.ylabel('Silhouette Score')
    plt.title('Silhouette Score vs. Number of Clusters')
    plt.grid(True)
    plt.show()

def main():
    if __name__ == '__main__':
        main()
    
    data = load_dataset()
    range_k = range(2, 10) # begin with 2 clusters because 1 cluster is meaningless
    silhouette_scores = []
    
    for k in range_k:
        cluster_IDs, centers = kmeans(data, k, max_iter = 100)
        silhouette_score = calculate_silhouette(data, cluster_IDs)
        silhouette_scores.append(silhouette_score)
    
    plot_silhouette(range_k, silhouette_scores)
