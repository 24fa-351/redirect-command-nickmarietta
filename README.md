# 351-redirect
This project is based with the intended use of:

```bash
./redirect <input file> <command> <command arguments> <output file>
```

For Example:
```bash
./redirect input.txt wc output.txt
```
or
```bash
./redirect input.txt ls output.txt
```

## Sample Output!
**input.txt** contains: 
```bash
hello i am nick
i am a student
i like dogs
```

*./redirect input.txt "ls" output.txt*

**output.txt**:
```bash
README.md
clang-format
helperfunctions.c
input.txt
output.txt
redirect
redirect.c
redirectNotes
redirectNotes.c
run.sh
```

**OR**
*./redirect input.txt "wc" output.txt*

**output.txt**:
```bash
 2 11 42
```

