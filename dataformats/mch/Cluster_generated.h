// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_CLUSTER_O2_MCH_H_
#define FLATBUFFERS_GENERATED_CLUSTER_O2_MCH_H_

#include "flatbuffers/flatbuffers.h"

namespace o2 {
namespace mch {

struct ClusterPos;

struct PreCluster;

struct Cluster;

struct ClusterTimeBlock;

struct ClusterDE;

struct ClusterPos FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "o2.mch.ClusterPos";
  }
  enum {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X) &&
           VerifyField<float>(verifier, VT_Y) &&
           VerifyField<float>(verifier, VT_Z) &&
           verifier.EndTable();
  }
};

struct ClusterPosBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(ClusterPos::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(ClusterPos::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(ClusterPos::VT_Z, z, 0.0f);
  }
  explicit ClusterPosBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ClusterPosBuilder &operator=(const ClusterPosBuilder &);
  flatbuffers::Offset<ClusterPos> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ClusterPos>(end);
    return o;
  }
};

inline flatbuffers::Offset<ClusterPos> CreateClusterPos(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f) {
  ClusterPosBuilder builder_(_fbb);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct PreCluster FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "o2.mch.PreCluster";
  }
  enum {
    VT_DIGITBENDINGIDS = 4,
    VT_DIGITNONBENDINGIDS = 6
  };
  const flatbuffers::Vector<int32_t> *digitBendingIds() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_DIGITBENDINGIDS);
  }
  const flatbuffers::Vector<int32_t> *digitNonBendingIds() const {
    return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_DIGITNONBENDINGIDS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_DIGITBENDINGIDS) &&
           verifier.Verify(digitBendingIds()) &&
           VerifyOffset(verifier, VT_DIGITNONBENDINGIDS) &&
           verifier.Verify(digitNonBendingIds()) &&
           verifier.EndTable();
  }
};

struct PreClusterBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_digitBendingIds(flatbuffers::Offset<flatbuffers::Vector<int32_t>> digitBendingIds) {
    fbb_.AddOffset(PreCluster::VT_DIGITBENDINGIDS, digitBendingIds);
  }
  void add_digitNonBendingIds(flatbuffers::Offset<flatbuffers::Vector<int32_t>> digitNonBendingIds) {
    fbb_.AddOffset(PreCluster::VT_DIGITNONBENDINGIDS, digitNonBendingIds);
  }
  explicit PreClusterBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PreClusterBuilder &operator=(const PreClusterBuilder &);
  flatbuffers::Offset<PreCluster> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<PreCluster>(end);
    return o;
  }
};

inline flatbuffers::Offset<PreCluster> CreatePreCluster(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> digitBendingIds = 0,
    flatbuffers::Offset<flatbuffers::Vector<int32_t>> digitNonBendingIds = 0) {
  PreClusterBuilder builder_(_fbb);
  builder_.add_digitNonBendingIds(digitNonBendingIds);
  builder_.add_digitBendingIds(digitBendingIds);
  return builder_.Finish();
}

inline flatbuffers::Offset<PreCluster> CreatePreClusterDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<int32_t> *digitBendingIds = nullptr,
    const std::vector<int32_t> *digitNonBendingIds = nullptr) {
  return o2::mch::CreatePreCluster(
      _fbb,
      digitBendingIds ? _fbb.CreateVector<int32_t>(*digitBendingIds) : 0,
      digitNonBendingIds ? _fbb.CreateVector<int32_t>(*digitNonBendingIds) : 0);
}

struct Cluster FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "o2.mch.Cluster";
  }
  enum {
    VT_PRE = 4,
    VT_POS = 6
  };
  const PreCluster *pre() const {
    return GetPointer<const PreCluster *>(VT_PRE);
  }
  const ClusterPos *pos() const {
    return GetPointer<const ClusterPos *>(VT_POS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_PRE) &&
           verifier.VerifyTable(pre()) &&
           VerifyOffset(verifier, VT_POS) &&
           verifier.VerifyTable(pos()) &&
           verifier.EndTable();
  }
};

struct ClusterBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_pre(flatbuffers::Offset<PreCluster> pre) {
    fbb_.AddOffset(Cluster::VT_PRE, pre);
  }
  void add_pos(flatbuffers::Offset<ClusterPos> pos) {
    fbb_.AddOffset(Cluster::VT_POS, pos);
  }
  explicit ClusterBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ClusterBuilder &operator=(const ClusterBuilder &);
  flatbuffers::Offset<Cluster> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Cluster>(end);
    return o;
  }
};

inline flatbuffers::Offset<Cluster> CreateCluster(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<PreCluster> pre = 0,
    flatbuffers::Offset<ClusterPos> pos = 0) {
  ClusterBuilder builder_(_fbb);
  builder_.add_pos(pos);
  builder_.add_pre(pre);
  return builder_.Finish();
}

struct ClusterTimeBlock FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "o2.mch.ClusterTimeBlock";
  }
  enum {
    VT_TIMESTAMP = 4,
    VT_CLUSTERS = 6
  };
  int32_t timestamp() const {
    return GetField<int32_t>(VT_TIMESTAMP, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Cluster>> *clusters() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Cluster>> *>(VT_CLUSTERS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_TIMESTAMP) &&
           VerifyOffset(verifier, VT_CLUSTERS) &&
           verifier.Verify(clusters()) &&
           verifier.VerifyVectorOfTables(clusters()) &&
           verifier.EndTable();
  }
};

struct ClusterTimeBlockBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_timestamp(int32_t timestamp) {
    fbb_.AddElement<int32_t>(ClusterTimeBlock::VT_TIMESTAMP, timestamp, 0);
  }
  void add_clusters(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Cluster>>> clusters) {
    fbb_.AddOffset(ClusterTimeBlock::VT_CLUSTERS, clusters);
  }
  explicit ClusterTimeBlockBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ClusterTimeBlockBuilder &operator=(const ClusterTimeBlockBuilder &);
  flatbuffers::Offset<ClusterTimeBlock> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ClusterTimeBlock>(end);
    return o;
  }
};

inline flatbuffers::Offset<ClusterTimeBlock> CreateClusterTimeBlock(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t timestamp = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Cluster>>> clusters = 0) {
  ClusterTimeBlockBuilder builder_(_fbb);
  builder_.add_clusters(clusters);
  builder_.add_timestamp(timestamp);
  return builder_.Finish();
}

inline flatbuffers::Offset<ClusterTimeBlock> CreateClusterTimeBlockDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t timestamp = 0,
    const std::vector<flatbuffers::Offset<Cluster>> *clusters = nullptr) {
  return o2::mch::CreateClusterTimeBlock(
      _fbb,
      timestamp,
      clusters ? _fbb.CreateVector<flatbuffers::Offset<Cluster>>(*clusters) : 0);
}

struct ClusterDE FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  static FLATBUFFERS_CONSTEXPR const char *GetFullyQualifiedName() {
    return "o2.mch.ClusterDE";
  }
  enum {
    VT_DETELEMID = 4,
    VT_CLUSTERTIMEBLOCKS = 6
  };
  int32_t detElemId() const {
    return GetField<int32_t>(VT_DETELEMID, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<ClusterTimeBlock>> *clusterTimeBlocks() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<ClusterTimeBlock>> *>(VT_CLUSTERTIMEBLOCKS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_DETELEMID) &&
           VerifyOffset(verifier, VT_CLUSTERTIMEBLOCKS) &&
           verifier.Verify(clusterTimeBlocks()) &&
           verifier.VerifyVectorOfTables(clusterTimeBlocks()) &&
           verifier.EndTable();
  }
};

struct ClusterDEBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_detElemId(int32_t detElemId) {
    fbb_.AddElement<int32_t>(ClusterDE::VT_DETELEMID, detElemId, 0);
  }
  void add_clusterTimeBlocks(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ClusterTimeBlock>>> clusterTimeBlocks) {
    fbb_.AddOffset(ClusterDE::VT_CLUSTERTIMEBLOCKS, clusterTimeBlocks);
  }
  explicit ClusterDEBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ClusterDEBuilder &operator=(const ClusterDEBuilder &);
  flatbuffers::Offset<ClusterDE> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ClusterDE>(end);
    return o;
  }
};

inline flatbuffers::Offset<ClusterDE> CreateClusterDE(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t detElemId = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<ClusterTimeBlock>>> clusterTimeBlocks = 0) {
  ClusterDEBuilder builder_(_fbb);
  builder_.add_clusterTimeBlocks(clusterTimeBlocks);
  builder_.add_detElemId(detElemId);
  return builder_.Finish();
}

inline flatbuffers::Offset<ClusterDE> CreateClusterDEDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t detElemId = 0,
    const std::vector<flatbuffers::Offset<ClusterTimeBlock>> *clusterTimeBlocks = nullptr) {
  return o2::mch::CreateClusterDE(
      _fbb,
      detElemId,
      clusterTimeBlocks ? _fbb.CreateVector<flatbuffers::Offset<ClusterTimeBlock>>(*clusterTimeBlocks) : 0);
}

inline const o2::mch::ClusterDE *GetClusterDE(const void *buf) {
  return flatbuffers::GetRoot<o2::mch::ClusterDE>(buf);
}

inline bool VerifyClusterDEBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<o2::mch::ClusterDE>(nullptr);
}

inline void FinishClusterDEBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<o2::mch::ClusterDE> root) {
  fbb.Finish(root);
}

}  // namespace mch
}  // namespace o2

#endif  // FLATBUFFERS_GENERATED_CLUSTER_O2_MCH_H_
