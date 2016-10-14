// Copyright (c) 2016 IBM Corporation.
#ifndef _STRING_EXTERN
#define _STRING_EXTERN

// Forward declarations

namespace tosca {
    class StringTerm;
    class Context;
    class DoubleTerm;
}

class Bool;


/* @return the string after the first occurrence of the given first string, or
           the empty string if there is no such occurrences */
extern tosca::StringTerm& AfterFirst(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&);

/* @return the string before the first occurrence of the given first string, or
            the original string if there is no such occurrences */
extern tosca::StringTerm& BeforeFirst(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&);

/* @return `TRUE` if and only if the given strings are equals. */
extern Bool& StringEqual(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&);

/* @return the concatenation of the two given string */
extern tosca::StringTerm& ConcatString(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&);

extern tosca::StringTerm& Escape(tosca::Context&, tosca::StringTerm&);

extern tosca::DoubleTerm& Length(tosca::Context&, tosca::StringTerm&);

extern tosca::StringTerm& Mangle(tosca::Context&, tosca::StringTerm&);

/* Converts all of the characters of the given string to upper case */
extern tosca::StringTerm& UpCase(tosca::Context&, tosca::StringTerm&);

/* Converts all of the characters of the given string to lower case */
extern tosca::StringTerm& DownCase(tosca::Context&, tosca::StringTerm&);

extern tosca::StringTerm& Replace(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&, tosca::StringTerm&);

extern Bool& Contains(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&);

extern tosca::StringTerm& Substring(tosca::Context&, tosca::StringTerm&, tosca::DoubleTerm&, tosca::DoubleTerm&);

extern tosca::StringTerm& Substring2(tosca::Context&, tosca::StringTerm&, tosca::DoubleTerm&);

extern Bool& MatchRegex(tosca::Context&, tosca::StringTerm&, tosca::StringTerm&);

/* Tests if the given string starts with the specified prefix. */
extern Bool& StartsWith(tosca::Context&, tosca::StringTerm& str, tosca::StringTerm& prefix);

/* Tests if the given string ends with the specified suffix. */
extern Bool& EndsWith(tosca::Context&, tosca::StringTerm& str, tosca::StringTerm& suffix);

#endif
