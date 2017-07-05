# Worklog 

Worklog is a small console utility that allows you to write down the work you did. The purpose of it is that you can manage your free time projects.

Example

```bash
 worklog list
1         2017-05-20  Virtual Machine in C#           [csharp, virtualmachine]
2         2017-05-20  Laravel ToDo List App           [laravel, php]
3         2017-05-20  Hex Dump in assembler           [assembler]
```

## Features

The following commands are supported:

```bash
Available commands:
  broken              lists all invalid logs
  edit                edit a work log. An additional id parameter is required.
  help                shows this help
  init                initializes a worklog space
  list                lists all logs
  new                 add a new work log
  rep                 repeats a command. Example: ./tool rep 1,3,7 view ["separator string"] (shows 1, 3 & 7 in a loop)
  rm                  removes a work log. An additional id parameter is required.
  search              search the work logs by a filter: tag:php -tag:javascript
  tag                 add, remove or list tags
  view                view a work log. An additional id parameter is required.
  yearly              shows a breakdown report by year
```

## Requirements

* [bazel build system](http://bazel.build)
* Clang version 4.0.1
* OSX (I have compiled & tested it under OSX but it also should work under any linux system)

## How to compile

In order to compile it execute the following command: 

```
$ bazel build :worklog
```
(and you will find the binary in ```./bazel-bin/worklog```.


## Demo

### Initializing a worklog repo:

First we need to initialize a worklog repository (sort of like git works):

```bash
$ worklog init
```
This will create a .worklog directory with all the meta data needed. The logs are stored in .worklog/logs/*

### Adding a worklog entry:

Type in the following:

```bash
$ worklog new
```

It will automatically launch your default editor ($EDITOR) and feeds it with a template, like:

```bash
date=2017-06-24
tags=tag1, tag2

Title here

Description here
```

It fills in the date automatically but you must now edit the tags, title and description. 

__NOTE__: The empty line between header area and title, and between title and description is important.

(for demo purposes I have added a few valid and invalid work log entries)

### Listing the work logs:

```bash
$ worklog list
7         2017-06-24  Title here                      [tag1, tag2]
2         2017-05-20  Laravel ToDo List App           [laravel, php]
3         2017-05-20  Hex Dump in assembler           [assembler]
1         2014-05-20  Virtual Machine in C#           [csharp, virtualmachine]
```

### Listing the work logs by year:

```bash
$ worklog yearly
2017:
7         2017-06-24  Title here                      [tag1, tag2]
2         2017-05-20  Laravel ToDo List App           [laravel, php]
3         2017-05-20  Hex Dump in assembler           [assembler]

2014:
1         2014-05-20  Virtual Machine in C#           [csharp, virtualmachine]
```

### Listing the broken work logs:

It might be possible that you save a invalid work log by accident (and they don't appear when entering 'worklog list'). In order to list them type the following:

```bash
$ worklog broken
4         1970-01-01  broken                          []
5         1970-01-01  broken version 2                []
```
Based on the output you can see that the date is wrong. In order to fix that we can type in ```worklog edit 4``` and ```worklog edit 5```.

### Searching the work logs:

Work log provides functionality to search all the work logs by the tag and also title. Example:

```bash
$ worklog search tag:php
2         2017-05-20  Laravel ToDo List App           [laravel, php]
```

For more information please check out ```worklog help```.

## Notes on this version

In my initial version I had the boost dependencies included in third_party/boost/* but Github doesn't allow it to push such a big repository. For that reason I have changed all the bazel dependencies & updates the references to ```https://github.com/nelhage/rules_boost```. The git repository containing all the dependencies are stored in BitBucket (https://bitbucket.org/andin/cpp-worklog). If you want access to it then please drop me an email.


## Development status

It was a free time project in order to learn C++ so I don't know if I will continue to develop on worklog further.


## License

*The entire C++ code is licensed under MIT License (worklog only, **NOT** the third party libraries, utils, tools, etc.):*

Copyright (c) 2017 Andreas Näpflin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.



