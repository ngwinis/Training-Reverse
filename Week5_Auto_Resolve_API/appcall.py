import idaapi
import ida_ida
import idautils
import idc
import ida_name

def is_mov_reg_imm(ea, reg):
    if idc.print_insn_mnem(ea) != "mov":
        return False
    return idc.print_operand(ea, 0) == reg and idc.get_operand_type(ea, 1) == idc.o_imm

def get_imm_val(ea):
    return idc.get_operand_value(ea, 1)

def is_call_to(ea, target_name):
    if idc.print_insn_mnem(ea) != "call":
        return False
    operand = idc.print_operand(ea, 0)
    return operand == target_name

def find_pairs():
    start = ida_ida.inf_get_min_ea()
    end = ida_ida.inf_get_max_ea()

    pairs = []

    ea = start
    while ea < end:
        # Bước 1: tìm mov ecx, <dll_res>
        if is_mov_reg_imm(ea, "ecx"):
            dll_res = get_imm_val(ea)
            next_ea = idc.next_head(ea, end)

            # Bước 2: call resolve_dll
            if is_call_to(next_ea, "resolve_dll"):
                # Bước 3: mov edx, <func_res>
                next_ea2 = idc.next_head(next_ea, end)
                if is_mov_reg_imm(next_ea2, "edx"):
                    func_res = get_imm_val(next_ea2)

                    # Bước 4: check tiếp mov ecx, eax và call resolve_func
                    next_ea3 = idc.next_head(next_ea2, end)
                    next_ea4 = idc.next_head(next_ea3, end)
                    if (
                        idc.print_insn_mnem(next_ea3) == "mov"
                        and idc.print_operand(next_ea3, 0) == "ecx"
                        and idc.print_operand(next_ea3, 1) == "eax"
                        and is_call_to(next_ea4, "resolve_func")
                    ):
                        pairs.append((dll_res, func_res, ea))
        ea = idc.next_head(ea, end)

    return pairs

def resolve_and_print():
    pairs = find_pairs()
    print(f"[+] Found {len(pairs)} valid call sequences.")

    for dll_res, func_res, origin in pairs:
        try:
            r = idaapi.Appcall.resolve_dll(dll_res)
            addr = r.__at__
            x = idaapi.Appcall.resolve_func(addr, func_res)
            func_name = ida_name.get_name(x)
            print(f"0x{origin:X}: Resolved func {hex(func_res)} -> {hex(x)} -> {func_name}")
        except Exception as e:
            print(f"0x{origin:X}: Failed to resolve ({hex(dll_res)}, {hex(func_res)}): {e}")

# Gọi chạy
resolve_and_print()
