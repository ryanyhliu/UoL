import panda
plot = panda.BambooPlot(
    'Power',
    [16, 8, 4, 2, 1, 1]
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
