/*
The MIT License
Copyright (c) 2019 Lehrstuhl Informatik 11 - RWTH Aachen University
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE

This file is part of embeddedRTPS.

Author: i11 - Embedded Software, RWTH Aachen University
*/

#ifndef RTPS_PARTICIPANT_H
#define RTPS_PARTICIPANT_H

#include "rtps/common/types.h"
#include "rtps/config.h"
#include "rtps/discovery/SEDPAgent.h"
#include "rtps/discovery/SPDPAgent.h"
#include "rtps/messages/MessageReceiver.h"

namespace rtps {

class Writer;
class Reader;

class Participant {
public:
  GuidPrefix_t m_guidPrefix;
  ParticipantId_t m_participantId;
  uint8_t domainId;

  Participant();
  explicit Participant(const GuidPrefix_t &guidPrefix,
                       ParticipantId_t participantId,
                       uint8_t _domainId);

  // Not allowed because the message receiver contains a pointer to the
  // participant
  Participant(const Participant &) = delete;
  Participant(Participant &&) = delete;
  Participant &operator=(const Participant &) = delete;
  Participant &operator=(Participant &&) = delete;

  ~Participant();
  bool isValid();

  void reuse(const GuidPrefix_t &guidPrefix, ParticipantId_t participantId, uint8_t _domainId);

  std::array<uint8_t, 3> getNextUserEntityKey();

  // Actually the only two function that should be used by the user
  bool registerOnNewPublisherMatchedCallback(void (*callback)(void *arg),
                                             void *args);
  bool registerOnNewSubscriberMatchedCallback(void (*callback)(void *arg),
                                              void *args);

  //! Not-thread-safe function to add a writer
  Writer *addWriter(Writer *writer);
  bool isWritersFull();
  bool deleteWriter(Writer *writer);

  //! Not-thread-safe function to add a reader
  Reader *addReader(Reader *reader);
  bool isReadersFull();
  bool deleteReader(Reader *reader);

  //! (Probably) Thread safe if writers cannot be removed
  Writer *getWriter(EntityId_t id);
  Writer *getMatchingWriter(const TopicData &topicData);
  Writer *getMatchingWriter(const TopicDataCompressed &topicData);

  //! (Probably) Thread safe if readers cannot be removed
  Reader *getReader(EntityId_t id);
  Reader *getReaderByWriterId(const Guid_t &guid);
  Reader *getMatchingReader(const TopicData &topicData);
  Reader *getMatchingReader(const TopicDataCompressed &topicData);

  bool addNewRemoteParticipant(const ParticipantProxyData &remotePart);
  bool removeRemoteParticipant(const GuidPrefix_t &prefix);
  void removeAllProxiesOfParticipant(const GuidPrefix_t &prefix);
  void removeProxyFromAllEndpoints(const Guid_t &guid);

  const ParticipantProxyData *findRemoteParticipant(const GuidPrefix_t &prefix);
  void refreshRemoteParticipantLiveliness(const GuidPrefix_t &prefix);
  uint32_t getRemoteParticipantCount();
  MessageReceiver *getMessageReceiver();
  bool checkAndResetHeartbeats();

  bool hasReaderWithMulticastLocator(ip4_addr_t address);

  void addBuiltInEndpoints(BuiltInEndpoints &endpoints);
  void newMessage(const uint8_t *data, DataSize_t size);

  SPDPAgent &getSPDPAgent();
  void printInfo();

private:
  friend class SizeInspector;
  MessageReceiver m_receiver;
  bool m_hasBuilInEndpoints = false;
  std::array<uint8_t, 3> m_nextUserEntityId{{0, 0, 1}};
  std::array<Writer *, Config::NUM_WRITERS_PER_PARTICIPANT> m_writers = {
      nullptr};
  std::array<Reader *, Config::NUM_READERS_PER_PARTICIPANT> m_readers = {
      nullptr};

  SemaphoreHandle_t m_mutex;
  MemoryPool<ParticipantProxyData, Config::SPDP_MAX_NUMBER_FOUND_PARTICIPANTS>
      m_remoteParticipants;

  SPDPAgent m_spdpAgent;
  SEDPAgent m_sedpAgent;
};
} // namespace rtps

#endif // RTPS_PARTICIPANT_H
