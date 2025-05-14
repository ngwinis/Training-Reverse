import sys

# Initial byte_B4118 data from the problem
byte_B4118_hex = "746F69354F656D3232794232715568316F5FDBCEC9EFCEC9FE925F1027BC090E17BA4D180FBEAB5F9C8EA989988A9D8DD7CCDCDCA4CEDF8F81895F69371D46465F5E7D8AF35F5901576706417801652D7B0E5703685D076923553760147E1D2F625F625F"
data = bytearray.fromhex(byte_B4118_hex)

key2 = 0xBEEF # From check_breakpoint, used in sub_B1460 word XOR
xor_key_dword_B4658 = 0xC0FE1337 # From sub_B11D0 exception handler

# Helper functions for bitwise rotations
def ROL_byte(val, n):
    n %= 8
    return ((val << n) | (val >> (8 - n))) & 0xFF

def ROR_byte(val, n):
    n %= 8
    return ((val >> n) | (val << (8 - n))) & 0xFF

# --- Reversing Functions ---

def reverse_running_xor_sub_B1190(current_data):
    temp_data = list(current_data) # Make a mutable copy
    for i in range(1, 30): # i from 1 to 29
        idx = 70 + i
        temp_data[idx] = temp_data[idx] ^ temp_data[idx-1]
    return bytearray(temp_data)

def reverse_dword_B4658_xor_sub_B11D0(current_data):
    temp_data = list(current_data)
    temp_data[24] ^= (xor_key_dword_B4658 >> 0) & 0xFF 
    temp_data[25] ^= (xor_key_dword_B4658 >> 8) & 0xFF 
    temp_data[26] ^= (xor_key_dword_B4658 >> 16) & 0xFF
    temp_data[27] ^= (xor_key_dword_B4658 >> 24) & 0xFF
    return bytearray(temp_data)

def reverse_rotations_sub_B11D0(current_data):
    temp_data = list(current_data)
    temp_data[59] = ROR_byte(temp_data[59], 1)
    temp_data[61] = ROL_byte(temp_data[61], 1)
    temp_data[62] = ROL_byte(temp_data[62], 2)
    temp_data[63] = ROL_byte(temp_data[63], 3)
    return bytearray(temp_data)

def reverse_word_xor_sub_B1460(current_data):
    temp_data = list(current_data)
    for i in range(9):
        offset = 40 + (2 * i)
        word_val = (temp_data[offset+1] << 8) | temp_data[offset]
        word_val ^= key2
        temp_data[offset] = word_val & 0xFF
        temp_data[offset+1] = (word_val >> 8) & 0xFF
    return bytearray(temp_data)

byte_B4082 = 0xAB 
byte_B4083 = 0x0C

def reverse_transform_data(current_data, offset_in_data):
    temp_data_segment = list(current_data)

    start_loop2 = offset_in_data + 9
    for i in range(12):
        idx = start_loop2 + i
        key_loop2 = (byte_B4083 + i) & 0xFF
        val_after_xor = temp_data_segment[idx] ^ key_loop2

        if (val_after_xor & 1) == 0:
            print(f"CẢNH BÁO: Đảo ngược vòng lặp 2, byte {idx}: val_after_xor {val_after_xor} không lẻ!")
            original_val_times_2 = val_after_xor # Sẽ sai nếu val_after_xor chẵn
        else:
            original_val_times_2 = val_after_xor -1 # Hoặc val_after_xor & 0xFE

        original_val = (original_val_times_2 // 2) & 0xFF
        temp_data_segment[idx] = original_val
    start_loop1 = offset_in_data
    for i in range(8):
        idx = start_loop1 + i
        temp_data_segment[idx] = temp_data_segment[idx] ^ byte_B4082
        
    return bytearray(temp_data_segment)

def reverse_initial_xor_by_1_sub_B1547(current_data):
    temp_data = list(current_data)
    for i in range(17): # Indices 0 to 16
        temp_data[i] = temp_data[i] ^ 1
    return bytearray(temp_data)

data = reverse_running_xor_sub_B1190(data)

data = reverse_dword_B4658_xor_sub_B11D0(data)
data = reverse_rotations_sub_B11D0(data)

data = reverse_word_xor_sub_B1460(data)
data = reverse_transform_data(data, 28)

flag_bytes = reverse_initial_xor_by_1_sub_B1547(data)

try:
    flag_string = flag_bytes.decode('ascii')
    print(f"Recovered flag: {flag_string}")
except UnicodeDecodeError:
    print(f"Could not decode flag as ASCII. Raw bytes (hex): {flag_bytes.hex()}")
    print(f"Decoded with replacements: {flag_bytes.decode('ascii', errors='replace')}")