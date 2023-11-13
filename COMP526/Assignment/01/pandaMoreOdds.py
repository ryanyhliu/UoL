import panda
plot = panda.BambooPlot(
    'MoreOdds',
    [9, 7, 7, 5, 5, 3, 3, 3]
)

# Don't change anything above this line
# ===================================

# generate your solution as a list of indices
# queue = [0, 1]

import pandaYHCode
queueYH = pandaYHCode.Solution(
    plot.name,
    plot.growthRates
)
queue = queueYH.regularQueue()
print("Answer: ")
print(queue)

# ====================================
# Dont change anything below this line

# records your solution
plot.simulate(queue)
