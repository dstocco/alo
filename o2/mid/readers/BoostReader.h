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

namespace alo
{
namespace mid
{
class BoostReader
{
  public:
    BoostReader(const char *filename);

    template <class T>
    std::vector<T> run(int nObjects)
    {
        std::vector<T> vec;
        T obj;
        int iobj = 0;
        while (mInFile.tellg() < mStreamEnd && ++iobj <= nObjects)
        {
            mInArchive >> obj;
            vec.emplace_back(obj);
        }
        return vec;
    }

  private:
    std::ifstream mInFile;                      /// Input file
    std::streampos mStreamEnd;                  /// File end position
    boost::archive::binary_iarchive mInArchive; /// Input archive
};

} // namespace mid
} // namespace alo

#endif
