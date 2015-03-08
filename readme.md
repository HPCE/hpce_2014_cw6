HPCE 2014 CW6
=============

- Issued: Sun 8th March
- Due: Sun 22nd March, 23:59

(First DoC exam open to EEE/EIE/MSc is 24th March, AFAICT)

(Just realised I haven't made the private repositories yet. To come...)

Specification
-------------

You have been given the included code with the
goal of making things faster. On enquiring which
things, you were told "all the things". Further
deliberation on what a "thing" was resulted in
the elaboration that it was an instance of
`puzzler::Puzzle`. Further tentative queries
revealed that "faster" was determined by the
wall-clock execution time of `puzzler::Puzzle::Execute`,
with an emphasis on larger scale factors, on an
amazon GPU instance.

At this point marketing got quite irate, and
complained about developers not knowing how to
do their job, and they had commissioned this wonderful
enterprise framework, and did they have to do
all the coding themselves? Sales then chimed
in that they had similar problems having to
hold the developers hand, and that they did
VBA as part of their business masters, and it was
easy. Oh, and that they had already sold a customer
a version that contains more things; the spec should
be ready on Friday 13th (and no, that is not ominous,
it just happens to be a religious holiday for the
customer), but it is only "small" stuff. Developers
are all agile these days aren't they?

Meta-specification
------------------

The previous coursework was about deep diving on one
problem, and (hopefully) trying out a number of alternative
strategies. This coursework represents the other end
of the spectrum, which is sadly the more common end: you
haven't got much time, either to analyse the problem or
to do low-level optimisation, and the problem is actually
a large number of sub-problems. So the goal here is to
identify and capture as much of the low-hanging performance
fruit as possible while not breaking anything.

The code-base I've given you is somewhat baroque,
(though not as convoluted as my original version,
I took pity), and despite having some rather iffy
OOP practises, actually has things quite reasonably
isolated. You will probably encounter the problem
that sometimes the reference solution starts to take
a very long time at large scales, but the persistence
framework gives you a way of dealing with that.

Beyond that, there isn't a lot more guidance, either
in terms of what you should focus on, or how
_exactly_ it will be measured. Part of the assesment
is in seeing whether you can work out what can be
accelerated, and where you should spend your time.
And in reacting to externally evolving specs and
code - the Friday 13th comment is true, though the
change is minor (there aren't another five problems),
additive (all work done this week is needed and
evaluated for the final assesment), and has a default
fallback (a git pull will bring any submission back
into correcness).

The allocation of marks I'm using is as before:

- Performance: 33%

  - You are competing with each other here, so there is an element of
    judgement in terms of how much you think others are doing or are
    capable of.

- Correctness: 33%

  - As far as I'm aware the ReferenceExecute is always correct, though slow.

- Code style, insight, analysis: 33%

  - Can I understand your code (can you understand your code)? Are the methods
    and techniques employed appropriate?

Deliverable format
------------------

- As before, your repository should contain a readme.txt, readme.pdf, or readme.md covering:

    - What is the approach used to improve performance, in terms of algorithms, patterns, and optimisations.

    - A description of any testing methodology or verification.

    - A summary of how work was partitioned within the pair, including planning, analysis, design, and testing, as well as coding.

- Anything in the `include` directory is not owned by you, and subject to change
  
  - Changes will happen in an additive way (existing classes and APIs will remain, new ones may be added)
  
  - Bug-fixes to `include` stuff are still welcome.

- The public entry point to your code is via `puzzler::PuzzleRegistrar::UserRegisterPuzzles`,
    which must be compiled into the static library `lib/libpuzzler.a`.
    
    - Clients will not directly include your code, they will only `#include "puzzler/puzzles.h`,
      then access puzzles via the registrar. They will get access to the registrar implementation
      by linking against `lib/libpuzzler.a`.
    
    - **Note**: If you do something complicated in your building of libpuzzler, it should still be
      possible to build it by going into `lib` and calling `make all`.
      
- The programs in `src` have no special meaning or status, they are just example programs 

The reason for all this strange indirection is that I want to give
maximum freedom for you to do strange things within your implementation
(example definitions of "strange" include CMake) while still having a clean
abstraction layer between your code and the client code.

Notes
-----

All the algorithms here are quite classic, though for the most
part different enough to require some thought. Where it is possible
to directly use an off-the-shelf implementation (partially true in
most cases), you need to bear in mind that you're trying to
show-case your understanding and ability here. So if you're
relying on someone elses library, you need to:

- Correctly and clearly attribute it
- Be able to demonstrate you understand how and why it works

Make sure you spend a little bit of time thinking about how
feasible it is to accelerate something - in some cases you
may be able to get linear speed-up in the processor count,
in others less so. Sometimes the fundamental algorithmic
complexity doesn't look friendly, and can be improved in
simple ways.
