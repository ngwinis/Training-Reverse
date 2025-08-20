import idaapi
import ida_name
import ida_xref
import ida_funcs
import ida_bytes

# địa chỉ thật thay thế tại đây
resolve_dll_addr = 0x00F41DF0
resolve_api_addr = 0x00F41F10

# thu thập xref resolve_dll
dll_xrefs = []
xref = ida_xref.get_first_fcref_to(resolve_dll_addr)
while xref != idaapi.BADADDR:
    dll_xrefs.append(xref)
    xref = ida_xref.get_next_fcref_to(resolve_dll_addr, xref)

print(f"[*] Found {len(dll_xrefs)} xref(s) to resolve_dll")

# thu thập xref resolve_api
func_xrefs = []
xref = ida_xref.get_first_fcref_to(resolve_api_addr)
while xref != idaapi.BADADDR:
    func_xrefs.append(xref)
    xref = ida_xref.get_next_fcref_to(resolve_api_addr, xref)

print(f"[*] Found {len(func_xrefs)} xref(s) to resolve_api")

# xử lý resolve_dll
dll_list = []
for call_addr in dll_xrefs:
    print(f"[resolve_dll] call at 0x{call_addr:X}")
    args = idaapi.get_arg_addrs(call_addr)
    if not args or len(args) < 1:
        print(f"  cannot get argument at 0x{call_addr:X}")
        continue

    arg_inst = args[0]
    operand_val = None

    insn = idaapi.insn_t()
    idaapi.decode_insn(insn, arg_inst)
    for i in range(idaapi.UA_MAXOP):
        op = insn.ops[i]
        if op.type == idaapi.o_imm:
            operand_val = op.value
            break

    if operand_val is None:
        print(f"  cannot decode operand at 0x{arg_inst:X}")
        continue

    try:
        r = idaapi.Appcall.resolve_dll(operand_val)
        base = r.__at__
        dll_list.append(base)
        idaapi.set_cmt(call_addr, f"resolved base = 0x{base:X}", 0)
        print(f"  resolved base: 0x{base:X}")
    except Exception as e:
        print(f"  appcall failed: {e}")

# xử lý resolve_api
for dll, call_addr in zip(dll_list, func_xrefs):
    print(f"[resolve_api] call at 0x{call_addr:X}")
    args = idaapi.get_arg_addrs(call_addr)
    if not args or len(args) < 2:
        print(f"  cannot get arguments at 0x{call_addr:X}")
        continue

    hash_val = None

    insn2 = idaapi.insn_t()
    idaapi.decode_insn(insn2, args[1])
    for i in range(idaapi.UA_MAXOP):
        op = insn2.ops[i]
        if op.type == idaapi.o_imm:
            hash_val = op.value
            break

    if hash_val is None:
        print(f"  cannot decode operands at 0x{call_addr:X}")
        continue

    try:
        x = idaapi.Appcall.resolve_api(dll, hash_val)
        func_name = ida_name.get_name(x)
        idaapi.set_cmt(call_addr, f"resolved func 0x{hash_val:X} -> {func_name}", 0)
        print(f"  resolved func 0x{hash_val:X} -> 0x{x:X} -> {func_name}")
    except Exception as e:
        print(f"  appcall failed: {e}")
