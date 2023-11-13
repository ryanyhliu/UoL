import panda
plot = panda.BambooPlot(
    'Fibonacci',
    [21, 13, 8, 5, 3, 2, 1, 1]
)

# Don't change anything above this line
# ===================================

# generate your solution as a list of indices
# queue = [0, 1, 0]

import pandaYHCode
queueYH = pandaYHCode.Solution(
    plot.name,
    plot.growthRates
)
queue = queueYH.regularQueue()
print("Answer: ")
print(queue)
# queue = [0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4]

# ====================================
# Dont change anything below this line

# records your solution
plot.simulate(queue)
