try:
        new_centers = [] # store the new centroids
        for i in range(k): # for each cluster k
            cluster_points = data[cluster_IDs == i] # all the points in the cluster i
            if len(cluster_points) > 0:
                cluster_means = np.mean(cluster_points, axis = 0) # calculate the mean of the points in the cluster, assign to the new center
                new_centers.append(cluster_means)
            else:
                raise ValueError(f"ValueError in computeClusterRepresentatives: Cluster {i} is empty.")
        
        new_centers = np.array(new_centers) # convert the list to a numpy array
        return new_centers
    
    except ValueError as e:
        print(e)
    except Exception as e:
        print(f"Error in computeClusterRepresentatives: {e}")