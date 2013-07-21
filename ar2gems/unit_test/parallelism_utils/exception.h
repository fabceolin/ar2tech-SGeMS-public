#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <string>


class Exception
{
public:
  std::string what() const { return what_; }
  virtual ~Exception() {}

protected:
  Exception( std::string _what ) : what_("[EXCEPTION] " + _what) {}

private:
  std::string what_;
};

class InternalException : public Exception {
public:
   InternalException( std::string what ) : Exception(what) {}
};

class RangeException : public Exception {
public:
   RangeException( std::string what ) : Exception(what) {}
};

class MemoryException : public Exception {
 public:
   MemoryException( std::string what ) : Exception(what) {}
};

class PermissionException : public Exception {
public:
   PermissionException( std::string what ) : Exception( what ) {}
};



#endif
