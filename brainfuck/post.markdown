### Introduction to brainfuck

brainfuck is an esoteric programming language based on the idea of
making a language with the smallest possible compiler. Indeed, [my
Python brainfuck
compiler](http://probablyprogramming.com/2009/08/01/500-programming-languages-python/)
is only 30 lines of Python code, and I'm sure there are some
optimizations I could have done to make it shorter if that was my goal
with the whole thing.

The only data structure you get is a single array (if it were infinite
you'd have a good representation of a Turing machine), and all you can
do is move around this array with a pointer, increment and decrement
values, input and output values, and loop.

The entire language consists of 8 commands:

- **>** move the pointer forward one cell
- **<** move the pointer back one cell
- **+** increment the current cell
- **-** decrement the current cell
- **.** output the byte in the current cell
- **,** input a byte into the current cell
- **[** move to the matching **]** if the current cell is zero
- **]** loop back to the matching **[** if the current cell is non-zero

Anything other characters in a program are ignored, and thus comments
can be inserted anywhere into the program. Just be careful not to type
proper English sentences with commas and periods, or you might get
some funny bugs until you realize what's happening.

That's really all there is to the language. There are a couple of
areas of behavior that aren't standardized between interpreters,
relating to the size of the array, the cell size, and handling
end-of-file when inputing. Those are covered in more detail on the
[brainfuck Wikipedia](http://en.wikipedia.org/wiki/Brainfuck) page.

So, since this is such a simple language (to comprehend, not to write
in), and I already provided a working compiler in my previous post,
I'm going to do this post a little differently, and just cover the
process I went through in writing my first non-trivial brainfuck
program.

### The idea

I tried to come up with an idea for a program you don't normally see
implemented in brainfuck, and I've never seen anything done with
brainfuck and audio, so I decided to write a simple (non-interactive)
synthesizer.

### My first brainfuck program

To start out with, I wrote a program to just generate one second of
simple square wave, at 4kHz (the max for 8kHz sample-rate audio).

With 8000Hz audio, there are 8000 bytes per second, so if the
wavelength is two bytes (one byte high, one byte low, makes a complete
single wave), that makes the frequency 8000/2, or 4000kHz. If I
doubled the wavelength, I'd get half the frequency, or 2000Hz.

<pre lang="brainfuck">
set counter to four thousand
> ++++ four
[ > +++++ +++++ times ten
  [ > +++++ +++++ times ten
    [ > +++++ +++++ times ten
      [ <<<< + >>>> - ]
      < -
    ]
    < -
  ]
  < -
]

keep a zero value and generate a 255 value
>> +++++ five
[ > +++++ times 5
  [ > +++++ +++++ times ten
    [ <<< + >>> - ]
    < -
  ]
  < -
]
< +++++ plus five (255 total)

<<
[
  >> . output 255
  < .  output 0
  <
  - decrement counter
]
</pre>

This demonstrates how even generating a large number in brainfuck is
quite a chore. I had to use four nested loops and four different cells
to calculate 4*10*10*10. Still, it's better than having 4000 plusses
at the beginning of my program.

The way loops are done in brainfuck reminds me of doing loops in
assembly language. In fact, brainfuck in general reminds me of
assembly language, but without all those convenient opcodes you're
used to. :)

### What's your frequency, brainfuck?

Next, I wanted to generate a lower frequency, working my way toward
taking user input to determine frequency. 

I picked the A below middle C, which has a nice, even frequency of
220Hz. When you divide 8000 by 220, you come up with a wavelength of
approximately 36. Half low and half high makes that 18 bytes per half,
so I needed to update the previous program to output 18 bytes of each
value instead of just one byte per value.

<pre lang="brainfuck">
222 waves of 36 bytes each equals 8000 samples for 1 second of audio
> +++++ +++++ +++++ +++++ ++  22
[ > +++++ +++++ times ten
  [ << + >> - ]
  < -
]
++ plus 2 more

>> +++++ five
[ > +++++ times 5
  [ > +++++ +++++ times ten
    [ <<< + >>> - ]
    < -
  ]
  < -
]
< +++++ plus five (255 total)

<<
[
  >>>
  +++++ +++++ +++++ +++ eighteen up and down = 220Hz
  [
    < . output 255 byte
    > - subtract from counter
  ]
  +++++ +++++ +++++ +++ eighteen up and down = 220Hz
  [
    << . output 0 byte
    >> - subtract from counter
  ]
  <<<
  -
]
</pre>

This version adds two extra loops and an extra cell to generate
eighteen 255s and eighteen zeroes for each iteration of the loop.

### Like MIDI, but crappier

So, the next and final step was to allow input of notes to play, and
the lengths to play each of those notes.

I found a helpful reference on the [frequencies of musical
notes](http://www.phy.mtu.edu/~suits/notefreqs.html).

Due to the fact that brainfuck only allows you to read a single byte
at a time, and because it made the program simpler over all, I decided
to have the two values input for each note represent the length, in
waves, and the length of each half-wave. This allowed me to use the
basic structure of the previous program, and to support a wider range
of frequencies without having to handle multiple-byte input for notes.

<pre lang="brainfuck">
generate the 255 byte for output
>>> +++++ five
[ > +++++ times 5
  [ > +++++ +++++ times ten
    [ <<< + >>> - ]
    < -
  ]
  < -
]
< +++++ plus five (255 total)
<<

, read in the length of the first note (in waves)
> , read in the length of each half wave
[
  <
  [
    >
    [->>+>+>+<<<<] copy the value to two cells for use in the following loops plus an extra copy
    >>>>[-<<<<+>>>>] copy the value back to the original location
    <<
    [
      < . output 255 byte
      > - subtract from counter
    ]
    > goto next counter
    [
      < . output 0 byte (zeroed by the last loop)
      > - subtract from counter
    ]
    <<<<
    -
  ]
  , read in the length of the next note (in waves)
  > , read in the length of each half wave
]
</pre>

Since the half-wave length is input from the user, I can't simply
re-generate the value each time through the main loop, so I had to
make extra copies of the value, and then re-copy the value back to its
original input location, since it's zeroed out when it's copied to the
other location. 

Like I said, it's harder than assembly language.

Now all you have to do is fire up your favorite hex editor and create a binary file with a list of lengths and notes (for example [mary.notes][]), and save the output from the program into a file (for example, [mary.raw][]), and you will have your very own brainfuck-generated music! 

In an 8-bit unsigned 8kHz mono raw PCM file. Linux users can use aplay to play this file. It just so happens the default format is the same as the output of this file. This also means you can pipe the output of this program directly into aplay, as well. 

All you need now is some sort of front-end which will interactively
take keyboard input and translate it into notes, and you could use
this for live performances. (note: you probably don't want to use this
for live performances)

[mary.notes]: 
[mary.raw]:

### Learning more about brainfuck

I learned everything I know about brainfuck from the [brainfuck
Wikipedia page](http://en.wikipedia.org/wiki/Brainfuck), and so can
you!


### Conclusion

This is probably not a language you want to use, like, ever.

Unless you're a huge nerd, like myself. 

Then you might want to, just for the fun of it.

If it doesn't sound like fun to you, then you can safely say you're
less of a nerd than me.
