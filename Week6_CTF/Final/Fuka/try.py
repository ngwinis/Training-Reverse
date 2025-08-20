# Chuỗi byte ví dụ
b = bytes([0x4A, 0x55, 0x0A, 0x11, 0x13, 0x0E, 0xD2, 0xEC, 0x03, 0x47, 0x52, 0x47, 0x92, 0x8B, 0x89, 0x8E, 0xCA, 0xAF, 0x98, 0x55])

# Diễn giải không dấu
num_unsigned = int.from_bytes(b, byteorder='big', signed=False)

# Diễn giải có dấu (two's complement)
num_signed = int.from_bytes(b, byteorder='big', signed=True)

print("Unsigned:", num_unsigned)
print("Signed:", num_signed)
