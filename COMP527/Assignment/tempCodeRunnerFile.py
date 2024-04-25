try:
        data = load_dataset()
        range_k = range(1, 10) # actually, k should be larger than 1
        silhouette_scores = []

        for k in range_k:
            cluster_IDs, centers = kmeansplusplus(data, k, max_iter = 100)
            silhouette_score = calculate_silhouette(data, cluster_IDs)
            silhouette_scores.append(silhouette_score)

        plot_silhouttee(range_k, silhouette_scores)
    except Exception as e:
        print(f"Error in main: {e}")