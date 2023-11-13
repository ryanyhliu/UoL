# Compute the honking protocol for the exam cheaters

def compute_and_send_code(exam):
    code = [0] * 10
    # Don't change anything above this line
    # ==========================

    # TODO Add your solution here.
    code.clear()
    
    # 前5位直接传
    for index in range(5):
        code.append(exam[index])
    
    # 后15位3个一组, 取较多的, 例如[0, 1, 0]取[0, 0, 0]
    for index in range(5, len(exam), 3):
        # 3个一组, 分成5组, 每组用一个数字表示, 1代表111, 0代表000
        # codeCompress = [0] * 5
        
        sum = 0
        for t_index in range(index, index + 3):
            sum += exam[t_index]
        if sum <= 1:
            # <= 1, average of 3 nums = 0
            code.append(0)
        else:
            # >= 2, average = 1
            code.append(1)

    # ==========================
    # Don't change anything below this line
    return code


def enter_solution_based_on_code(code):
    answer = [0] * 20
    # Don't change anything above this line
    # ==========================

    # TODO Add your solution here.
    answer.clear()
    
    # 前5位直接解码
    for index in range(5):
        answer.append(code[index])
    
    # 后5位, 直接把平均值填充的容量为3的子数组
    for index in range(5, len(code)):
        if code[index] == 0:
            answer.extend([0, 0, 0])
        else:
            # 1 -> [1, 1, 1]
            answer.extend([1, 1, 1])
    

    # ==========================
    # Don't change anything below this line
    return answer



