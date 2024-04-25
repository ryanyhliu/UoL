
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

SEED = 47

def load_dataset(filename='dataset'):
    """
    Load a dataset from a file.

    Args:
        filename (str): The name of the file to load the dataset from. Defaults to 'dataset'.

    Returns:
        data (pandas.DataFrame): The dataset.
    """
    
    script_dir = os.path.dirname(__file__) # get the directory of the script
    filepath = os.path.join(script_dir, filename) # get the path of the file
    data = pd.read_csv(filepath, sep = ' ', header = None) # split the data by space, no header
    data.set_index(0, inplace = True) # set the first column as the index
    return data.values # return Numpy


def compute_distance(point1, point2, axis = 0):
    """
    Compute the Euclidean distance between two points.

    Args:
        point1 (numpy.ndarray): The first point.
        point2 (numpy.ndarray): The second point.
    
    Returns:
        float: The Euclidean distance between the two points.
    """
    
    return np.sqrt(np.sum((point1 - point2) ** 2, axis = axis))

def initial_selection(data, k):
    """
    Randomly select k points from the data as the initial centroids.
    
    Args:
        data (pandas.DataFrame): The dataset.
        k (int): The number of clusters.
    
    Returns:
        pandas.DataFrame: The initial centroids.
    """
    
    np.random.seed(SEED)  
    indices = np.random.choice(data.shape[0], size=k, replace=False)   # randomly select k points from the data. replace: not allow the same point to be selected
    return data[indices, :] # row indices and all columns

def assign_cluster_IDs(data, centers):
    """
    Assign each point to the nearest centroid.
    
    Args:
        data (pandas.DataFrame): The dataset.
        centers (pandas.DataFrame): The centroids.
    
    Returns:
        cluster_IDs (list): The cluster ID of each point.
    """
    
    distances = [] # store the distances between each point and each center
    
    for center in centers:
        distance = compute_distance(data, center, axis = 1) # calculate the distance between each point and the center
        distances.append(distance)
    
    distances = np.array(distances) # convert the list to a numpy array
    distances = distances.T # transpose the array, so that the shape is (n, k)
    
    cluster_IDs = np.argmin(distances, axis = 1) # get the index of the minimum distance, assign to the cluster ID
    
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
        cluster_points = data[cluster_IDs == i] # all the points in the cluster i
        cluster_means = np.mean(cluster_points, axis = 0) # calculate the mean of the points in the cluster, assign to the new center
        new_centers.append(cluster_means)
    
    new_centers = np.array(new_centers) # convert the list to a numpy array
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
def compute_a(data, cluster_IDs, i):
    """
    Compute the mean distance between the point i and all other points in the same cluster. (a(i))
    
    Args:
        data (pandas.DataFrame): The dataset.
        cluster_IDs (list): The cluster ID of each point.
        i (int): The index of the point.
    
    Returns:
        mean_distance (float): The mean distance between the point i and all other points in the same cluster.
    """
    
    same_cluster_indices = np.where(cluster_IDs == cluster_IDs[i])[0]  # get the indices of the points in the same cluster
    if len(same_cluster_indices) <= 1:
        return 0
    else: 
        distances = []
        for point in same_cluster_indices: # calculate the distance between the point i and each point in the same cluster
            if point != i:
                distance = compute_distance(data[i], data[point])
                distances.append(distance)
        mean_distance = np.mean(distances)
        return mean_distance


def compute_b(data, cluster_IDs, i):
    """
    Find the closest cluster to the point i 
    and compute the mean distance between the point i and all other points in that cluster. (b(i))
    
    Args:
        data (pandas.DataFrame): The dataset.
        cluster_IDs (list): The cluster ID of each point.
        i (int): The index of the point.
        
    Returns:
        min_distances (float): The minimum distance between the point i and all other points in the other clusters.
    """

    min_distance = np.inf
    for cluster in np.unique(cluster_IDs): # get the unique cluster IDs
        if cluster != cluster_IDs[i]: # exclude the cluster of the point i itself
            other_cluster_indices = np.where(cluster_IDs == cluster)[0] # get the indices of the points in the other clusters
            
            distances = []
            for point in other_cluster_indices: # calculate the distance between the point i and each point in the other clusters
                distance = compute_distance(data[i], data[point])
                distances.append(distance)
            cluster_distance = np.mean(distances)
            
            if cluster_distance < min_distance:
                min_distance = cluster_distance
                
    return min_distance


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
    """
    Plot the silhouette score against the number of clusters.
    
    Args:
        range_k (range): The range of the number of clusters.
        silhouette_scores (list): The silhouette scores of the clustering results.
        
    Returns:
        None
    """
    
    plt.figure(figsize=(10, 6))
    plt.plot(range_k, silhouette_scores, marker='o')
    plt.xlabel('Number of Clusters, k')
    plt.ylabel('Silhouette Score')
    plt.title('Silhouette Score vs. Number of Clusters')
    plt.grid(True)
    plt.show()
    
    
def computeSumOfSquares(cluster):
    """
    Compute the sum of squares of the cluster, 
    which is the sum of the squared distances between each point and the center of the cluster.
    
    Args:
        cluster (pandas.DataFrame): The cluster.
        
    Returns:
        float: The sum of squares of the cluster.
    """
    
    if len(cluster) == 0: # critical point judge
        return 0
    
    center = np.mean(cluster, axis=0) # calculate the center of the cluster
    sum_of_squares = 0
    for point in cluster:
        sum_of_squares += compute_distance(point, center) ** 2
    return sum_of_squares

    
def bisecting_kmeans(data, num_clusters, max_iter):
    """
    Perform the bisecting k-means clustering algorithm.
    
    Args:
        data (pandas.DataFrame): The dataset.
        num_clusters (int): The number of clusters.
        max_iter (int): The maximum number of iterations.
        
    Returns:
        list: The clusters.
    """
    
    initial_sum_of_squares = computeSumOfSquares(data)
    clusters = [(data, initial_sum_of_squares)]  # store the whole dataset and its SSQ
    
    while len(clusters) < num_clusters:
        # find the cluster with the largest SSQ
        sum_of_squares = []
        for cluster, ssq in clusters:
            sum_of_squares.append(ssq)
        sum_of_squares = np.array(sum_of_squares)
        
        # get and remove the largest cluster
        largest_cluster_index = np.argmax(sum_of_squares)
        largest_cluster = clusters.pop(largest_cluster_index)[0] # for split
        

        # split the largest cluster into two clusters
        split_cluster_IDs, _ = kmeans(largest_cluster, 2, max_iter)

        # get the two new clusters, and calculate their SSQ
        cluster1 = largest_cluster[split_cluster_IDs == 0]
        cluster2 = largest_cluster[split_cluster_IDs == 1]
        clusters.append((cluster1, computeSumOfSquares(cluster1)))
        clusters.append((cluster2, computeSumOfSquares(cluster2)))

    return [cluster for cluster, _ in clusters] # return the clusters without SSQ


def main():
    data = load_dataset()
    range_k = range(1, 10) # actually, k should be larger than 1
    silhouette_scores = []

    for k in range_k:
        clusters = bisecting_kmeans(data, k, max_iter = 100)
        # create a list to store the cluster ID of each point
        cluster_IDs = np.zeros(len(data), dtype=int)
        # assign the cluster ID to each point
        cluster_index = 0
        for cluster in clusters: 
            for point in cluster: # assign the cluster ID to each point
                matches = np.all(data == point, axis=1) # find the point in the dataset
                point_index = np.where(matches)[0][0] # get the index of the point
                cluster_IDs[point_index] = cluster_index
            cluster_index += 1
            
        silhouette_score = calculate_silhouette(data, cluster_IDs)
        silhouette_scores.append(silhouette_score)

    plot_silhouette(range_k, silhouette_scores)

if __name__ == '__main__':
    main()


