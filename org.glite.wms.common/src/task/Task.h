// File: Task.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id: Task.h,v 1.4 2005/06/10 12:58:17 monforte Exp $

#ifndef GLITE_WMS_COMMON_TASK_TASK_H
#define GLITE_WMS_COMMON_TASK_TASK_H

#include <queue>
#include <cassert>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace task {

// exceptions
struct Eof {};
struct SigPipe {};
struct Empty {};
struct Full {};

template<typename T, typename Q> class Pipe;

template<typename T, typename Q = std::queue<T> >
class PipeReadEnd {

  typedef Pipe<T, Q> parent_type;

  parent_type* m_parent;

public:
  PipeReadEnd(parent_type* parent = 0): m_parent(parent) {}

  T read() { return m_parent->read(); }
  T try_read() { return m_parent->try_read(); }
  void open() { m_parent->open_read_end(); }
  void close() { m_parent->close_read_end(); }
};

template<typename T, typename Q = std::queue<T> >
class PipeWriteEnd {

  typedef Pipe<T, Q> parent_type;

  parent_type* m_parent;

public:
  PipeWriteEnd(parent_type* parent = 0): m_parent(parent) {}

  void write(T obj) { m_parent->write(obj); }
  void try_write(T obj) { m_parent->try_write(obj); }
  void open() { m_parent->open_write_end(); }
  void close() { m_parent->close_write_end(); }
};

// when a pipe is created it is open both for reading and for writing,
// although there are no readers and writers at that moment
// a pipe end (read or write) becomes closed when all the threads who had
// previously opened it have then closed it
template<typename T, typename Q = std::queue<T> >
class Pipe: boost::noncopyable
{

  boost::mutex m_mutex;       // control any access to the pipe
  Q m_queue;
  size_t m_max_size;
  boost::condition m_not_full; // sync access to the queue
  boost::condition m_not_empty; // sync access to the queue

  int m_num_writers;
  int m_num_readers;

  bool m_write_end_is_closed;
  bool m_read_end_is_closed;

public:
  typedef PipeReadEnd<T, Q> read_end_type;
  typedef PipeWriteEnd<T, Q> write_end_type;

public:
  Pipe(size_t max_size = 10)
    : m_max_size(max_size), m_num_writers(0), m_num_readers(0),
      m_write_end_is_closed(false), m_read_end_is_closed(false)
  {
  }

  read_end_type read_end() { return read_end_type(this); }
  write_end_type write_end() { return write_end_type(this); }

  // before the first read is done m_num_writers > 0
  T read()
  {
    boost::mutex::scoped_lock lock(m_mutex);

    while (m_queue.empty()) {
      if (m_write_end_is_closed) { // see comment in close_write_end()
        throw Eof();
      }
      m_not_empty.wait(lock);
    }
    T result = m_queue.front();
    m_queue.pop();
    m_not_full.notify_one();

    return result;
  }

  T try_read()
  {
    boost::mutex::scoped_lock lock(m_mutex);

    if (m_queue.empty()) {
      if (m_write_end_is_closed) { // see comment in close_write_end()
        throw Eof();
      } else {
        throw Empty();
      }
    }

    T result = m_queue.front();
    m_queue.pop();
    m_not_full.notify_one();

    return result;
  }

  void write(const T& obj)
  {
    boost::mutex::scoped_lock lock(m_mutex);

    if (m_read_end_is_closed) {
      throw SigPipe();
    }

    while (m_queue.size() == m_max_size) {
      m_not_full.wait(lock);
      if (m_read_end_is_closed) { // see comment in close_read()
        throw SigPipe();
      }
    }
    m_queue.push(obj);
    m_not_empty.notify_one();
  }

  void try_write(const T& obj)
  {
    boost::mutex::scoped_lock lock(m_mutex);

    if (m_read_end_is_closed) {
      throw SigPipe();
    } else if (m_queue.size() == m_max_size) {
      throw Full();
    }

    m_queue.push(obj);
    m_not_empty.notify_one();
  }

  bool empty()
  {
    boost::mutex::scoped_lock lock(m_mutex);

    return m_queue.empty();
  }

  void open_read_end(int num = 1)
  {
    boost::mutex::scoped_lock lock(m_mutex);

    m_num_readers += num;
  }

  void open_write_end(int num = 1)
  {
    boost::mutex::scoped_lock lock(m_mutex);

    m_num_writers += num;
  }

  void close_read_end()
  {
    boost::mutex::scoped_lock lock(m_mutex);

    --m_num_readers;
    if (m_num_readers == 0) {
      // be careful when writing... (see comment in write())
      m_read_end_is_closed = true;
      // wake up all writers so that they can realize that they will never
      // be able to push data again into the pipe
      m_not_full.notify_all();
    }
  }

  void close_write_end()
  {
    boost::mutex::scoped_lock lock(m_mutex);

    --m_num_writers;
    if (m_num_writers == 0) {
      // be careful when reading... (see comment in read())
      m_write_end_is_closed = true;
      // wake up all readers so that they can realize that no other data
      // will ever appear on the pipe
      m_not_empty.notify_all();
    }
  }

};

template<typename T>
void close_pipe_end(T* t)
{
  if (t) {
    t->close();
  }
}

template<typename T_IN, typename Q = std::queue<T_IN> >
class PipeReader
{
  typedef PipeReadEnd<T_IN, Q> read_end_type;

  boost::shared_ptr<read_end_type> m_read_end;

public:
  void read_from(read_end_type const& from)
  {
    m_read_end.reset(new read_end_type(from), close_pipe_end<read_end_type>);
    m_read_end->open();
  }

protected:
  read_end_type& read_end() const { return *m_read_end; }
  ~PipeReader() {}
};

template<typename T_OUT, typename Q = std::queue<T_OUT> >
class PipeWriter
{
  typedef PipeWriteEnd<T_OUT, Q> write_end_type;

  boost::shared_ptr<write_end_type> m_write_end;
public:
  void write_to(write_end_type const& to)
  {
    m_write_end.reset(new write_end_type(to), close_pipe_end<write_end_type>);
    m_write_end->open();
  }

protected:
  write_end_type& write_end() const { return *m_write_end; }
  ~PipeWriter() {}
};

template<
  typename T_IN,
  typename T_OUT,
  typename Q_IN = std::queue<T_IN>,
  typename Q_OUT = std::queue<T_OUT>
>
class PipeForwarder
{
  typedef PipeReadEnd<T_IN, Q_IN> read_end_type;
  typedef PipeWriteEnd<T_OUT, Q_OUT> write_end_type;

  boost::shared_ptr<read_end_type> m_read_end;
  boost::shared_ptr<write_end_type> m_write_end;

public:
  void read_from(read_end_type const& from)
  {
    m_read_end.reset(new read_end_type(from), close_pipe_end<read_end_type>);
    m_read_end->open();
  }
  void write_to(write_end_type const& to)
  {
    m_write_end.reset(new write_end_type(to), close_pipe_end<write_end_type>);
    m_write_end->open();
  }

protected:
  read_end_type& read_end() const { return *m_read_end; }
  write_end_type& write_end() const { return *m_write_end; }
  ~PipeForwarder() {}
};

template<typename R, typename T, typename Q>
void init_task(
  R r,
  Pipe<T, Q>& pipe,
  int num_threads,
  boost::thread_group& g,
  PipeWriter<T, Q> const&
)
{
  r.write_to(pipe.write_end());
  while (num_threads--) {
    g.create_thread(r);
  }
}

template<typename R, typename T, typename Q>
void init_task(
  R r,
  Pipe<T, Q>& pipe,
  int num_threads,
  boost::thread_group& g,
  PipeReader<T, Q> const&
)
{
  r.read_from(pipe.read_end());
  while (num_threads--) {
    g.create_thread(r);
  }
}

class Task: boost::noncopyable
{
  boost::thread_group m_group;

public:

  Task(boost::function<void()> const& fun, int num_threads = 1)
  {
    assert(num_threads > 0);

    while (num_threads--) {
      m_group.create_thread(fun);
    }
  }

  template<typename R, typename T, typename Q>
  Task(R const& r, Pipe<T, Q>& pipe, int num_threads = 1)
  {
    assert(num_threads > 0);

    init_task(r, pipe, num_threads, m_group, r);
  }

  template<typename R, typename T_IN, typename T_OUT, typename Q_IN, typename Q_OUT>
  Task(
    R r,
    Pipe<T_IN, Q_IN>& pipe_in,
    Pipe<T_OUT, Q_OUT>& pipe_out,
    int num_threads = 1
  )
  {
    assert(num_threads > 0);

    r.read_from(pipe_in.read_end());
    r.write_to(pipe_out.write_end());
    while (num_threads--) {
      m_group.create_thread(r);
    }
  }

  ~Task()
  {
    m_group.join_all();
  }

};

}}}} // glite::wms::common::task

#endif // GLITE_WMS_COMMON_TASK_TASK_H
