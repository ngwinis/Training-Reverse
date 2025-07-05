import sqlite3

db_path = "anti3\\sc_hashes.db"
table = "symbol_hashes"
NEW_HASH_TYPE = 38  # hash_type mới để phân biệt custom_hash thêm vào

with sqlite3.connect(db_path) as conn:
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()

    # Truy vấn các bản ghi gốc có hash_type = 38
    cur.execute(f'''
        SELECT symbol_name, hash_val, lib_key
        FROM "{table}"
        WHERE hash_type = 38;
    ''')
    rows = cur.fetchall()

    if not rows:
        print("(Không có bản ghi hash_type = 38)")
    else:
        print("symbol_name\tcustom_hash")

        for r in rows:
            original_name = r["symbol_name"]
            hash_val = r["hash_val"]
            lib_key = r["lib_key"]

            # Tính custom_hash
            custom_hash = (-hash_val) % 0xFFFFFFFF + 1
            print(f"{original_name}\t{custom_hash}")

            # Thêm vào bảng
            cur.execute(f'''
                INSERT INTO {table} (hash_val, hash_type, lib_key, symbol_name)
                VALUES (?, ?, ?, ?)
            ''', (custom_hash, NEW_HASH_TYPE, lib_key, f"{original_name}"))

        conn.commit()
        print(f"Đã thêm {len(rows)} bản ghi custom_hash mới với hash_type = {NEW_HASH_TYPE}")
