def build_suffix_array(s):
    suffixes = [(s[i:], i) for i in range(len(s))]
    suffixes.sort(key=lambda x: x[0])
    return [suffix[1] for suffix in suffixes]

def build_lcp_array(s, suffix_array):
    n = len(s)
    lcp = [0] * n
    inv_suffix = [0] * n

    for i in range(n):
        inv_suffix[suffix_array[i]] = i

    k = 0
    for i in range(n):
        if inv_suffix[i] == n - 1:
            k = 0
            continue
        j = suffix_array[inv_suffix[i] + 1]
        while i + k < n and j + k < n and s[i + k] == s[j + k]:
            k += 1
        lcp[inv_suffix[i]] = k
        if k > 0:
            k -= 1

    return lcp[:-1]

T = "dbcbbbaaacbcbbba$"
# badaaddbbdaddbbd$
# abbbbbbbdadbcada$
# cbcdbbabdbabdbaa$

suffix_array = build_suffix_array(T)
lcp_array = build_lcp_array(T, suffix_array)
result = ','.join(map(str, lcp_array))

print(result)
