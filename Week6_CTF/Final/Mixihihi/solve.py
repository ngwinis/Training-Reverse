flagEnc = [143, 124, 41, 217, 251, 185, 171, 26, 74, 83, 173, 236, 169, 239, 22, 115, 128, 248, 92, 9]

for i in flagEnc:
    print(f"{i:02x}".upper(), end=" ")
print()
flag = '4B4353437B68334C6C305F6652304D5F63232329'
print('flag: ', end='')
for i in bytes.fromhex(flag):
    print(chr(i), end='')

# flag: KCSC{h3Ll0_fR0M_c##)