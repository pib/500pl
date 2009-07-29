After my last [post on the A++ programming language][lastpost], there
was quite a bit of [discussion on reddit][redditdis] about both a
mistake I made about the classification of the language (which I
corrected in my previous post), and questions about how A++ is
different from Scheme.

[lastpost]: http://probablyprogramming.com/2009/07/13/the-aplusplus-programming-language/
[redditdis]: http://www.reddit.com/r/lisp/comments/9109y/the_a_programming_language/

Because of the relative simplicity of A++ compared to Scheme, rather
than talk about how they compare and contrast (that would mostly be
like, "Scheme does this, this, this, this, this, and this, whereas A++
does this"), I'm simply going to talk about the main aspects of A++,
namely, its simplicity and the fact that it uses lazy evaluation.

### The core of A++ is simplicity (and lambda-calculus)

The idea behind A++ is that it is lambda-calculus, with a simplified
uniform syntax (using parens all the time, rather than only in certain
cases), and with explicit name-binding added to it.

What this means is that *everything* is a function, with the exception
of named constants, which enable a simple form of message-passing to
be implemented. The only thing you can do with named constants is
check if they are equal, so they add a very small amount of complexity
to the language.

There are no data structures (that aren't made up of nested lambdas),
there are no primitive values, and no built-in operators, aside from
those which allow the definition, binding, and applying of functions.

A tiny exception is made in instances when printing things out, but
the only thing you can do a with a primitive is increment it,
decrement it, or print it out, so it can't be used in a program. Named
constants also don't count, since they essentially don't have
values. Ignoring these two trivial exceptions, there's one main rule
that A++ follows:

### In A++, there are (pretty much) only functions.

The only (non-output-related) built-in operations are define, lambda,
apply (which is done when the first thing inside some parenstheses
isn't "define", "lambda", or "equal"), and equal (which is used for
comparing the aforementioned named constants).

To make A++ the powerful enough to write new language constructs
within the language itself, it [lazy evaluation](http://en.wikipedia.org/wiki/Lazy_evaluation).

### Lazy evaluation

Parameter evaluation is done lazily, meaning an operation passed into
a function is only applied (called) when you explicitly apply it (by
wrapping it in a pair of parentheses).

This is needed in A++, because there are no built-in control
structures. While loops aren't strictly needed because A++ allows
recursion, you do need some sort of conditional statement.

In A++, true and false, and if are defined as follows:

<pre lang="lisp">
(define true   (lambda (x y) 
                 x))

(define false  (lambda (x y) 
                 y))

(define if (lambda (b t f) (b t f)))
</pre> 

Any predicate or boolean operation evaluates to either true or false,
so when passed to if, it makes sense that only one of the two
following parameters is evaluated.

If A++ used eager evaluation, both the t and f parameters would be
evaluated before being passed into the if function, which, since A++
allows side-effects, could result in unwanted output. Even if A++ was
purely functional, allowing no side-effects, it would waste time to
evaluate both branches of the if function.

For example, if you were to run the following code:

<pre lang="lisp">
(if true (ndisp! four) (ndisp! five))
</pre>

you would get the expected output from A++ of

    -->4

but if A++ eagerly-evaluated the parameters to if, you would end up with

    -->4
    -->5

instead (or possibly in the opposite order, depending on which
evaluation order was used by the language).

Lisp offers deferred evaluation via the quote operator, and allows the
construction of control structures with macros. Since the goal of A++
is simplicity, it makes sense that it would pick the lazy evaluation
scheme, which allows it to fulfil both of those roles without any
extra feature being added to the language.

<div class="pitch">
This post is part of an ongoing series in which I am attempting to write about, and write programs in, 500 different programming languages, <a href="http://probablyprogramming.com/category/500-programming-languages/">500 Programming Languages</a><br /><br />
If you have any languages to suggest, or comments to make about this post, or the project in general, please don't hesitate to leave a comment on this post or the main 500 Languages post.
</div>
