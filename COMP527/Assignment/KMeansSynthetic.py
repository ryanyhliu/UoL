import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

SEED = 47


def generate_data():
    """
    Generate synthetic data for K-means clustering. 
    The data has 327 rows and 300 columns. The first column is the cluster ID.
    
    Args:
        seed (int): The random seed.
    
    Returns:
        data (numpy.ndarray): The synthetic data.
    """
    
    try:
        np.random.seed(SEED)
        random_data = np.random.normal(loc=0.0, scale=1.0, size=(327, 300)) # generate random data with 327 rows and 300 columns
        # No need for labels
        # labels = np.array([f"Cluster{i+1:03d}" for i in range(327)]).reshape(327, 1) # generate labels like 'Cluster001', 'Cluster002', ..., 'Cluster327'
        # data = np.hstack((labels, random_data)) # combine the labels and random data
        return random_data
    except Exception as e:
        print(f"An error occurred while generating synthetic data: {str(e)}")
        return None


def ComputeDistance(point1, point2, axis=0):
    """
    Compute the Euclidean distance between two points.

    Args:
        point1 (numpy.ndarray): The first point.
        point2 (numpy.ndarray): The second point.
        axis (int): The axis along which to compute the distance. Defaults to 0.

    Returns:
        float: The Euclidean distance between the two points.
    """

    try:
        distance = np.sqrt(np.sum((point1 - point2) ** 2, axis=axis)) # calculate the Euclidean distance
        return distance
    except ValueError:
        print("Value Error in ComputeDistance: Invalid input. The points must have the same shape.")
    except Exception as e:
        print(f"Error in ComputeDistance: {e}")
        

def initialSelection(data, k):
    """
    Randomly select k points from the data as the initial centroids.
    
    Args:
        data (pandas.DataFrame): The dataset.
        k (int): The number of clusters.
    
    Returns:
        pandas.DataFrame: The initial centroids.
    """
    
    try:
        np.random.seed(SEED)  
        indices = np.random.choice(data.shape[0], size=k, replace=False)   # randomly select k points from the data. replace: not allow the same point to be selected
        return data[indices, :] # row indices and all columns
    except ValueError as e:
        print(f"ValueError in initialSelection: {e}")
    except Exception as e:
        print(f"Error in initialSelection: {e}")


def assignClusterIds(data, centers):
    """
    Assign each point to the nearest centroid.
    
    Args:
        data (pandas.DataFrame): The dataset.
        centers (pandas.DataFrame): The centroids.
    
    Returns:
        cluster_IDs (list): The cluster ID of each point.
    """
    
    try:
        distances = [] # store the distances between each point and each center
        
        for center in centers:
            distance = ComputeDistance(data, center, axis = 1) # calculate the distance between each point and the center
            distances.append(distance)
        
        distances = np.array(distances) # convert the list to a numpy array
        distances = distances.T # transpose the array, so that the shape is (n, k)
        
        cluster_IDs = np.argmin(distances, axis = 1) # get the index of the minimum distance, assign to the cluster ID
        
        return cluster_IDs
    
    except Exception as e:
        print(f"Error in assignClusterIds: {e}")

def computeClusterRepresentatives(data, cluster_IDs, k):
    """
    Compute the new centroids of the clusters.
    
    Args:
        data (pandas.DataFrame): The dataset.
        cluster_IDs (list): The cluster ID of each point.
        k (int): The number of clusters.
        
    Returns:
        new_centers (pandas.DataFrame): The new centroids.
    """
    try:
        new_centers = [] # store the new centroids
        for i in range(k): # for each cluster k
            cluster_points = data[cluster_IDs == i] # all the points in the cluster i
            if len(cluster_points) > 0:
                cluster_means = np.mean(cluster_points, axis = 0) # calculate the mean of the points in the cluster, assign to the new center
                new_centers.append(cluster_means)
        
        if len(new_centers) == 0:
            raise ValueError("ValueError in computeClusterRepresentatives: No points in the clusters.")
        
        new_centers = np.array(new_centers) # convert the list to a numpy array
        return new_centers
    
    except ValueError as e:
        print(f"ValueError in computeClusterRepresentatives: {e}")
    except Exception as e:
        print(f"Error in computeClusterRepresentatives: {e}")
    

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
    
    try:
        centers = initialSelection(data, k)  # randomly select k points from the data as the initial centroids
        for i in range(max_iter):
            cluster_IDs = assignClusterIds(data, centers)  # assign each point to the nearest centroid
            new_centers = computeClusterRepresentatives(data, cluster_IDs, k)  # compute the new centroids of the clusters
            if new_centers is None:
                print("Error: Failed to compute new centroids.")
                return None, None
            if np.array_equal(centers, new_centers):
                print("Convergence reached. Stopping iterations.")
                break
            centers = new_centers  # update the centroids
        else:
            print("Maximum number of iterations reached. Stopping iterations.")
        return cluster_IDs, centers
    except Exception as e:
        print(f"Error in KMeans: {e}")

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
    
    try:
        same_cluster_indices = np.where(cluster_IDs == cluster_IDs[i])[0]  # get the indices of the points in the same cluster
        if len(same_cluster_indices) <= 1:
            return 0
        else: 
            distances = []
            for point in same_cluster_indices: # calculate the distance between the point i and each point in the same cluster
                if point != i:
                    distance = ComputeDistance(data[i], data[point])
                    distances.append(distance)
            mean_distance = np.mean(distances)
            return mean_distance
    except Exception as e:
        print(f"Error in compute_a: {e}")

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
                try:
                    distance = ComputeDistance(data[i], data[point])
                    distances.append(distance)
                except ValueError as e:
                    print(f"ValueError in compute_b: {e}")
                except Exception as e:
                    print(f"Error in compute_b: {e}")
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
        try:
            a_i = compute_a(data, cluster_IDs, i)
            b_i = compute_b(data, cluster_IDs, i)
            if a_i == 0 and b_i == 0: # if a(i) and b(i) are both 0, the silhouette value is 0
                silhouette.append(0) 
            else:
                silhouette.append((b_i - a_i) / max(a_i, b_i)) 
        except Exception as e:
            print(f"Error in calculate_silhouette: {e}")

    return np.mean(silhouette) # return the mean silhouette value


def plot_silhouttee(range_k, silhouette_scores):
    """
    Plot the silhouette score against the number of clusters.
    
    Args:
        range_k (range): The range of the number of clusters.
        silhouette_scores (list): The silhouette scores of the clustering results.
        
    Returns:
        None
    """
    
    try:
        plt.figure(figsize=(10, 6))
        plt.plot(range_k, silhouette_scores, marker='o')
        plt.xlabel('Number of Clusters, k')
        plt.ylabel('Silhouette Score')
        plt.title('Silhouette Score vs. Number of Clusters')
        plt.grid(True)
        # plt.show()
        plt.savefig('KMeansSynthetic.png')
    except Exception as e:
        print(f"Error in plot_silhouttee: {e}")
        
def main():
    # data = load_dataset()
    try:
        data = generate_data()
        range_k = range(2, 10) # actually, k should be larger than 1
        silhouette_scores = []

        for k in range_k:
            cluster_IDs, centers = kmeans(data, k, max_iter = 100)
            silhouette_score = calculate_silhouette(data, cluster_IDs)
            silhouette_scores.append(silhouette_score)

        plot_silhouttee(range_k, silhouette_scores)
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    main()