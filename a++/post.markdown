### Introduction to A++

Despite having the same name with just one more plus, A++ is not
related in any way to A+. A++ is a truly tiny, purely functional
programming language, whereas A+ is anything but tiny and is far from
being purely functional.

[A++](http://aplusplus.net/), which is short for
Abstraction + reference + synthesis, is an abstraction of [the lambda
calculus](http://en.wikipedia.org/wiki/Lambda_calculus), with a syntax
that is even simpler (and thankfully contains only symbols which are
on a standard keyboard). This simplicity is due to the fact that it
borrows its syntax from Lisp and Scheme, requiring parentheses to
surround each expression.

The addition that A++ makes to lambda calculus is the ability to
explicityly assign names to objects (functions or values), something
which lambda calculus only supports through binding via function
calls.

A++ is intended to be a "learning instrument for programming" and thus
doesn't try to add any extensions to the language which would make it
too useful for everyday programming.

Due to its simplicity, it is a great language for getting to
understand concepts which would normally be overshadowed by all the
other stuff in a language.

A++ is a
[Lisp-1](http://en.wikipedia.org/wiki/Lisp-1#The_function_namespace),
with a single namespace for both functions and data. Indeed, in A++
there is no operator to define a function, one simply *define*s a
symbol to map to a lambda expression.

Since A++ uses lazy-evaluation, control structures such as if
statements and while loops can be created with just lambdas, without
having to add macros to the language.

There are some primitive functions built into the interpreter to allow
for converting church numerals into regular numbers, and for
displaying booleans and numbers. There is support for primitive
integers, strings, and symbolic constants. However, the support for
primitive integers is limited to the minimum needed for converting
from a church-encoded integer to a primitive integer, so the primitive
integer can be printed, and the support for strings is only in place
to allow for loading of external files into the interpreter. This
means that in the language, you have to work exclusively with church
numerals, and the only *really* useful primitive (aside from lambdas and
define) are the symbolic constants.

All the standard lisp stuff, like car, cdr, map, filter, boolean
operators (true and false, once again, defined in church encoding),
various predicates (like nullp and zerop), while, and for-each are all
built in the A++ language itself, and loaded automatically from a file
when the interpreter is started, rather than being built-in functions.

So, when I said this was a *purely* functional language, I wasn't
kidding. The only difference from what most people would consider a
purely functional language, is the fact that you can re-define a new
value to the same name. Of course, without the ability to do this,
closures would be of limited usefulness.

### My First A++ Program

I had a hard time coming up with an idea for a program to implement in
A++. Mainly, anything more complex I wanted to do, I couldn't because
of the very limited interaction with the outside world allowed by the
A++ interpreter.

I looked at the list of [solutions by programming task at Rosetta
Code](http://rosettacode.org/wiki/Category:Solutions_by_Programming_Task),
and decided to implement the Roman Numerals task. 

I based my solution off [the first C solution for roman
numerals](http://rosettacode.org/wiki/Roman_Numerals#C). Due to the
lazy-evaluation provided by A++, I was able to translate the macros
used almost exactly.

<pre lang="lisp">
(define roman 
    (lambda(n)

      (define iftrue      ; standard if function requires an "else" bit
          (lambda(b t)
            (if b t nil)))

      (define digit 
          (lambda(loop num c)
            (loop (gep n num) 
               (lambda()
                 (print c)
                 (define n (sub n num))))))
      (define digits
          (lambda(loop num c1 c2)
            (loop (gep n num)
               (lambda()
                 (print c1)
                 (print c2)
                 (define n (sub n num))))))

      (define hundred (mult ten ten))

      (digit  while   (mult ten hundred) 'M   )
      (digits iftrue (mult nine hundred) 'C 'M)
      (digit  iftrue (mult five hundred) 'D   )
      (digits iftrue (mult four hundred) 'C 'D)
      (digit  while              hundred 'C   )
      (digits iftrue     (mult nine ten) 'X 'C)
      (digit  iftrue     (mult five ten) 'L   )
      (digits iftrue     (mult four ten) 'X 'L)
      (digit  while                  ten 'X   )
      (digits iftrue                nine 'I 'X)
      (digit  iftrue                five 'V   )
      (digits iftrue                four 'I 'V)
      (digit  while                  one 'I   )))

</pre>

You can [download the A++ roman numeral program][romanprog] if you'd like.

[romanprog]:

Sadly, A++ offers no way to build up a string for printing or to print
multiple items on a single line, so each of the numerals will be
printed on its own line.

Also, I chose this particular implementation because A++ doesn't
provide a function to perform division!

**continue on to learn more about A++**

<!--more-->

### Anatomy of an A++ Program

There are only three things you can do in an A++ program:

1. Define a name to have a value (assignment).

2. Create a lambda expression.

3. Apply a lambda expression (i.e. call a function).

One thing lambda calculus doesn't directly have is data, so in pure
lambda calculus, data is represented by various nested lambda
expressions via [Church
encoding](http://en.wikipedia.org/wiki/Church_encoding). A++ uses the
same technique to encode most of its data. For example, the numbers
zero through ten are defined in the default A++ environment as
follows:

<pre lang="lisp">
(define zero   (lambda (f) 
                 (lambda (x) 
                   x)))
(define one    (lambda (f) 
                 (lambda (x) 
                   (f x))))
(define two    (lambda (f) 
                 (lambda (x) 
                   (f (f x)))))
(define three  (lambda (f) 
                 (lambda (x) 
                   (f (f (f x))))))
(define four   (succ three))
(define five   (succ four))
(define six    (mult two three))
(define seven  (add three four))
(define eight  (add four four))
(define nine   (add four five))
(define ten    (add five five))
</pre>


A++ has lexical scoping, with variables in a function either being
"free," meaning they were inhereted from a parent of the current
scope, or "lambda-bound," which means they were bound as arguments to
the current function.

Since all lambdas are closures, you can create a state inside of a
function, then return a function which will have access to that
state. This implements data-hiding. You can use this feature to
implement object-oriented programming or just use it to have a
function save state between invocations.

### Installing A++

There are a couple of A++ interpreters available for download. I chose
to use the version in C.

You can get any of the implementations at [the A++ download page](http://www.aplusplus.net/app/download.html).

I downloaded appc.tar.gz, unpacked it, installed the required
dependency of the Boehm garbage collector, and compiled it as follows:

<pre lang="sh">
$ wget http://www.aplusplus.net/app/download/appc.tar.gz
--2009-07-12 14:43:01--  http://www.aplusplus.net/app/download/appc.tar.gz
Resolving www.aplusplus.net... 212.162.12.193
Connecting to www.aplusplus.net|212.162.12.193|:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: 105773 (103K) [application/x-tar]
Saving to: `appc.tar.gz'

100%[===============================================================================>] 105,773     88.7K/s   in 1.2s    

2009-07-12 14:43:03 (88.7 KB/s) - `appc.tar.gz' saved [105773/105773]

$ tar zxf appc.tar.gz 

$ cd appc

$ sudo apt-get install libgc-dev
  ...

$ time make
gcc -c  arsc.c
gcc -c  arsparser.c
gcc -c  mainint.c
gcc -g  -rdynamic -o arscint arsc.o arsparser.o  mainint.o -ldl -lgc -lc 

real    0m0.219s
user    0m0.168s
sys     0m0.036s

</pre>

And that's it! You can now run the resulting arscint, and get a lovely
shell you can play around with. I timed the make, just to show that it
takes less than a quarter of a second to compile. It's a very tiny
language.

### Running A++

Running A++ is done by simply starting up the shell like so:

<pre lang="sh">
$ ./arscint
  ...
ARS-Eval-> 
</pre>

The "..." represents about 50 lines of stuff that's printed out as the
init file is loaded as if you typed the lines in one-by-one. After
that, you get the ARS-Eval prompt, and you're ready to start running
commands.

If you want to run my roman numeral function from above, you can use
it as follows, assuming you have downloaded the file and put it in
your appc directory:

<pre lang="lisp">
ARS-Eval-> (load "roman.ars")
roman.ars
> > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > lambda(n)
*eof*


ARS-Eval-> (roman (add ten four)) ; ...and then wait about 5 seconds
-->X
-->I
-->I
-->I
-->I
lambda(x y)



ARS-Eval-> (roman (add four (mult ten ten))) ; ... and then wait about 20 seconds
-->C
-->I
-->I
-->I
-->I
lambda(x y)



ARS-Eval-> (define n (add six (mult ten  (mult ten (add ten seven)))))
lambda(f)



ARS-Eval-> (ndisp! n)
-->1706
void



ARS-Eval-> (roman n) ; .. and the first numeral prints out quickly, then a several minute pause...then it accelerates toward the end
-->M
-->C
-->C
-->C
-->C
-->C
-->C
-->C
-->I
-->I
-->I
-->I
-->I
-->I
lambda(x y)



ARS-Eval-> (quit) ; or CTRL-D if you want
</pre>

Not the most efficient piece of code, but it gives you an idea of how things work in A++


### Learning more about A++

The creator of A++ has written a couple of books on the language, and
the one I'd recommend starting out with is [The Smallest Programming
Language in the
World](http://www.aplusplus.net/bookonl/aplpleonl.html). It not only
gives a good introduction to the language, but it goes into some
detail on how and why things are done the way they are.

I've left completely uncovered the way in which the author implements
object-oriented programming in A++, but I would highly recommend you
check out the section in the book on [object-oriented programming in
A++](http://www.aplusplus.net/bookonl/node140.html).

### Conclusion

Even though this is a simple, for-education-only language, it has
already given me inspiration for ways to simplify the core of my own
programming language (which I will discuss in detail in a later post,
when there's actually a working implementation to write about).

I would highly recommend for any aspiring programmer, or even
professional programmer, to at least read [the A++
book](http://aplusplus.net/bookonl/index.html), even if you don't
write a single line of A++ code, I think you can get a lot out of
seeing how it's put together.
