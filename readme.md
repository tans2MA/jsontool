# Json Tool in Command Line

A command line tool to validate json format including schema, view inner node
by json pointer path, comapre tow json files or tow json node in the same or
different json file, or compare multiple json files from different dirctory.

## Dependents

* [CLI11](https://github.com/CLIUtils/CLI11)  command line option lib.
* [rapidjson](https://github.com/Tencent/rapidjson/) json lib.

They are header-only libs, can simplely copy into `include/` directory when
build.

## Usage

Can run in linux / windows command line, quick start with `-h` or `--help`.

```bash
./jsontool.exe -h
./jsontool.exe [OPTIONS] [file] [extraArgs...]
cat file.json | ./jsontool.exe [OPTIONS]
./jsontool.exe [OPTIONS] < file.json
```

The `file` name not necessary end with `.json`, and actually can read from
`stdin` if not provide any argument.

### Check Json Format -c

```bash
./jsontool.exe -c file.json file2.json ...
./jsontool.exe -c *.json
```

Will check if one or more files in valid json. When read from `stdin` (use `|` or `<`)
can only accept a single json as whole.

Can also provide `-s` option to specify a schema file, then will validate the
json content as well. Note that the schema is applied for all json file
arguments, if the json files is following different schema, should invoke
command separately.

```bash
./jsontool.exe -c -s schema.json file.json file2.json ...
```

The options can be combind to `-cs schema.json` as `-c` has no followed
argument.

### Print View Json and Reformat -p -l

```bash
./jsontool.exe -p file.json
./jsontool.exe -l file.json
./jsontool.exe -p -o file-preety.json file.json
```

Option `-p` will print json in pretty format, while `-l` print in condensed
single line, to `stdout` by default, but can specify output file name with `-o`.
So it can used as normalized reformator for json.

If json is large, can only print out a part of it by json pointer path, using
`-t` option.

```bash
./jsontool.exe -p -t /root/path/to/some/node file.json
./jsontool.exe -p 'file.json#/root/path/to/some/node'
```

The pointer part can be joined after file name with `#` as a single argument,
but then it is better to quote the joined argument because in bash `#` may be
interpreted as comment. Can also use `:` or `?` instead of `#` to avoid quote by
convinience, but that is not standard notation.

It make no sense to print different json file together, so should not provide
extra positional arguemnts, but if provided, they are interpreted as different
path point to the same current json, and print out one after another. 

### Comapre Json Files -d

There are tow pairs of options to specify which tow file to be compared, they
are equivalent. Note `--f1` and `--f2` are prefixed with tow dashed `--`.

```bash
./jsontoo.exe -d -f file.json -F bench-file.json
./jsontoo.exe -d --f1 file1.json --f2 file2.json
```

The option `-f` or `--f1` can also be omitted, as the primary json can be from
the first positional argument or `stdin`.

```bash
./jsontoo.exe -d -F bench-file.json file.json
cat file.json | ./jsontoo.exe -d -F bench-file.json
```

#### Compare Json Files within Directory

Can compare the json file with same name from tow different directories, by
`-i/-I` or `--i1/--i2` options. The remaining arguments specify which file
names to be compared, or can read a file name list from `stdin`.

```bash
./jsontool.exe -d -i this-dir/ -I that-dir/ file1 file2 ...
./jsontool.exe -d -i this-dir/ -I that-dir/ this-dir/*.json
ls this-dir/*.json | ./jsontool.exe -d -i this-dir/ -I that-dir/
cat filelist.txt | ./jsontool.exe -d --i1 this-dir/ --i2 that-dir/
```

#### Comapre Json Node

Use `-t/-T` or `--t1/--t2` to specify which nodes to compare. If not specify
the second json file with `-F/--f2`, then the path point to the same json
specified by `-f/--f1` or first positional argument, otherwise the tow pointer
path bound to corresponding json respecitvely.

```bash
./jsontool.exe -d -t /root/to/one -T /root/to/another file.json
./jsontool.exe -d --f1 file1.json --t1 /root/to/path1 --f2 file2.json --t2 /root/to/path2
./jsontool.exe -d --f1 'file1.json#/root/to/path1' --f2 'file2.json#/root/to/path2'
```

Again, the json file name and pointer path can be joined to one argument.

And it is also make sense to combind option `-t/-T` and `-i/-I`.

#### Comapre by Arguments but no Options

If there is no more options above(`-FIT`) beside `-d`, can also simplely
determine what to compare by positional arguments.

```bash
./jsontool.exe -d file1.json file2.json
./jsontool.exe -d file.json /pointer/path/1 /pointer/path/2
```

Tow arguments means compare two json files(can subfiexed with pointer), Three
argument means compare tow node within the same json file.

#### Compare Methods

Normally compare json by equality, which means object have exactlly the same
keys and value, and array have the same length with same value.

But sometimes it is also acceptable when one json is supper set of another,
with more object keys that ensure back compability. So there is variant
compare methods option along with `-d`.

* `--eq` means `==` , this is default method;
* `--le` means `<=` , json1 may be sub set of json2;
* `--ge` means `>=` , json1 may be supper-set of json2.

```bash
./jsontool.exe -d --f1 file1.json --le --f2 file2.json
./jsontool.exe -d --f2 file2.json --ge --f1 file1.json
```

Notice that option order is not important.

## Output and Exit Code

When `jsontool.exe` running, it will print some information to `stderr` to
tell what happened.  If all actions are sucessfully fininshed, exit with `0`,
otherwise exit with negative number, which absolute value means the count of
fails(check format or compare value).

But of course `-p/-l` will print to `stdout` as required.
