#ifndef HELPERS_H
#define HELPERS_H
/////////////////////////////////////////////////////////////////////
// Helpers.h - Utilities for STL container demos                   //
//                                                                 //
// Application: CSE687 - Object Oriented Design C++11 demos        //
// Platform:    Dell XPS 2720 Touch, Win 8.1, Visual Studio 2013   //
// Author:      Jim Fawcett, CST 4-187, Syracuse Univesity         //
//              jfawcett@twcny.rr.com                              //
//              Source from Dr. Jim                                //
/////////////////////////////////////////////////////////////////////
/*
 *  Package Operations
 * --------------------
 *  This package provides two classes: ContainerHelpers and Utilties.
 *  - Containerhelpers: provides functions to display the contents of
 *    sequence and key-value pair containers.
 *  - Utilities: provides a single function title that writes an
 *    underlined string to the console.
 *
 *  Required Files:
 * -----------------
 * - Helpers.h, Helpers.cpp
 *
 *  Build Command:
 * ----------------
 *    devenv Project3.sln /rebuild debug
 *
 *  Maintenance History:
 * ----------------------
 *  Ver 1.1 : Mar 30 16
 *  modify the operator() to display type name
 *  file name and namespace
 *  Ver 1.0 : 8 Jan 14
 *  - first release
 */
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <utility>

namespace UtilityHelpers
{

  ///////////////////////////////////////////////////////////////////////////
  // ContainerHelpers - Display sequence container contents
  // - demonstrates use of range-for, functor, and lambda
  // - all three methods are equivalent

  class ContainerHelpers
  {
  public:

    /////////////////////////////////////////////////////////////////////////
    // formatting helper for show
    // - define delimiter of strings ("\""), chars ("\'"), and everything else ("")
    // - see above for usage

    template<typename T>
    static std::string format()
    {
      std::string delimiter;  // each item of output is wrapped in delimiters, fore and aft

      if (typeid(T) == typeid(std::string))
      {
        delimiter = "\"";
      }
      else if (typeid(T) == typeid(char))
      {
        delimiter = "\'";
      }
      else
      {
        delimiter = "";
      }
      return delimiter; 
    }
    //----< displays a single formatted container element >------------------

    template<typename C>
    struct ShowItem
    {
      void operator()(typename const C::value_type& v) const // operator() makes functor
      {
        std::string delimiter = format<C::value_type>();
        std::cout << delimiter << v << delimiter;
      }
    };
    //----< displays a single formatted container element >------------------

    template<typename C>
    struct ShowPtrItem
    {
      void operator()(typename const C::value_type& v) const // operator() makes functor
      {
        std::string delimiter = format<C::value_type>();
        std::cout << delimiter << *v << delimiter;
      }
    };
    //----< displays a formatted key-value pair >----------------------------

    template<typename C>
    struct ShowPair
    {
      void operator()(const std::pair<typename C::key_type, typename C::mapped_type>& p) const
      {
        std::string key_delimiter = format<C::key_type>();
        std::string value_delimiter = format<C::value_type>();
		std::string eleType = p.second->type;
		std::string elefileName = p.second->fileName;
        std::cout << p.second->startLineCount << "{ typeName:" << key_delimiter << p.first << key_delimiter;
		std::cout << ", fileName: " << value_delimiter << elefileName << value_delimiter;
		std::cout << ", Type: " << value_delimiter << eleType << value_delimiter << " }";
      }
    };
    //----< displays container items, using the formatter class F >----------

    template<typename C, typename F>
    static void show(const C& c, F& f, bool vertical = false)
    {
      bool first = true;
      for (auto e : c)
      {
        if (first)
        {
          first = vertical;
          std::cout << "\n  ";
        }
        else
        {
          std::cout << ", ";
        }
        f(e);
      }
      std::cout << "\n";
    }

  public:

    //----< displays items from a seqence container >------------------------

    template<typename C>
    static void showItems(const C& c, bool vertical = false)
    {
      show(c, ShowItem<C>(), vertical);
    }
    //----< displays items from a key-value container >----------------------

    template <typename C>
    static void showPairs(const C& c, bool vertical = true)
    {
      show(c, ShowPair<C>(), vertical);
    }
  };

  class Utilities
  {
  public:

    ///////////////////////////////////////////////////////////////////////
    // display title with underscore

    static void title(const std::string& title, char underscore = '-')
    {
      std::string tempStr = "\n  " + title + "\n " + std::string(title.size() + 2, underscore);
      std::cout << tempStr;
      if (underscore == '=')
        std::cout << "\n";
    }
  };
}

#endif
