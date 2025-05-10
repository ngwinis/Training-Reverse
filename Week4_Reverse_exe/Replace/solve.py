def decrypt_tea(encrypted_buffer, key):
    import struct
    result = []
    
    k = []
    for i in range(0, len(key), 4):
        k_bytes = key[i:i+4].ljust(4, b'\0')
        k_val, = struct.unpack("<I", k_bytes)
        k.append(k_val)
    
    for j in range(0, len(encrypted_buffer), 8):
        if j + 8 <= len(encrypted_buffer):
            chunk = bytes(encrypted_buffer[j:j+8])
            v0, v1 = struct.unpack("<II", chunk)
            
            delta = 0x9E3779B9
            sum_val = (delta * 32) & 0xFFFFFFFF
            
            for i in range(32):
                v1 = (v1 - ((k[3] + (v0 >> 5)) ^ (sum_val + v0) ^ (k[2] + (v0 << 4)))) & 0xFFFFFFFF
                v0 = (v0 - ((k[1] + (v1 >> 5)) ^ (sum_val + v1) ^ (k[0] + (v1 << 4)))) & 0xFFFFFFFF
                sum_val = (sum_val - delta) & 0xFFFFFFFF
            
            result.extend(struct.pack("<II", v0, v1))
    
    return result

Buf2 = [
    0x19, 0x2C, 0x30, 0x2A, 0x79, 0xF9, 0x54, 0x02, 0xB3, 0xA9, 
    0x6C, 0xD6, 0x91, 0x80, 0x95, 0x04, 0x29, 0x59, 0xE8, 0xA3, 
    0x0F, 0x79, 0xBD, 0x86, 0xAF, 0x05, 0x13, 0x6C, 0xFE, 0x75, 
    0xDB, 0x2B, 0xAE, 0xE0, 0xF0, 0x5D, 0x88, 0x4B, 0x86, 0x89, 
    0x33, 0x66, 0xAC, 0x45, 0x9A, 0x6C, 0x78, 0xA6
]

key = b"VdlKe9upfBFkkO0L"

decrypted = decrypt_tea(Buf2, key)

ascii_result = ""
for b in decrypted:
    if 32 <= b <= 126:
        ascii_result += chr(b)
    else:
        ascii_result += "."

print("Decrypted text (ASCII):", ascii_result)

c_string = bytes(decrypted).decode('utf-8', errors='replace')
print(c_string)
# Decrypted text (ASCII): PTITCTF{bdc90e23aa0415e94d0ac46a938efcf3}.......
# PTITCTF{bdc90e23aa0415e94d0ac46a938efcf3}





