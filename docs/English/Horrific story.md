# The very horrific story of the reference counter

There comes a time when a computer scientist will have to enter a secret world where the strongest mind can turn in a few hours into a drooling and stuttering wreck. A terrifying world that transforms the slightest bug into a thick brute more dangerous than a werewolf on a full moon day. A world made for minds that have not yet been born. I obviously want to talk about multithreading. 

## Stability

I had created a small Tamgu program whose purpose was to test the strength of my thread implementation. In a graphic window, a multitude of small circles swirled without ever touching each other, chasing a red circle that bounced in every corner of the screen. Each of these circles was controlled by its own thread. 

And the program was crashing....

Not a neat way, in a specific location in the code. No, it would have been too simple. Actually, it would randomly crashed in the most unusual places in the code. But worse than that, it only crashed in release mode. When I executed it in the debugger, it worked perfectly. 

And sometimes it didn't crash at all, running for hours without any problem. And sometimes, it would sundenly engage in an outrageous consumption of memory, often bringing my machine to its knees.

## Can't see any solutions

My mind was full of the most convoluted scenarios that I would test by burning my eyes to follow step by step in debug mode each creation and each destruction of objects. But nothing seemed to make sense. Usually, there is a strange pleasure in discovering the origin of a bug, our mind lights up, a smile appears on our lips, we breathe a sigh of relief. But here, the more I tried to understand what was happening, the more frustration set in. I put traces everywhere in my code, which had the effect of _suppressing the crashes_. I began to suspect the compiler, the operating system (Windows). I could not understand how the presence of a "cout" could make a bug disappear. 

## Once you've eliminated the impossible...

In desperation, I applied Sherlock Holmes' rule and decided to display the reference counter. Tamgu like many other languages uses a reference counter to track the life cycle of an object. When this counter goes to zero the object is destroyed. Until then, I had not given this counter any thought. It was a simple integer that was incremented or decremented according to needs. I didn't see how it could be a source of trouble.

However, during an execution, I saw that a reference counter was set to 2... It should have been 1.

It seemed like a tiny detail, but suddenly I could see the reason why sometimes the program over-consummed memory. If this counter was at 2, it could never go back to 0 later and therefore the object in question would never be destroyed.

## Light

I went back into debugging to try to find out where this counter might have been incremented once too much. Nothing, nada, zilch...

My frustration increased a notch... Maybe under Linux, I'll have more luck... 

I ran the program under "gdb" and again nothing happened. The program was working properly. The release version, on the other hand, crashed with even more conviction than on Windows and in even more improbable places.

I explored Stack Overflow to look at the most common type of errors in multithreading.  But I couldn't find anything conclusive.

Then during a test on Linux, I saw that a reference was 0. It should have been 1.
The program crashed right after....

I suddenly understood that these two bugs were connected. 

And the light came on....

## Sequence

In fact, the code crashed because the reading, arithmetic and writing operations were all dissociated from each other.

Let's take a simple example. We will assume that the initial reference counter value is 1.

```
reference+++; (thread 1)
reference--; (thread 2)
```

These two operations can be represented by the following six instructions:

```
1. reading the reference (thread 1)
2. increment of the reference (thread 1)
3. writing the reference (thread 1)
———
4. reading the reference (thread 2)
5. decrement of the reference (thread 2)
6. reference writing (thread 2) 
```

Unfortunately, in a multithreaded program, these instructions can be mixed freely...

```
Crashing

_1_. reading the reference, we read 1 (thread 1)
_4_... reading the reference, we read 1 more (thread 2) -> ARGH....
2. increment of the reference (thread 1) --> reference is 2
5. decrement of the reference (thread 2) --> reference is 0
3. writing the reference (thread 1) --> write 2
6. write the reference (thread 2) --> overwrite 2 with the value 0, which leads to the destruction of the object.

Crash, the object was destroyed but still in use...
```

As long as the memory cell containing the reference has not been written, it is 1 for both readings. The calculations are done in parallel and therefore lead to two different values. Depending on the order of operations 3 and 6, this leads either to the destruction of the object or to its indestructibility.

```
Indestructible
…
_6_. writing the reference (thread 2) --> write 0
_3_. writing the reference (thread 1) --> overwrite 0 with the value 2, we will no longer be able to destroy this object.
```

The solution was extremely simple, we only needed to declare the reference counter as an "atomic" value.

```C++
std::atomic<short> reference;
```

An "atomic" value allows to synchronize readings and writings in a multithreaded program, which help avoid this type of bugs. The fact that the program does not crash in debug mode is due to the fact that since execution is slower, the risk of mixing instructions is much lower. 

I never thought that simple mathematical operations on an integer could have such a big impact on my code.
