def compile(program):
    indent = 4
    code = [
        'def compiled(input=None, output=None):',
        '    import sys',
        '    if not input: input = sys.stdin',
        '    if not output: output = sys.stout',
        '    i = 0',
        '    a = [0]*30000'
        ]
    commands = {
        '>': 'i += 1',
        '<': 'i -= 1',
        '+': 'a[i] += 1',
        '-': 'a[i] -= 1',
        '.': 'output.write(chr(a[i]))',
        ',': 'a[i] = ord(input.read(1))',
        '[': 'while a[i]:',
        ']': ''
        }
    for command in program:
        line = commands.get(command, None)
        if line: code.append((' ' * indent) + line)

        if command == '[':   indent += 4
        elif command == ']': indent -= 4
    exec '\n'.join(code)
    return compiled
