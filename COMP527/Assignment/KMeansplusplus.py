
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

SEED = 47


def load_dataset(filename='dataset'):
    """
    Load a dataset from a file, with comprehensive error handling.

    Args:
        filename (str): The name of the file to load the dataset from. Defaults to 'dataset'.

    Returns:
        data (numpy.ndarray): The dataset as a numpy array, or None if loading fails.
    """
    try:
        script_dir = os.path.dirname(os.path.realpath(__file__))  # get the directory of the script
        filepath = os.path.join(script_dir, filename)  # construct the file path
        
        # Check if the file is accessible
        if not os.access(filepath, os.R_OK):
            raise PermissionError("The file is not accessible due to permission restrictions.")
        
        # Attempt to read the file
        data = pd.read_csv(filepath, sep=' ', header=None)  # split data by space, no header
        
        # Check for proper data format and integrity
        if data.empty:
            raise ValueError("ValueError in load_dataset: The file is empty or contains only headers.")
        
        if data.shape[1] < 2:  # Assuming we expect at least 2 columns
            raise ValueError("ValueError in load_dataset: The file does not contain enough columns.")
        
        if data.duplicated(0).any():
            raise ValueError("ValueError in load_dataset: Duplicate entries found in the index column.")
        
        data.set_index(0, inplace=True)  # set the first column as the index
        return data.values  # return as a numpy array

    except FileNotFoundError:
        print(f"FileNotFoundError in load_dataset: The file '{filename}' does not exist.")
    except PermissionError as e:
        print(f"PermissionError in load_dataset: {e}")
    except pd.errors.EmptyDataError:
        print("EmptyDataError in load_dataset: The file is empty or corrupted.")
    except pd.errors.ParserError:
        print("ParserError in load_dataset: The file is corrupted and cannot be parsed.")
    except ValueError as e:
        print(f"ValueError in load_dataset: {e}")
    except Exception as e:
        print(f"Error in load_dataset: {e}")


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
    Randomly select k points from the data as the initial centroids using the k-means++ algorithm, 
    which is a variation of the k-means algorithm.
    
    Args:
        data (pandas.DataFrame): The dataset.
        k (int): The number of clusters.
        
    Returns:
        centers (pandas.DataFrame): The initial centroids.
    """
    
    n = data.shape[0] # get the number of points
    first_center_index = np.random.choice(n) # randomly select the first centroid
    centers = [data[first_center_index]] # store the centroids

    distances = ComputeDistance(data, data[first_center_index], axis=1) # calculate the distance between each point and the first centroid

    for _ in range(1, k):
        probabilities = distances ** 2 # calculate the probability of each point being selected as the next centroid
        probabilities /= probabilities.sum() # normalize the probabilities, so that they sum up to 1
        next_center_index = np.random.choice(n, p=probabilities) # randomly select the next centroid based on the probabilities
        centers.append(data[next_center_index])

        new_distances = ComputeDistance(data, data[next_center_index], axis=1) # calculate the distance between each point and the new centroid
        distances = np.minimum(distances, new_distances) # get the minimum distance between the previous distance and the new distance

    return np.array(centers)  # Return centroids as a numpy array.



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
            if len(cluster_points) == 0:
                new_centers.append(data[np.random.choice(data.shape[0])])
            else:
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
    

def kmeansplusplus(data, k, max_iter = 100):
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
        centers = initialSelection(data, k) # randomly select k points from the data as the initial centroids
        for i in range(max_iter):
            cluster_IDs = assignClusterIds(data, centers) # assign each point to the nearest centroid
            new_centers = computeClusterRepresentatives(data, cluster_IDs, k) # compute the new centroids of the clusters
            if np.array_equal(centers, new_centers): # check if the centroids have converged
                break
            centers = new_centers # update the centroids
        else:
            print("Warning: The maximum number of iterations has been reached without convergence.")
        return cluster_IDs, centers
    except Exception as e:
        print(f"Error in kmeansplusplus: {e}")
        return None, None


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
        current_dir = os.path.dirname(os.path.realpath(__file__))  # get the directory of the script
        plt.savefig(os.path.join(current_dir, 'KMeansplusplus.png'))  # save the plot as a PNG file
    except Exception as e:
        print(f"Error in plot_silhouttee: {e}")

def main():
    data = load_dataset()
    if data is None:
        print("Failed to load data. Exiting.")
        return

    range_k = range(2, 10)  # It's good that k starts from 2 as k-means doesn't work with k=1
    silhouette_scores = []

    for k in range_k:
        cluster_IDs, centers = kmeansplusplus(data, k, max_iter=100)
        if cluster_IDs is None or centers is None:
            print(f"Failed to perform k-means++ for k={k}. Skipping.")
            continue

        silhouette_score = calculate_silhouette(data, cluster_IDs)
        if silhouette_score is not None:
            silhouette_scores.append(silhouette_score)
        else:
            print(f"Failed to calculate silhouette score for k={k}.")

    plot_silhouttee(range_k, silhouette_scores)

if __name__ == '__main__':
    main()