def gen_const(a1: int, a2: bytearray, a3: int) -> int:
    v4 = a3 - 1
    v18 = 171
    v6 = 0

    while v18:
        # Part 1: update v7 and mutate a2
        if v4 <= 5:
            offset = 4 * v4 + 16
            if int.from_bytes(a2[offset:offset + 4], 'little') != 0:
                v8 = int.from_bytes(a2[offset:offset + 2], 'little')
            else:
                v8 = int.from_bytes(a2[4 * v4:4 * v4 + 2], 'little')
            v7 = (v8 >> 1) | (((32 * v8) ^ (v8 ^ (4 * (v8 ^ (2 * v8)))) & 0xFFE0) << 10)
            a2[offset:offset + 4] = (v7 & 0xFFFFFFFF).to_bytes(4, 'little')
        else:
            v7 = 0

        v9 = v7 & 0x7FF
        v10 = v7 & 7
        v11 = v9 >> 3

        if a1:
            v12 = a2[v11 + 44]
        else:
            v12 = (~a2[v11 + 44]) & 0xFF

        a2[v11 + 44] = v12 ^ (1 << v10)

        v18 -= 1

    # Part 2: checksum across bytes
    v6 = 0
    for i in range(64):
        base = 46 + 4 * i
        v16 = a2[base - 2]
        v6 ^= a2[base] ^ a2[base + 1] ^ a2[base - 1] ^ v16
    return v6 & 0xFF


index_table = [9, 18, 15, 3, 4, 23, 6, 7, 8, 22, 10, 11, 33, 13, 14, 27, 16, 37, 17, 19, 20, 21, 5, 34, 24, 25, 26, 2, 12, 29, 30, 31, 32, 28, 0, 35, 36, 1]
g_const = [1, 3, 1, 1, 2, 1, 3, 1, 2, 2, 4, 4, 1, 3, 4, 4, 4, 1, 2, 1, 4, 1, 4, 3, 1, 2, 4, 4, 2, 2, 1, 3, 4, 2, 1, 2, 2, 3]
cipher = [14, 235, 243, 246, 209, 107, 167, 143, 61, 145, 133, 43, 134, 167, 107, 219, 123, 110, 137, 137, 24, 149, 103, 202, 95, 226, 84, 14, 211, 62, 32, 90, 126, 212, 184, 16, 194, 183]
program = [6, 1, 7, 1, 3, 2, 4, 3, 6, 3, 7, 6, 1, 4, 7, 4, 1, 5, 7, 6, 7, 5, 6, 4, 5, 1, 7, 5, 2, 3, 1, 2, 3, 2, 1, 6, 2, 4]

# Build default_const accurately (length 302+ maybe)
default_const_list = [0]*400
default_const_list[0:16] = [54,236,0,0,54,237,0,0,54,187,0,0,54,140,0,0]
default_const_list[32] = 95
default_const = bytearray(default_const_list)

key = bytearray(40)

for i in range(38):
    g = g_const[i]
    p = program[i]
    if p in (1, 4, 5):
        tmp = gen_const(0, default_const, g)
    elif p in (2, 3, 6, 7):
        tmp = gen_const(1, default_const, g)
    else:
        continue
    key[index_table[i]] = cipher[i] ^ tmp

print(key.decode('latin1', errors='replace'))

# I_10v3-y0U__wh3n Y0u=c411..M3 Senor1t4