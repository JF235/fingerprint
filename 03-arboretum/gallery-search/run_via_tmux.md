# Run via tmux

1. Create tmux session

```
$ tmux new -s <session_name>
```

2. Run commands inside tmux

```
session>$ make
session>$ make run > output.txt &
```

3. Detach from tmux

```
session>$ tmux detach
```

or `Ctrl+b`, `d`

4. List sessions

```
$ tmux ls
```

5. Attach to session

Returning to last session

```
$ tmux a -s <session_name>
```

6. Kill session

```
$ tmux kill-session -t <session_name>
```
