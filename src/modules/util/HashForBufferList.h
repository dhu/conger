#ifndef HASH_FOR_BUFFER_LIST_H
#define HASH_FOR_BUFFER_LIST_H

// DEPRECATED!! INCLUDE as I did below ...
//#include <hash_map.h>
#include <algorithm>
#include <ext/hash_map>

#include "BufferList.h"

BOREALIS_NAMESPACE_BEGIN;

using __gnu_cxx::hash;
using __gnu_cxx::hash_map;

/*
struct buffer_hash
{
  size_t operator()(key_type* s) const
  {
    unsigned long h = 0;
    for (int i = 0; i < s->key_size; i++)
      h = 5*h + s->key[i];
    return size_t(h);
  }
};
*/

struct equal_string
{
  bool operator()(const char *s1, const char *s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};


class HashForBufferList
{
public:
  HashForBufferList();
  ~HashForBufferList() {};

  BufferList* get_buffer_list(char *value);

  void add_buffer_list(BufferList *list);
  int get_hash_size();
  int get_buffer_list_size(char *value);
  bool is_empty();

  //***************************
  //is this method even necessary?
  //what arguments??????????????????
  void remove_buffer_list();



private:

  hash_map<char*, BufferList*, hash<const char*>, equal_string>      _hash_map;
  hash_map<char*, BufferList*, hash<const char*>, equal_string>::iterator   _hash_iter;

};

BOREALIS_NAMESPACE_END;

#endif
