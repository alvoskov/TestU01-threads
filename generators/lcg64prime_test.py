a = 13891176665706064842
m = 2**64 - 59

def lcg64prime(a, m, x):
    x = (a * x) % m
    return x

# https://en.wikipedia.org/wiki/Linear_congruential_generator
def lcg64prime_tr(a, m, x):
    hi = (a * x) // 2**64
    lo = (a * x) % 2**64
    r = lo + 59 * hi

    k = (r >> 64) - 1
    if k > 0 and k < 100:
        r -= k * 2**64 - k * 59

    #print(hi, lo, r, r >> 64)

    while r >= 2**64 - 59:
        #print('-')
        r -= 2**64 - 59
    return r
    

x = 1
for i in range(0,100000):
    x = lcg64prime(a, m, x)

x1 = 1
for i in range(0,100000):
    x1 = lcg64prime_tr(a, m, x1)

print('a = ', a)
print('m = ', m)
print('x = ', x)
print('x1 = ', x1)
