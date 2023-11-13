# Simulates taking the exam; it computes the worst possible grade when
# using the protocol implemented in examCheatingCode

import examCheatingCode
import itertools
import random


def sit_exams(n_questions = 20, n_honks = 10):
    # generate all exams
    exams = []
    for exam in itertools.product([0, 1], repeat=20):
        exams.append(exam)
    # randomize order, so next exam is not predictable
    random.shuffle(exams)

    worst_mark = n_questions
    exams_to_check = len(exams)
    print("{} exams still to check".format(exams_to_check))
    for exam in exams:
        # encode copy of exam solutions in 10 bit
        copy = []
        copy[:] = exam
        code = examCheatingCode.compute_and_send_code(copy)
        if len(code) != n_honks or any(map(lambda b: b != 0 and b != 1, code)):
            raise Exception("Illegal code!")
        # compute answers to the exam from the 10 bit
        answers = examCheatingCode.enter_solution_based_on_code(code)
        if len(answers) != n_questions or any(map(lambda b: b != 0 and b != 1, answers)):
            raise Exception("Illegal decoded exam answers!")
        mark = 0
        for i in range(n_questions):
            if answers[i] == exam[i]:
                mark += 1
        if mark < worst_mark:
            worst_mark = mark
        # progress report
        if exams_to_check % 10000 == 0:
            print("{} exams still to check".format(exams_to_check))
        exams_to_check -= 1
    return worst_mark


my_worst_mark = sit_exams()
# Print result
print("The worst achieved mark is: " + str(my_worst_mark))


