def rc4_init(key: bytes) -> list:
    S = list(range(256))
    j = 0
    for i in range(256):
        j = (j + S[i] + key[i % len(key)]) % 256
        S[i], S[j] = S[j], S[i]
    return S

def rc4_crypt(data: bytes, key: bytes) -> bytes:
    S = rc4_init(key)
    i = j = 0
    result = bytearray()
    for byte in data:
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        t = (S[i] + S[j]) % 256
        k = S[t]
        result.append(byte ^ k)
    return bytes(result)

key = bytes([0xde, 0xad, 0xbf, 0x33][::-1])
plaintext = bytes([0x7D, 0x08, 0xED, 0x47, 0xE5, 0x00, 0x88, 0x3A,
                   0x7A, 0x36, 0x02, 0x29, 0xE4])

plaintext = rc4_crypt(plaintext, key)
print("Plaintext (hex):", plaintext.hex(' '))
print("Plaintext (raw):", plaintext)

# Plaintext (hex): 44 31 74 5f 6d 33 5f 48 34 5f 4e 34 31
# Plaintext (raw): b'D1t_m3_H4_N41'