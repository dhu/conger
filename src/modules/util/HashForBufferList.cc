#include "HashForBufferList.h"

BOREALIS_NAMESPACE_BEGIN;

HashForBufferList::HashForBufferList()
{
  // key, value, hashfunc (opt), equality_pred (opt), allocator (opt)
  _hash_map = hash_map<char*, BufferList*, hash<const char*>, equal_string>();
}

BufferList* HashForBufferList::get_buffer_list(char *value)
{
  // BUG FIX (see HASHNUL in BSortQBox)
  //  we replace any NUL bytes in the group_by region with 0x7F
  // (more comments in addBufferList
  //  For this, this method now takes the size of the memory region
  // FINAL UPDATE - method mentioned before is no good (changing nuls to anything means
  //  you would confuse some numerical values with "nuls that changed"...
  // now value is ALREADY A NUL DELIMITED STRING. UNIQUE.

  /*
  char *value_nul_delim = new char[value_size + 1]; // to add the trailing null

  for (int i=0; i < value_size; i++)
  {
    if (value[i] == 0x0)
      value_nul_delim[i] = 0xFF;
    else
      value_nul_delim[i] = value[i];
  }
  value_nul_delim[value_size] = '\0';
  */

  //cout << " HashForBufferList::getBufferList("<<value<<") called"<<endl;
  _hash_iter = _hash_map.find(value);
  if (_hash_iter != _hash_map.end())
    return (*_hash_iter).second;
  else
    return NULL;
}
// WARNING: You may be tempted to make this function have another argument called "group_by"
//  to avoid the silly looking copy done below (the caller, BSortQBox::addToBuffer knows that value!)
//  BUT I think it has to do with the variable being just a temporary local variable at the caller... -eddie
void HashForBufferList::add_buffer_list(BufferList *list)
{
  int size = list->get_group_by_size();
  // BUG FIX (see HASHNUL in BSortQBox)
  //  we replace any NUL bytes in the group_by region with 0x01
  //  UPDATE: use 0x7F
  // ARG! Now, of course, a decimal 127 value in a tuple is equivalent to a decimal null
  //   meaning (127-x) hashes same as (0-x) NO GOOD
  //    what if I made then any 0x7F become NUL bytes? NO WAY that would just ignore the second value...
  // NOTE: see if there is any impact when using large numbers...
  //   Ok, 0xFF, all bits on (max of a signed integer) .. that would do right?
  //   now the only confusion is if a value has value MAX_INT
  // NEW BUG POSSIBLE: Well, what about the last null, that delimits the char*
  //  the way the hash_map will be expecting it? oh oh...
  //    this means you need to always ADD a trailing \0 whenever using it for the hash
  char* group_by = new char[size];
  memcpy(group_by, list->get_group_by_values(), size);
  //char *orig_group_by = list->getGroupByValues(); // number of bytes

  /*
  char *group_by = new char[size + 1]; // to add the trailing null

  for (int i=0; i < size; i++)
  {
    if (orig_group_by[i] == 0x0)
      group_by[i] = 0xFF;
    else
      group_by[i] = orig_group_by[i];
  }
  group_by[size] = '\0';
  */
  // IM NOT SURE YOU NEED TO MAKE A COPY OF GROUP_BY
  // since I believe I read that hash_map stores COPIES already
  // WHY NOT CHECK? - eddie
  _hash_map.insert(pair<char*, BufferList*> (group_by, list));

}

int HashForBufferList::get_hash_size()
{
  return _hash_map.size();
}

// See above why this takes a size param
int HashForBufferList::get_buffer_list_size(char *value)
{
  BufferList *temp = get_buffer_list(value);
  if (temp != NULL)
    return temp->size();
  else
    return 0;
}

bool HashForBufferList::is_empty()
{
  return _hash_map.empty();
}

BOREALIS_NAMESPACE_END;
