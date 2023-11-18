// 初始化距离矩阵
    double **dist = (double **)malloc(numOfCoords * sizeof(double *));
    for (int i = 0; i < numOfCoords; i++) {
        dist[i] = (double *)malloc(numOfCoords * sizeof(double));
    }
    calculateDist(inputs, dist, numOfCoords);

	// 初始化序列，假设起始点为0
	int *resultSeq = (int *)malloc((numOfCoords + 2) * sizeof(int)); // +2 for start and end
	resultSeq[0] = 0; // 起始点
	for (int i = 1; i <= numOfCoords; i++) {
		resultSeq[i] = -1; // 初始化为-1
	}
	resultSeq[numOfCoords + 1] = 0; // 结束点，也为0
	int currentSeqLen = 1; // 当前序列长度，初始包含起始点

    for (int i = 0; i < numOfCoords; i++) {
    InsertionTask task = findCheapestInsertion(resultSeq, dist, numOfCoords, currentSeqLen);
    if (task.insertPoint != -1) {
        insertPoint(resultSeq, currentSeqLen, task);
        currentSeqLen++;
    }