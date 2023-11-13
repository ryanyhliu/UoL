import panda
plot = panda.BambooPlot(
    'Odds',
    [9, 7, 7, 5, 5, 3, 3]
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
# queue = [0, 1, 3, 5, 0, 2, 4, 1, 0, 6, 3, 2, 0, 1, 4, 5, 0, 2, 3, 1, 0, 6, 4, 2]
print("Answer: ")
print(queue)

# ====================================
# Dont change anything below this line

# records your solution
plot.simulate(queue)
