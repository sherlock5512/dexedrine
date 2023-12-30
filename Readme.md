# Dexedrine - A simple way to keep your system up

## Dependencies
- libsystemd
- libbsd
- A C compiler (although this may only work with GCC)

## How to use
- Run `make`
- Move the binary to somewhere in your PATH (or run `make install`)
- Run `dexedrine`
    - Your system will now be blocked from suspending, going idle, and it
    will ignore the lid switch
    - To stop it either:
        - Run `dexedrine` again (if you started it outside
        of a terminal) it will find the running instance and send it a `SIGINT`
        for you.
        - Send `SIGINT` to `dexedrine`.

### Advanced usage
You can also run `dexedrine` as a `systemd` user service
```bash
systemctl --user start dexedrine.service
```


## Backstory
This is by all means a very dumb program, All it does is take out an
inhibitor on `systemd` to prevent your device from going idle, suspending, or
handling its lid switch.

I wrote it to solve a problem that I had in a way that made sense to me.
I wanted to turn off a monitor connected to my laptop without it realising
it was no longer connected to it, and as such re-reading the lid switch and
suspending.

While I could just disable sleep altogether when connected to power I
figured it would be easier if I could control when sleep could happen and
when it couldn't.

My original solution was much more basic, just running `systemd-inhibit`
with a long running program inside it. First I tried using `yes` but the
constant flood of output to `stdout` completely consumes a single core of
my CPU. I then thought about just using any program but that's a waste of
resources.

As such I decided to learn the basics of `dBus` in C and wrote Dexedrine.

## Why the Name?
Dexedrine is named for a brand of dextroamphetamine originally from 1937.
Dextroamphetamine is a potent CNS stimulant inspiring, in my opinion, a
clever name.
