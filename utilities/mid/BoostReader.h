// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALO_MID_BOOSTREADER_H
#define ALO_MID_BOOSTREADER_H

#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace alo
{
namespace mid
{
class BoostReader
{
 public:
  BoostReader(const char* filename);

  template <class T>
  bool read(T& obj)
  {
    if (eof()) {
      return false;
    }
    mInArchive >> obj;
    return true;
  }

  template <class T>
  bool read(std::vector<T>& vec, int nObjects)
  {
    if (eof()) {
      return false;
    }

    T obj;
    while (read(obj)) {
      vec.emplace_back(obj);
      if (nObjects > 0 && vec.size() >= nObjects) {
        break;
      }
    }
    return true;
  }

 private:
  bool eof(); // is end of file

  std::ifstream mInFile;                      /// Input file
  std::streampos mStreamEnd;                  /// File end position
  boost::archive::binary_iarchive mInArchive; /// Input archive
};

} // namespace mid
} // namespace alo

#endif
