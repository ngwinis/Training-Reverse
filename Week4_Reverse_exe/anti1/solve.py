
def dec(cipher, key):
    i = 0
    plain = ''
    for x in cipher:
        p = chr(x ^ ord(key[i%len(key)]))
        plain += p
        i += 1
    return plain

cipher = [
  0, 0, 0, 0, 6, 56, 38, 119, 48, 88, 126, 66, 42, 127, 63, 41, 26, 33, 54, 55, 28, 85, 73, 18, 48, 120, 12, 40, 48, 48, 55, 28, 33, 18, 126, 82, 45, 38, 96, 26, 36, 45, 55, 114, 28, 69, 68, 67, 55, 44, 108, 122, 56
]
key = 'BKSEECCCC!!!'

plaintext = dec(cipher, key)
print(plaintext)

# BKSEC{e4sy_ch4ll_but_th3r3_must_b3_som3_ant1_debug??}