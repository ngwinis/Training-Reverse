cipher = [0x6ab4a0c7, 0x72b7b7d4, 0x349da1da, 0x3a94ecc4, 0xe5700a45, 0x38b2b080, 0x6694a3b3, 0x179f8ba3]
key_tlscallback = 0x81000700 ^ 0xEFBEADDE
key_case0 = 0x656E6F67
key_case1 = 0x776F6978
key_case2 = 0x574F4958
key_case3 = 0x68617264
key_case5 = 0x21626570
key_case6 = 0x6C6C756E
key_case6_2 = 0x65757274
key_case7 = 0xFFFFFFFF

for i in range(len(cipher)):
    cipher[i] ^= key_tlscallback    # 7 b'y!!}'
    # cipher[i] ^= key_case0          # 5 b'3bu9'
    # cipher[i] ^= key_case1          # 0 b'seca'
    # cipher[i] ^= key_case2          # 6 b'_e@5'
    # cipher[i] ^= key_case3          # 1 b'thon'
    # cipher[i] ^= key_case5          # 2 b'{Ant'
    # cipher[i] ^= key_case6
    # cipher[i] ^= key_case6_2        # 3 b'1_4n'
    # cipher[i] ^= key_case7          # 4 b't1_d'
    print(i, cipher[i].to_bytes(4, 'big'))

# flag: seca thon {Ant 1_4n ti_d 3bu9 _e@5 y!!}
# Case:  1    3    5   6_2   7    0    2    -1