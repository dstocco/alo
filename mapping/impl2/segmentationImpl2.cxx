//
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @author  Laurent Aphecetche

#include "segmentationImpl2.h"
#include "padGroup.h"
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include "polygon.h"
#include "contourCreator.h"
#include "padSize.h"
#include "boost/format.hpp"
#include "svgContour.h"
#include "polygon.h"
#include "padGroupTypeContour.h"

using namespace o2::mch::mapping::impl2;
using namespace o2::mch::contour;

namespace o2 {
namespace mch {
namespace mapping {
namespace impl2 {


std::vector<Segmentation::Contour> computeContours(const std::vector<PadGroup> &padGroups,
                                                   const std::vector<PadGroupType> &padGroupTypes,
                                                   const std::vector<std::pair<float, float>> &padSizes)
{
  //std::cout << boost::format("computeContours %3d padgroups %2d padgrouptypes %2d padsizes\n")
  //             % padGroups.size() % padGroupTypes.size() % padSizes.size();

  std::vector<o2::mch::contour::Polygon<int>> pgtContours = computeContours(padGroupTypes);

  std::vector<Segmentation::Contour> contours;

  for (auto &pg: padGroups) {
    auto p = pgtContours[pg.mPadGroupTypeId].convert(1.0);
    float dx{padSizes[pg.mPadSizeId].first};
    float dy{padSizes[pg.mPadSizeId].second};
    p.scale(dx, dy);
    p.translate(pg.mX, pg.mY);
    contours.push_back(p);
  }

  return contours;
}
using PadGroupFunc = int (*)(const PadGroup &);

int fecId(const PadGroup &padGroup)
{ return padGroup.mFECId; }

int groupTypeId(const PadGroup &padGroup)
{ return padGroup.mPadGroupTypeId; }

int padSizeId(const PadGroup &padGroup)
{
  return padGroup.mPadSizeId;
}

std::set<int> getUnique(const std::vector<PadGroup> &padGroups, PadGroupFunc func)
{
  // extract from padGroup vector the unique integer values given by func
  std::set<int> u;
  for (auto &pg: padGroups) {
    u.insert(func(pg));
  }
  return u;
}

std::vector<PadGroupType> getPadGroupTypes(const std::vector<PadGroup> &padGroups)
{
  // get a vector of unique padGroupTypes from padGroups
  std::vector<PadGroupType> padGroupTypes;
  std::set<int> padGroupTypeIds = getUnique(padGroups, groupTypeId);
  for (auto &pgt: padGroupTypeIds) {
    padGroupTypes.push_back(getPadGroupType(pgt));
  }
  return padGroupTypes;
}

std::vector<PadGroup> remap(const std::vector<PadGroup> &padGroups,
                            const std::vector<PadGroupType> &padGroupTypes,
                            const std::vector<std::pair<float, float>> &padSizes)
{
  // change the padGroupType ids in padGroups vector to
  // be from 0 to number of different padgrouptypes in padGroups vector
  std::map<int, int> idmap;
  std::map<int, int> sizemap;
  std::vector<PadGroup> remappedPadGroups;

  std::set<int> padGroupTypeIds = getUnique(padGroups, groupTypeId);
  std::set<int> padSizeIds = getUnique(padGroups, padSizeId);

  int i{0};
  for (auto &pgt: padGroupTypeIds) {
    idmap[pgt] = i++;
  }

  i = 0;
  for (auto &ps: padSizeIds) {
    sizemap[ps] = i++;
  }

  for (auto &pg: padGroups) {
    auto it = idmap.find(pg.mPadGroupTypeId);
    if (it == idmap.end()) {
      throw std::runtime_error("pad group id " + std::to_string(pg.mPadGroupTypeId) + " not found");
    }
    auto sit = sizemap.find(pg.mPadSizeId);
    if (sit == sizemap.end()) {
      throw std::runtime_error("pad size id " + std::to_string(pg.mPadSizeId) + " not found");
    }
    PadGroup rpg{pg};
    rpg.mPadGroupTypeId = it->second;
    rpg.mPadSizeId = sit->second;
    remappedPadGroups.push_back(rpg);
  }

  return remappedPadGroups;
}

std::vector<std::pair<float, float>> getPadSizes(const std::vector<PadGroup> &padGroups)
{
  std::set<int> padSizeIds = getUnique(padGroups, padSizeId);

  std::vector<std::pair<float, float>> padSizes;
  for (auto &sid: padSizeIds) {
    padSizes.push_back(std::make_pair<float, float>(padSizeX(sid), padSizeY(sid)));
  }
  return padSizes;
}

//int getSegType(int segtype)
//{
//  std::cout << "SEGTYPE " << segtype << "\n";
//  return segtype;
//}
//
//bool getBendingPlane(bool bp)
//{
//  std::cout << "BENDING " << (bp ? "true" : "false") << "\n";
//  return bp;
//}

Segmentation::Segmentation(int segType, bool isBendingPlane, std::vector<PadGroup> padGroups) :
  mSegType{segType},
  mIsBendingPlane{isBendingPlane},
  mDualSampaIds{getUnique(padGroups, fecId)},
  mPadGroupTypes{getPadGroupTypes(padGroups)},
  mPadSizes{getPadSizes(padGroups)},
  mPadGroups{remap(padGroups, mPadGroupTypes, mPadSizes)},
  mPadGroupContours{computeContours(mPadGroups, mPadGroupTypes, mPadSizes)}
{
  // note that in order to have the Segmentation object as much as possible "self-contained"
  // (i.e. avoid relying on some global memory access)
  // we "import" the padgrouptypes and padsizes we are interested in
}

Segmentation::Segmentation(int segType, bool isBendingPlane, std::vector<PadGroup> padGroups,
                           std::vector<PadGroupType> padGroupTypes,
                           std::vector<std::pair<float,float>> padSizes)
:
  mSegType{segType},
  mIsBendingPlane{isBendingPlane},
  mDualSampaIds{getUnique(padGroups, fecId)},
  mPadGroupTypes{padGroupTypes},
  mPadSizes{padSizes},
  mPadGroups{padGroups},
  mPadGroupContours{computeContours(mPadGroups, mPadGroupTypes, mPadSizes)}
{
}

std::vector<int> Segmentation::padGroupIndices(int dualSampaId) const
{
  std::vector<int> rv;
  for (auto i = 0; i < mPadGroups.size(); ++i) {
    if (mPadGroups[i].mFECId == dualSampaId) {
      rv.push_back(i);
    }
  }
  return rv;
}

bool Segmentation::hasPadByPosition(double x, double y) const
{
  return false;
}

int Segmentation::nofPads(const PadGroup &padGroup) const
{
  return mPadGroupTypes[padGroup.mPadGroupTypeId].getNofPads();
}


bool Segmentation::hasPadByFEE(int dualSampaId, int dualSampaChannel) const
{
  bool rv{false};
  for (auto index: padGroupIndices(dualSampaId)) {
    auto &pg = mPadGroups[index];
    auto &pgt = mPadGroupTypes[pg.mPadGroupTypeId];
    if (pgt.hasPadById(dualSampaChannel)) {
      rv = true;
    }
  }
  return rv;
}

}
}
}
}

