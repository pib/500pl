Python is one of my favorite programming languages. It's almost always
the first one that I reach for when I have a programming task I'd like
to try out, and often enough, it's the final language of that task,
too.

### Introduction to Python

On the [official Python website](http://python.org/), Python is
described as "a dynamic object-oriented programming languages that can
be used for many kinds of software development."

They also claim that "Many Python programmers report substantial
productivity gains and feel the language encourages the development of
higher quality, more maintainable code."

I am inclined to agree.

Though Python is a true "everything is an object" OO language, you can
do more than just Object-Oriented programming with it. You can do
plain-ol' procedural programming, functional programming,
Object-Oriented programming, and probably some other types of
programming if you wanted.

One of the nicest things about Python is the "batteries included"
approach, which means that Python comes with a ton of modules that do
a large amount of what you want to do, right out of the box (or
tarball, or installer...I don't know if you can actually get Python in
a box). Does your app need to write a [cgi app](http://docs.python.org/library/cgi.html)?
Or even better, a [wsgi app](http://docs.python.org/library/wsgiref.html)?
[Web](http://docs.python.org/library/httplib.html) [client](http://docs.python.org/library/urllib2.html)?
[Web server?](http://docs.python.org/library/simplehttpserver.html)? 

Yep, it has all those, and many, many more.

There are many, [more thorough introductions to
Python](http://docs.python.org/tutorial/index.html) out there, so I
won't attempt to cover the language in too much more depth, but I will
show you a couple of things that really capture the spirit of Python:

<pre lang="python">
Python 2.6.2 (release26-maint, Apr 19 2009, 01:58:18) 
[GCC 4.3.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> from __future__ import braces
  File "<stdin>", line 1
SyntaxError: not a chance
>>> import this
The Zen of Python, by Tim Peters

Beautiful is better than ugly.
Explicit is better than implicit.
Simple is better than complex.
Complex is better than complicated.
Flat is better than nested.
Sparse is better than dense.
Readability counts.
Special cases aren't special enough to break the rules.
Although practicality beats purity.
Errors should never pass silently.
Unless explicitly silenced.
In the face of ambiguity, refuse the temptation to guess.
There should be one-- and preferably only one --obvious way to do it.
Although that way may not be obvious at first unless you're Dutch.
Now is better than never.
Although never is often better than *right* now.
If the implementation is hard to explain, it's a bad idea.
If the implementation is easy to explain, it may be a good idea.
Namespaces are one honking great idea -- let's do more of those!
>>> import __hello__
Hello world...
>>> 
</pre>

### My <del>First</del> Python Program

This is far from my first program in Python. I first discovered Python
when I was in High School, so possibly as early as 1999-2000. I had
discovered [Blender, a free 3D editing
application](http://www.blender.org/), and it had Python embedded
within it, for use in creating extensions and scripting games for the
built-in game engine. Back then, I still thought I was going to be a
video game developer, so I made some effort to learn Python so I could
write games in Blender.

Anyway, enough history. :)

For this program, I implemented a
[Brainfuck](http://en.wikipedia.org/wiki/Brainfuck)-to-Python
compiler. I won't go into Brainfuck right now because I will be doing
a post on it next.

This program is nowhere near demonstrating all of, or even a large
number of, Python's features, but it does demonstrate how easy it is
to write a quick-and-easy program. 

I threw this compiler together in less than 5 minutes, and it
functioned properly on my first try (the [Brainfuck hello world from
Wikipedia](http://en.wikipedia.org/wiki/Brainfuck#Hello_World.21)).

The only addition I had to make later was handling the case of
end-of-file on the input.

<pre lang="python">
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
</pre>

This function takes a string containing BF code as its one argument
and returns a function which will run that code when called. 

The function returned will have two optional arguments, input and
output, which are expected to be file-like objects (at least
implementing read and write, respectively), which allows for passing
in specific input and/or handling the output from within a python
program.

It's kind of a dirty way to do it, building Python source, an then
running it with *exec*, but it gets the job done. I could also save
the function to a compiled .pyc file, so you could later import it
directly, but that would require a bit more code and ruin the
simplicity of it all. I could also have generated input for my [Python
assembler](http://probablyprogramming.com/2008/04/18/ppya-python-assembler/),
and used that to generate a compiled .pyc file, but like I said, I was
going for simplicity here, and encouraging people to learn Python, not
necessarily Python bytecodes.

You can [download the Brainfuck-to-Python Compiler][xprog] if you'd like.

[xprog]:


**continue on to learn more about Python**

<!--more-->

### Anatomy of a Python program

#### Modules

Python is organized in modules and (optionally) packages. A module is
simply a file which contains some amount of Python code. A package is
simply a directory which contains some number of modules.

You can import modules with the *import* statement, which runs the
code in the module, and then imports any objects the module defines
into the current context (generally under some namespace, see the
[Python Modules
documentation](http://docs.python.org/tutorial/modules.html) for more
details on the import statement).)

The code in a module can be at the module-level, outside of any
functions, or it can be contained in functions and classes. Code at
the module level is run immediately when the module is imported.

For short scripts, it's usually fine to write code at the module
level. For more complex modules, it's common practice to put all the
code into functions, and then conditionally invoke that function if
the module is run directly.

For example, to extend my BF compiler above to be invoked directly
from the commandline, I added the following code to the end of the bf
module:

<pre lang="python">

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
</pre>

The cmd_line function is just a regular function. The real magic
happens on the last line there. At the module level, __name__ will be
set to "__main__" if the current module is the one which was run
directly.

If a module is imported from any other module, __name__ is set to the
name of the module.

#### Python Shell

You can also run Python code directly, one line at a time, from the
python shell. The shell comes up if you just run Python directly
without specifying a module for it to run.

Python makes a great calculator, random-number picker, interactive
prototyping tool, etc.

Often when I'm playing around with an idea for how to do something,
I'll just open up the Python shell and play around until I get it
working how I'd like, then I will commit the code to an actual file.

### Installing and Running Python

There's a nice [guide to installing and running
Python](http://docs.python.org/using/index.html) on various systems at
Python.org, so I won't spend time repeating what they've got there.


### Learning more about Python

A good place to start is [the Python
tutorial](http://docs.python.org/tutorial/), and there's a lot to
learn from the links on the [Python
Documentation](http://python.org/doc/) page.


### Conclusion

Python is a great language, for both new programmers and experienced
programmers who want to try a language that is probably way nicer than
what they're used to programming in, especially if what you're used to
programming in is PHP. Heck, even [Terry Chay recommends Python as a
learning
language](http://terrychay.com/blog/article/learning-programming.shtml),
and he's known to be a [strong supporter of PHP](http://terrychay.com/blog/article/category/web-development/php).

In conclusion, if you haven't yet, give Python a try. You won't regret it!
