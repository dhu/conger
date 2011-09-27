
#ifndef MEDUSA_NAME_H
#define MEDUSA_NAME_H

#include <string>

#include <NMSTL/debug>
#include <NMSTL/serial>
#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

class Name
{

 private:
    string _name;

 public:
    Name() {}
    Name(const char *name) { _name = string(name); }
    Name(string name) { _name = name; }

    operator const void* () const
    {
        return _name.length() ? this : 0;
    }

    string as_string() const
    {
        return _name;
    }

    bool operator != (const Name& other) const
    {
        return _name != other._name;
    }

    bool operator == (const Name& other) const
    {
        return _name == other._name;
    }

    bool operator < (const Name& other) const
    {
        return _name.compare(other._name) < 0;
    }

    NMSTL_SIMPLY_SERIALIZABLE(Name, << _name);
};
NMSTL_TO_STRING(Name);

BOREALIS_NAMESPACE_END;

#endif


/* class Name { */
/*     string host; */
/*     string path; */

/*     static string eliminate_leading_slashes(string path) { */
/*         unsigned int s = path.find_first_not_of('/'); */
/*         if (s == string::npos) */
/*             return path; */
/*         return string(path, s); */
/*     } */

/*     void parse_url(string url) {         */

/*         if (url.length() <= 0) */
/*             return; */

/*         if (url.find_first_of(':') != string::npos) { */
/*             if (url.substr(0, 9) != "medusa://") { */
/*                 FATAL << "Will be calling anon because of URL[" << url << "]"; */
/*                 *this = anon("", "bad-name"); */
/*                 return; */
/*             } */
/*             url.erase(0, 9); */
/*         } */

/*         unsigned int slash = url.find_first_of('/'); */
/*         if (slash == string::npos) { */
/*             host = url; */
/*         } else { */
/*             host = url.substr(0, slash); */
/*             path = url.substr(slash + 1); */
/*         } */
/*         return; */
/*     } */

/*   public: */
/*     Name() {} */
/*     Name(const char *url) { parse_url(url); } */
/*     Name(string url) { parse_url(url); } */

/*     Name(string host, string path) : */
/*         host(host), path(eliminate_leading_slashes(path)) */
/*     { */
/*     } */

/*     string get_host() const { return host; } */
/*     string get_path() const { return path; } */

/*     operator const void* () const { */
/*         return host.length() || path.length() ? this : 0; */
/*     } */

/*     string as_string() const { */
/*         if (!*this) { */
/*             //return "(none)"; */
/*             return ""; */
/*         } */

/*         return "medusa://" + host + "/" + path; */
/*     } */

/*     Name plus_path_element(string new_path) const { */
/*         return Name(host, path + "/" + new_path); */
/*     } */

/*     static Name anon(string host, string tag) { */

/*         FATAL << "*** Anon was called! ***"; */

/*         static map<string, int> counts; */

/*         int count = ++counts[tag]; */

/*         if (tag.length()) */
/*             return Name(host, "anon-" + tag + "-" + to_string(count)); */
/*         else */
/*             return Name(host, "anon-" + to_string(count)); */
/*     } */

/*     bool operator != (const Name& other) const { */
/*         return host != other.host || path != other.path; */
/*     } */

/*     bool operator == (const Name& other) const { */
/*         return host == other.host && path == other.path; */
/*     } */

/*     bool operator < (const Name& other) const { */
/*         int cmp = host.compare(other.host); */
/*         if (cmp == 0) cmp = path.compare(other.path); */
/*         return cmp < 0; */
/*     } */

/*     NMSTL_SIMPLY_SERIALIZABLE(Name, << host << path ); */
/* }; */
/* NMSTL_TO_STRING(Name); */

/* MEDUSA_NAMESPACE_END; */

/* #endif */
