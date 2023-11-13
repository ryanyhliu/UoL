import panda
plot = panda.BambooPlot(
    'YHCase',
    [10, 9, 6, 5, 4, 3, 2, 1, 1, 1]
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
# queue =  queueYH.originQueue()
# queue = [0, 1, 2, 0, 5, 1, 0, 2, 3, 0, 1, 4, 0, 2, 1, 0, 3, 5, 0, 1, 2, 0, 3, 1, 0, 2, 4, 0, 1, 3, 0, 2, 1, 0, 4, 3]
print("Answer: ")
print(queue)

# ====================================
# Dont change anything below this line

# records your solution
plot.simulate(queue)
