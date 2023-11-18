
FarthestInsertionTask findFarthestInsertion(int *seq, double **dist, int numOfCoords, int seqValidLen) {
    FarthestInsertionTask globalBestTask = {-1, -1, 0.0};

    #pragma omp parallel
    {
        FarthestInsertionTask localBestTask = {-1, -1, 0.0};

        #pragma omp for nowait
        for (int tempPoint = 0; tempPoint < numOfCoords; tempPoint++) {
            bool alreadyInSeq = false;
            for (int k = 0; k < seqValidLen; k++) {
                if (seq[k] == tempPoint) {
                    alreadyInSeq = true;
                    break;
                }
            }

            if (!alreadyInSeq) {
                double currentMaxDist = 0.0;
                int bestInsertIndex = -1;
                for (int i = 0; i < seqValidLen; i++) {
                    if (seq[i] != -1) {
                        double distToSeqPoint = dist[seq[i]][tempPoint];
                        if (distToSeqPoint > currentMaxDist) {
                            currentMaxDist = distToSeqPoint;

                            // Find best insertion position for the farthest point
                            double minInsertionCost = DBL_MAX;
                            for (int j = 0; j < seqValidLen - 1; j++) {
                                double insertionCost = dist[seq[j]][tempPoint] + dist[tempPoint][seq[j + 1]] - dist[seq[j]][seq[j + 1]];
                                if (insertionCost < minInsertionCost) {
                                    minInsertionCost = insertionCost;
                                    bestInsertIndex = j + 1;
                                }
                            }

                            if (currentMaxDist > localBestTask.distance) {
                                localBestTask = (FarthestInsertionTask){tempPoint, bestInsertIndex, currentMaxDist};
                            }
                        }
                    }
                }
            }
        }

        #pragma omp critical
        {
            if (localBestTask.distance > globalBestTask.distance) {
                globalBestTask = localBestTask;
            }
        }
    }

    return globalBestTask;
}






void insertPoint(int *seq, int seqLen, FarthestInsertionTask task) {
    if (task.insertIndex < seqLen + 1) {
        #pragma omp parallel for
        for (int i = seqLen; i >= task.insertIndex; i--) {
            seq[i + 1] = seq[i];
        }
        seq[task.insertIndex] = task.farthestPoint;
    }
}