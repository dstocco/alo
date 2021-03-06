#include "motifType.h"
#include "AliMpMotifType.h"
#include "AliMpPCB.h"
#include "AliMpMotifPosition.h"
#include <algorithm>
#include <iostream>
#include "seg.h"
#include "AliMpPlaneType.h"
#include "AliMpSector.h"
#include "AliMpMotifMap.h"
#include "AliMpMotifSpecial.h"
#include "AliMpConnection.h"
#include "AliMpEncodePair.h"
#include <memory>

std::vector<AliMpMotifType *> get_allslatmotiftypes(const std::vector<AliMpPCB *> &pcbs)
{

  std::vector<AliMpMotifType *> motifTypes;

  for (auto &pcb: pcbs) {
    for (int i = 0; i < pcb->GetSize(); ++i) {
      AliMpMotifType *mt = pcb->GetMotifPosition(i)->GetMotif()->GetMotifType();
      if (std::find_if(motifTypes.begin(), motifTypes.end(),
                       [&mt](AliMpMotifType *a) {
                         return strcmp(a->GetID(), mt->GetID()) == 0;
                       }
      ) == motifTypes.end()) {
        motifTypes.push_back(static_cast<AliMpMotifType *>(mt->Clone()));
      }
    }
  }

  return motifTypes;
}

AliMpMotifType *patchMotifType(const AliMpMotifType &mt, const std::string newname)
{
  // decrease iy by one for all connetions of motifTypes 2Bn1 and 2Nn1
  if ((newname != "2Bn1") && (newname != "2Nn1")) {
    return static_cast<AliMpMotifType *>(mt.Clone(newname.c_str()));
  }

  AliMpMotifType *m = new AliMpMotifType(newname);
  AliMpPad pad;
  std::unique_ptr<AliMpVPadIterator> it(mt.CreateIterator());
  it->First();
  int ixmin{std::numeric_limits<int>::max()};
  int iymin{std::numeric_limits<int>::max()};
  while (!it->IsDone()) {
    pad = it->CurrentItem();
    it->Next();
    AliMpConnection *c = mt.FindConnectionByLocalIndices(pad.GetIx(), pad.GetIy());
    AliMpConnection *patchedConnection = new AliMpConnection(c->GetPadNum(), c->GetBergNum(), c->GetKaptonNum(),
                                                             c->GetManuChannel(),
                                                             AliMp::Pair(c->GetLocalIx(), c->GetLocalIy() - 1));
    m->AddConnection(patchedConnection);
  }
  m->SetNofPads(mt.GetNofPadsX(), mt.GetNofPadsY());
  return m;
}

std::vector<AliMpMotifType *> get_allsectormotiftypes(const std::vector<const AliMpSector *> &sectors)
{
  std::vector<AliMpMotifType *> motifTypes;

  for (auto &s: sectors) {
    std::string prefix = get_sector_plane_prefix(*s);

    AliMpMotifMap *motifMap = s->GetMotifMap();

    for (int i = 0; i < motifMap->GetNofMotifPositions(); ++i) {
      AliMpMotifType *mt = motifMap->GetMotifPosition(i)->GetMotif()->GetMotifType();
      std::string newname = prefix + mt->GetID().Data();
      if (std::find_if(motifTypes.begin(), motifTypes.end(),
                       [&](AliMpMotifType *a) {
                         return newname == a->GetID();
                       }
      ) == motifTypes.end()) {
        motifTypes.push_back(patchMotifType(*mt, newname));
      }
    }
  }
  return motifTypes;
}


std::vector<AliMpMotifType *>
get_allmotiftypes(const std::vector<AliMpPCB *> &pcbs, const std::vector<const AliMpSector *> &sectors)
{
  /// get all the motif types, ordered alphabetically by (string) ID

  auto slatmt = get_allslatmotiftypes(pcbs);
  auto sectormt = get_allsectormotiftypes(sectors);
  auto mt = slatmt;
  mt.insert(mt.end(), sectormt.begin(), sectormt.end());
  std::sort(mt.begin(), mt.end(), [](AliMpMotifType *a, AliMpMotifType *b) { return a->GetID() < b->GetID(); });

  return mt;
}


int get_motiftype_index(std::string motifID, const std::vector<AliMpMotifType *> &motifTypes)
{
  auto i = motifID.find_first_of('-');

  std::string motifTypeId = motifID.substr(0, i);

  auto index = std::find_if(motifTypes.begin(), motifTypes.end(), [&](AliMpMotifType *mt) {
    return motifTypeId == mt->GetID();
  });

  return std::distance(motifTypes.begin(), index);
}


