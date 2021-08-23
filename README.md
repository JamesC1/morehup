# morehup

`morehup` is intended to be kind-of the opposite of nohup.

In a terminal context, nohup protects a background process, 
so that it is not killed by `SIGHUP` when the controlling terminal is closed.

Morehup arranges so that the process receives a signal 
(usually one that will kill it) when there is I/O available 
on a particular file descriptor.

With a little bit of arranging, in a bash-like shell, 
that can be when the shell exits. 
A sample usage (taking transfer of shell variables between shells as magically given) is:

    parent> TMPDIR="$(mktemp -d)"
    parent> mkfifo $TMPDIR/fifo
    parent> exec 7<>$TMPDIR/fifo
    # must be both input and output, or the process stalls
    
    child> ./morehup 8<$TMPDIR/fifo -f 8 /bin/sh
    # you get a shell, probably with a different looking prompt
    # in which to do whatever you wanted
    
    parent> exec 7<&-
    
    child>  # receives SIGHUP; handles it as desired, or terminates

This draft is very specific to linux. 
People who have access to other other operating systems are invited to 
submit extensions.

People who know that there is a standard way of doing this, 
that I've missed,
are invited to submit links to it.

(With thanks to the providers of the markdown viewer:
<https://codebeautify.org/markdown-viewer>)
