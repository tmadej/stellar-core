// Copyright 2014 Stellar Development Foundation and contributors. Licensed
// under the ISC License. See the COPYING file at the top-level directory of
// this distribution or at http://opensource.org/licenses/ISC

#include "FBA.h"

#include "fba/LocalNode.h"
#include "fba/Slot.h"

namespace stellar
{

using xdr::operator<;

int
FBA::Client::compareValues(const Hash& v1,
                           const Hash& v2)
{
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

FBA::FBA(const uint256& validationSeed,
         const FBAQuorumSet& qSetLocal,
         Client* client)
    : mClient(client)
{
    mLocalNode = new LocalNode(validationSeed, qSetLocal, this);
    mKnownNodes[mLocalNode->getNodeID()] = mLocalNode;
}

FBA::~FBA()
{
    for (auto it : mKnownNodes)
    {
        delete it.second;
    }
    for (auto it : mKnownSlots)
    {
        delete it.second;
    }
}

void
FBA::receiveQuorumSet(const uint256& nodeID, 
                      const FBAQuorumSet& qSet)
{
    getNode(nodeID)->cacheQuorumSet(qSet);
}

void
FBA::receiveEnvelope(const FBAEnvelope& envelope)
{
    uint64 slotIndex = envelope.statement.slotIndex;
    return getSlot(slotIndex)->processEnvelope(envelope);
}

bool
FBA::attemptValue(const uint64& slotIndex,
                  const Hash& valueHash)
{
    return getSlot(slotIndex)->attemptValue(valueHash);
}

void 
FBA::updateLocalQuorumSet(const FBAQuorumSet& qSet)
{
    mLocalNode->updateQuorumSet(qSet);
}

const FBAQuorumSet& 
FBA::getLocalQuorumSet()
{
    return mLocalNode->getQuorumSet();
}

const uint256& 
FBA::getLocalNodeID()
{
  return mLocalNode->getNodeID();
}

Node* 
FBA::getNode(const uint256& nodeID)
{
    auto it = mKnownNodes.find(nodeID);
    if (it == mKnownNodes.end())
    {
        mKnownNodes[nodeID] = new Node(nodeID, this);
    }
    return mKnownNodes[nodeID];
}

LocalNode* 
FBA::getLocalNode()
{
  return mLocalNode;
}

Slot*
FBA::getSlot(const uint64& slotIndex)
{
    auto it = mKnownSlots.find(slotIndex);
    if (it == mKnownSlots.end())
    {
        mKnownSlots[slotIndex] = new Slot(slotIndex, this);
    }
    return mKnownSlots[slotIndex];
}

FBA::Client*
FBA::getClient()
{
    return mClient;
}

}