def postfix_to_infix(postfix):
    stack = []
    operators = set("+-*/")

    for char in postfix:
        if char not in operators:
            stack.append(char)
        else:
            b = stack.pop()
            a = stack.pop()
            expr = f"({a}{char}{b})"
            stack.append(expr)
    return stack[0]

postfix_expr = "325*4-+763/*+"
infix_expr = postfix_to_infix(postfix_expr)
print("Postfix:", postfix_expr)
print("Infix:", infix_expr)

# Input: ((3+((2*5)-4))+(7*(6/3)))