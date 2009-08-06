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