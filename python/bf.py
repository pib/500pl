def compile(program):
    indent = 4
    code = [
        'def compiled(input=None, output=None):',
        '    import sys',
        '    if not input: input = sys.stdin',
        '    if not output: output = sys.stdout',
        '    i = 0',
        '    a = [0]*30000'
        ]
    commands = {
        '>': 'i += 1',
        '<': 'i -= 1',
        '+': 'a[i] += 1',
        '-': 'a[i] -= 1',
        '.': 'output.write(chr(a[i]))',
        ',': 'a[i] = ord(input.read(1) or "\0")',
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

def cmd_line():
    import sys
    if len(sys.argv) < 2:
        print('usage: %s bf_file\n' % sys.argv[0])
    else:
        try:
            with file(sys.argv[1]) as f:
                bf_fn = compile(f.read())
        except IOError, msg:
            print("couldn't read file %s: %s\n" % (sys.argv[1], msg))

        bf_fn()

if __name__ == '__main__': cmd_line()

