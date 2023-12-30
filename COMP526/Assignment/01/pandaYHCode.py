import collections
import math
from typing import List
import config
import json



class Solution:
    # Number of simulated days (length of returned list)
    global DAYS
    DAYS = 1000000
    
    def __init__(self, name, growthRates) -> None:
        self.name = name
        self.growthRates = growthRates
    
    def originQueue(self):
        
        # Sort in descending order without modifying the original list
        growthRatesCopy = self.growthRates.copy()
        growthRatesCopy.sort(reverse = True)
        
            
        # Maximum growth rate * longest interval days >= harvest >= maximum growth rate * 1 day
        # Pay attention to the right boundary -1, or you can't get the value (it took me 40 minutes to Debug the lesson)
        for harvest in range(growthRatesCopy[0] * len(growthRatesCopy), growthRatesCopy[0] * 1 - 1, -1):
            cuts:List[int] = [] # List of tree periods to cut
            
            # Initialization (expected harvest < growth rate of all bamboos * days)
            # The harvest value is satisfied, at least days between harvests
            intervalDays = [0 for index in range(0, len(growthRatesCopy))]
            # Current growth days
            currentDays = [0 for index in range(0, len(growthRatesCopy))]
            
            for index in range(len(growthRatesCopy) - 1, 0 - 1, -1):
                # Store current bamboo to meet harvest for at least a few days              
                currentDays[index] = intervalDays[index] = math.ceil(harvest / growthRatesCopy[index]) # round up to an integer
                currentDays[index] = len(growthRatesCopy) - index # The first round index is the number of days
                
            # Start calculating whether you can reach the harvest value (whether you can find bamboo that meets the conditions to cut down)
            # The first round is cut from beginning to end, and the initial length is positive infinity, which is not counted
            for index in range(len(growthRatesCopy) - 1, 0 - 1, -1):
                cuts.append(index)
            
            
            for day in range(len(growthRatesCopy), DAYS):
                # If the condition is satisfied (the bamboo was cut), otherwise terminate the current loop
                isCut = False
                
                # Starting from the largest (mainly to see if the minimum can meet the condition)
                differenceList = [0 for index in range(0, len(growthRatesCopy))]
                for index in range(0, len(growthRatesCopy)):
                    if currentDays[index] >= intervalDays[index]:
                        # Meet the cut, add this bamboo to the return list, height clear zero
                        cuts.append(index)
                        currentDays[index] = 0
                        isCut = True
                        break
                
                # Out of the loop, check that there is no bamboo to cut, and add all the days +1
                if isCut == False:
                    break
                
                for index in range(0, len(growthRatesCopy)):
                    if currentDays[index] < intervalDays[index]:
                        currentDays[index] += 1
            
            if isCut == True:
                print("Cuts array")
                print(cuts)
                print("Origin array: " + str(self.growthRates))
                print("Expected harvest: " + str(harvest))
                
                return cuts
    
    
    def regularQueue(self) -> str:
        # 统计最后NUM个数字, 出现最少的那个, 就是regularQueue的末尾, 从它往后数, 再次遇见它, 即为regularQueue
        
        originQueue = self.originQueue()
        
        # Find the longest repeated subsequence in a list
        # Use a sliding window method similar to finding the shortest substring
        
        lst = originQueue[-1000:]
        
        
        
        # Initialize the left and right pointers
        left = 0
        right = 0
        # Initialize the count of elements that appear at least twice in the window
        count = 0
        # Initialize the result list
        res = []
        # Use a dictionary to record the frequency of each element in the list
        freq = {}
        for x in lst:
            freq[x] = freq.get(x, 0) + 1
        # Use another dictionary to record the frequency of each element in the window
        window = {}
        # Loop until the right pointer reaches the end of the list
        while right < len(lst):
            # Add the current element to the window and update its frequency
            x = lst[right]
            window[x] = window.get(x, 0) + 1
            # If the current element appears at least twice in the window, increase the count by 1
            if window[x] == 2:
                count += 1
            # Move the right pointer to the next position
            right += 1
            # While the window contains all the elements that appear at least twice in the list
            while count == len([x for x in freq if freq[x] >= 2]):
                # Update the result list if the current window is longer or empty
                if len(res) == 0 or right - left > len(res):
                    res = lst[left:right]
                # Remove the current element from the window and update its frequency
                y = lst[left]
                window[y] -= 1
                # If the current element appears less than twice in the window, decrease the count by 1
                if window[y] < 2:
                    count -= 1
                # Move the left pointer to the next position
                left += 1
        # Return the result list
        return res