byte_624082 = 0xAB
byte_624083 = 0xCD
key_xor_dword_624658 = [0xC0, 0xFE, 0x13, 0x37][::-1]
key2 = [0xBE, 0xEF][::-1]

data = [
    0x74, 0x6F, 0x69, 0x35, 0x4F, 0x65, 0x6D, 0x32, 0x32, 0x79, 
    0x42, 0x32, 0x71, 0x55, 0x68, 0x31, 0x6F, 0x5F, 0xDB, 0xCE, 
    0xC9, 0xEF, 0xCE, 0xC9, 0xFE, 0x92, 0x5F, 0x10, 0x27, 0xBC, 
    0x09, 0x0E, 0x17, 0xBA, 0x4D, 0x18, 0x0F, 0xBE, 0xAB, 0x5F, 
    0x9C, 0x8E, 0xA9, 0x89, 0x98, 0x8A, 0x9D, 0x8D, 0xD7, 0xCC, 
    0xDC, 0x8A, 0xA4, 0xCE, 0xDF, 0x8F, 0x81, 0x89, 0x5F, 0x69, 
    0x37, 0x1D, 0x46, 0x46, 0x5F, 0x5E, 0x7D, 0x8A, 0xF3, 0x5F, 
    0x59, 0x01, 0x57, 0x67, 0x06, 0x41, 0x78, 0x01, 0x65, 0x2D, 
    0x7B, 0x0E, 0x57, 0x03, 0x68, 0x5D, 0x07, 0x69, 0x23, 0x55, 
    0x37, 0x60, 0x14, 0x7E, 0x1D, 0x2F, 0x62, 0x5F, 0x62, 0x5F
]

def dec4(arr, start):
    for i in range(30,1, -1):
        arr[i+start-1] ^= arr[i+start-2]

def dec3(arr, start):
    for i in range(0, 5):
        arr[i+start] = ((arr[i+start] >> (8-i)) | (arr[i+start] << (i))) & 0xFF
    arr[start+6] ^= key_xor_dword_624658[0]
    arr[start+7] ^= key_xor_dword_624658[1]
    arr[start+8] ^= key_xor_dword_624658[2]
    arr[start+9] ^= key_xor_dword_624658[3]

def reverse_transform_data(arr, start):
    for i in range(start, start+8):
        arr[i] ^= byte_624082
    start += 9
    for i in range(0, 12):
        x = (byte_624083 + i) & 0xFF
        arr[i+start] ^= x
        arr[i+start] >>= 1

def dec2(arr, start):
    reverse_transform_data(arr, start)
    start += 9+13
    for i in range(0, 9):
        arr[2*i + start] ^= key2[0]
        arr[2*i + start + 1] ^= key2[1]
def dec1(arr, start):
    for i in range(start, start+17):
        arr[i] ^= 1

dec4(data, 18+13+9+19+11)
dec3(data, 18+13+9+19)
dec2(data, 18)
dec1(data, 0)
inp = ''.join(chr(i) for i in data)
print(inp)
inp_hex = ' '.join(hex(i)[2:] for i in data)
print(inp_hex)

# unh4Ndl33xC3pTi0n_pebDebU9_nt9lob4Lfl49_s0F7w4r38r34Kp01n7_int2d_int3_YXV0aG9ydHVuYTk5ZnJvbWtjc2M===
# 75 6e 68 34 4e 64 6c 33 33 78 43 33 70 54 69 30 6e 5f 70 65 62 44 65 62 55 39 5f 6e 74 39 6c 6f 62 34 4c 66 6c 34 39 5f 73 30 46 37 77 34 72 33 38 72 33 34 4b 70 30 31 6e 37 5f 69 6e 74 32 64 5f 69 6e 74 33 5f 59 58 56 30 61 47 39 79 64 48 56 75 59 54 6b 35 5a 6e 4a 76 62 57 74 6a 63 32 4d 3d 3d 3d