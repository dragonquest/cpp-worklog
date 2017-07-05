#ifndef ATL_STREAM_H_
#define ATL_STREAM_H_

#include <cstdio>
#include <iostream>
#include <streambuf>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

// File helpful wrapper libraries for wrapping FILE*, pipe fds etc.
// with c++ streams.

namespace atl {

// (C) Copyright Nicolai M. Josuttis 2001.
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
class Fdoutbuf : public std::streambuf {
 protected:
  int fd;
 public:
  Fdoutbuf(int _fd) : fd(_fd) {}

 protected:
  // write one character
  virtual int_type overflow(int_type c) {
    if (c != EOF) {
      char z = c;
      if (write(fd, &z, 1) != 1) {
        return EOF;
      }
    }
    return c;
  }

  // write multiple characters
  virtual std::streamsize xsputn(const char* s, std::streamsize num) {
    return write(fd, s, num);
  }
};

class Fdostream : public std::ostream {
 protected:
  Fdoutbuf buf;

 public:
  Fdostream(int fd) : std::ostream(0), buf(fd) { rdbuf(&buf); }
};


// (C) Copyright Nicolai M. Josuttis 2001.
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
class Fdinbuf : public std::streambuf {
  protected:
    int fd;    // file descriptor
  protected:
    /* data buffer:
     * - at most, pbSize characters in putback area plus
     * - at most, bufSize characters in ordinary read buffer
     */
    static const int pbSize = 4;        // size of putback area
    static const int bufSize = 1024;    // size of the data buffer
    char buffer[bufSize+pbSize];        // data buffer

  public:
    /* constructor
     * - initialize file descriptor
     * - initialize empty data buffer
     * - no putback area
     * => force underflow()
     */
    Fdinbuf (int _fd) : fd(_fd) {
        setg (buffer+pbSize,     // beginning of putback area
              buffer+pbSize,     // read position
              buffer+pbSize);    // end position
    }

  protected:
    // insert new characters into the buffer
    virtual int_type underflow () {
#ifndef _MSC_VER
using std::memcpy;
#endif

        // is read position before end of buffer?
        if (gptr() < egptr()) {
            return *gptr();
        }

        /* process size of putback area
         * - use number of characters read
         * - but at most size of putback area
         */
        int numPutback;
        numPutback = gptr() - eback();
        if (numPutback > pbSize) {
            numPutback = pbSize;
        }

        /* copy up to pbSize characters previously read into
         * the putback area
         */
        memcpy (buffer+(pbSize-numPutback), gptr()-numPutback,
                numPutback);

        // read at most bufSize new characters
        int num;
        num = read (fd, buffer+pbSize, bufSize);
        if (num <= 0) {
            // ERROR or EOF
            return EOF;
        }

        // reset buffer pointers
        setg (buffer+(pbSize-numPutback),   // beginning of putback area
              buffer+pbSize,                // read position
              buffer+pbSize+num);           // end of buffer

        // return next character
        return *gptr();
    }
};

class Fdistream : public std::istream {
  protected:
    Fdinbuf buf;
  public:
    Fdistream (int fd) : std::istream(0), buf(fd) {
        rdbuf(&buf);
    }
};

// Example usage:
//
//    int my_pipe[2];
//    if(pipe(my_pipe) == -1)
//    {
//        fprintf(stderr, "Error creating pipe\n");
//    }
//
//    pid_t child_id;
//    child_id = fork();
//    if(child_id == -1)
//    {
//        fprintf(stderr, "Fork error\n");
//    }
//    if(child_id == 0) // child process
//    {
//        close(my_pipe[0]); // child doesn't read
//        dup2(my_pipe[1], 1); // redirect stdout
//
//				atl::Fdostream out(my_pipe[1]);
//				out << "31 hexadec: " << std::hex << 31 << "\n";
//
//    }
//    else
//    {
//        close(my_pipe[1]); // parent doesn't write
//
//				atl::Fdistream in(my_pipe[0]);
//
//        char c;
//        while (in.get(c)) {
//          std::cout << c;
//        }
//
//
//        close(my_pipe[0]);
//    }


} // namespace atl

#endif  // ATL_STREAM_H_
