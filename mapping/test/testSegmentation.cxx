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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include "segmentationFactory.h"

#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic/generators/xrange.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "boost/format.hpp"
#include <iostream>

using namespace o2::mch::mapping;
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(o2_mch_mapping)
BOOST_AUTO_TEST_SUITE(segmentation_interface)

std::unique_ptr<SegmentationInterface> testSegmentation(int impl, int detElemId, bool bending)
{
  if (impl == 0) {
    return getSegmentation(detElemId, bending);
  } else if (impl == 1) {
    return getSegmentation<SegmentationImplementation::Simple>(detElemId, bending);
  } else {
    throw std::out_of_range("impl can only be 0 or 1");
  }
}

BOOST_DATA_TEST_CASE(GetSegmentationThrowsIfDetElemIdIsNotValid, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_THROW(testSegmentation(impl, -1, true), std::out_of_range);
  BOOST_CHECK_THROW(testSegmentation(impl, 121, true), std::out_of_range);
}

BOOST_DATA_TEST_CASE(GetSegmentationMustNotThrowIfDetElemIdIsValid, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_NO_THROW(testSegmentation(impl, 100, true));
}

BOOST_DATA_TEST_CASE(NofBendingPads, boost::unit_test::data::make({0, 1}), impl)
{
  // we explicitly don't make a loop
  // we prefer this solution to more clearly document the number of pads per DE-type
  // sorted by number of pads.

  BOOST_CHECK_EQUAL(testSegmentation(impl, 100, true)->nofPads(), 14392);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 300, true)->nofPads(), 13947);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 902, true)->nofPads(), 4480);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 702, true)->nofPads(), 4160);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 701, true)->nofPads(), 4096);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 601, true)->nofPads(), 3648);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 501, true)->nofPads(), 3568);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 602, true)->nofPads(), 3200);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 700, true)->nofPads(), 3200);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 502, true)->nofPads(), 3120);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 600, true)->nofPads(), 3008);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 500, true)->nofPads(), 2928);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 903, true)->nofPads(), 2880);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 703, true)->nofPads(), 2560);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 904, true)->nofPads(), 2240);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 503, true)->nofPads(), 1920);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 704, true)->nofPads(), 1920);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 504, true)->nofPads(), 1280);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 905, true)->nofPads(), 1280);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 705, true)->nofPads(), 960);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 706, true)->nofPads(), 640);
}

BOOST_DATA_TEST_CASE(NofNonBendingPads, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_EQUAL(testSegmentation(impl, 100, false)->nofPads(), 14280);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 300, false)->nofPads(), 13986);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 902, false)->nofPads(), 3136);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 702, false)->nofPads(), 2912);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 701, false)->nofPads(), 2880);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 601, false)->nofPads(), 2560);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 501, false)->nofPads(), 2496);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 602, false)->nofPads(), 2240);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 700, false)->nofPads(), 2240);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 502, false)->nofPads(), 2176);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 600, false)->nofPads(), 2112);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 500, false)->nofPads(), 2048);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 903, false)->nofPads(), 2016);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 703, false)->nofPads(), 1792);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 904, false)->nofPads(), 1568);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 503, false)->nofPads(), 1344);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 704, false)->nofPads(), 1344);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 504, false)->nofPads(), 896);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 905, false)->nofPads(), 896);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 705, false)->nofPads(), 672);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 706, false)->nofPads(), 448);
}

BOOST_DATA_TEST_CASE(TotalNofBendingFECInSegTypes, boost::unit_test::data::make({0, 1}), impl)
{
  int nb{0};
  int nnb{0};
  for (auto detElemId : getOneDetElemIdPerSegmentation()) {
    nb += testSegmentation(impl, detElemId, true)->nofDualSampas();
    nnb += testSegmentation(impl, detElemId, false)->nofDualSampas();
  }
  BOOST_CHECK_EQUAL(nb,1246);
  BOOST_CHECK_EQUAL(nnb,1019);
}

BOOST_DATA_TEST_CASE(NofBendingFEC, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_EQUAL(testSegmentation(impl, 100, true)->nofDualSampas(), 226);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 300, true)->nofDualSampas(), 221);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 902, true)->nofDualSampas(), 70);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 702, true)->nofDualSampas(), 65);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 701, true)->nofDualSampas(), 64);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 601, true)->nofDualSampas(), 57);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 501, true)->nofDualSampas(), 56);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 602, true)->nofDualSampas(), 50);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 700, true)->nofDualSampas(), 50);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 502, true)->nofDualSampas(), 49);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 600, true)->nofDualSampas(), 47);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 500, true)->nofDualSampas(), 46);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 903, true)->nofDualSampas(), 45);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 703, true)->nofDualSampas(), 40);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 904, true)->nofDualSampas(), 35);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 503, true)->nofDualSampas(), 30);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 704, true)->nofDualSampas(), 30);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 504, true)->nofDualSampas(), 20);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 905, true)->nofDualSampas(), 20);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 705, true)->nofDualSampas(), 15);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 706, true)->nofDualSampas(), 10);
}

BOOST_DATA_TEST_CASE(NofNonBendingFEC, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_EQUAL(testSegmentation(impl, 100, false)->nofDualSampas(), 225);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 300, false)->nofDualSampas(), 222);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 902, false)->nofDualSampas(), 50);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 701, false)->nofDualSampas(), 46);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 702, false)->nofDualSampas(), 46);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 601, false)->nofDualSampas(), 40);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 501, false)->nofDualSampas(), 39);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 700, false)->nofDualSampas(), 36);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 602, false)->nofDualSampas(), 35);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 502, false)->nofDualSampas(), 34);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 600, false)->nofDualSampas(), 33);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 903, false)->nofDualSampas(), 33);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 500, false)->nofDualSampas(), 32);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 703, false)->nofDualSampas(), 29);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 904, false)->nofDualSampas(), 26);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 704, false)->nofDualSampas(), 22);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 503, false)->nofDualSampas(), 21);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 905, false)->nofDualSampas(), 16);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 504, false)->nofDualSampas(), 14);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 705, false)->nofDualSampas(), 12);
  BOOST_CHECK_EQUAL(testSegmentation(impl, 706, false)->nofDualSampas(), 8);
}

BOOST_DATA_TEST_CASE(CountPadsInSegmentations, boost::unit_test::data::make({0, 1}), impl)
{
  int n{0};
  for (auto detElemId : getOneDetElemIdPerSegmentation()) {
    for (auto plane : {true, false}) {
      auto seg = testSegmentation(impl, detElemId, plane);
      for (int i = 0; i < seg->nofDualSampas(); ++i) {
        auto pads = seg->getPads(seg->getSampaId(i));
        n += pads.size();
      }
    }
  }
  BOOST_CHECK_EQUAL(n, 143469);
}

BOOST_DATA_TEST_CASE(LoopOnSegmentations, boost::unit_test::data::make({0, 1}), impl)
{
  int n{0};
  auto des{getOneDetElemIdPerSegmentation()};
  for (auto detElemId : des) {
    for (auto plane : {true, false}) {
      auto seg = testSegmentation(impl, detElemId, plane);
      ++n;
    }
  }
  BOOST_CHECK_EQUAL(n, 42);
}

BOOST_DATA_TEST_CASE(DualSampasWithLessThan64Pads, boost::unit_test::data::make({0, 1}), impl)
{
  std::map<int, int> non64;
  for (auto detElemId : getOneDetElemIdPerSegmentation()) {
    for (auto plane : {true, false}) {
      auto seg = testSegmentation(impl, detElemId, plane);
      for (int i = 0; i < seg->nofDualSampas(); ++i) {
        auto pads = seg->getPads(seg->getSampaId(i));
        if (pads.size() != 64) {
          non64[pads.size()]++;
        }
      }
    }
  }

  BOOST_CHECK_EQUAL(non64[31], 1);
  BOOST_CHECK_EQUAL(non64[32], 2);
  BOOST_CHECK_EQUAL(non64[39], 1);
  BOOST_CHECK_EQUAL(non64[40], 3);
  BOOST_CHECK_EQUAL(non64[46], 2);
  BOOST_CHECK_EQUAL(non64[48], 10);
  BOOST_CHECK_EQUAL(non64[49], 1);
  BOOST_CHECK_EQUAL(non64[50], 1);
  BOOST_CHECK_EQUAL(non64[52], 3);
  BOOST_CHECK_EQUAL(non64[54], 2);
  BOOST_CHECK_EQUAL(non64[55], 3);
  BOOST_CHECK_EQUAL(non64[56], 114);
  BOOST_CHECK_EQUAL(non64[57], 3);
  BOOST_CHECK_EQUAL(non64[58], 2);
  BOOST_CHECK_EQUAL(non64[59], 1);
  BOOST_CHECK_EQUAL(non64[60], 6);
  BOOST_CHECK_EQUAL(non64[62], 4);
  BOOST_CHECK_EQUAL(non64[63], 7);

  int n{0};
  for (auto p: non64) {
    n += p.second;
  }

  BOOST_CHECK_EQUAL(n, 166);
}

struct SEG
{
    std::unique_ptr<SegmentationInterface> seg(int impl)
    { return testSegmentation(impl, 100, true); }
};

BOOST_FIXTURE_TEST_SUITE(HasPadBy, SEG)

BOOST_DATA_TEST_CASE(ThrowsIfDualSampaChannelIsNotBetween0And63, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_THROW(seg(impl)->hasPadByFEE(102, -1), std::out_of_range);
  BOOST_CHECK_THROW(seg(impl)->hasPadByFEE(102, 64), std::out_of_range);
}

BOOST_DATA_TEST_CASE(PadByFEEThrowsIfDualSampaIdIsInvalid, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_THROW(seg(impl)->hasPadByFEE(1000, 0), std::out_of_range);
}

BOOST_DATA_TEST_CASE(ReturnsTrueIfPadIsConnected, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_EQUAL(seg(impl)->hasPadByFEE(102, 3), true);
}

BOOST_DATA_TEST_CASE(ReturnsFalseIfPadIsNotConnected, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_EQUAL(seg(impl)->hasPadByFEE(214, 14), false);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(HasPadByPosition, boost::unit_test::data::make({0, 1}), impl)
{
  BOOST_CHECK_EQUAL(testSegmentation(impl, 100, true)->hasPadByPosition(40.0, 30.0), true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
