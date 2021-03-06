/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPSESSIONIMPL_H_
#define _JSEPSESSIONIMPL_H_


#include <vector>

#include "signaling/src/jsep/JsepCodecDescription.h"
#include "signaling/src/jsep/JsepMediaStreamTrack.h"
#include "signaling/src/jsep/JsepSession.h"
#include "signaling/src/jsep/JsepTrack.h"
#include "signaling/src/sdp/SipccSdpParser.h"

namespace mozilla {
namespace jsep {

class JsepSessionImpl : public JsepSession {
 public:
  JsepSessionImpl(const std::string& name) :
      JsepSession(name),
      mSessionId(0),
      mSessionVersion(0) {
    Init();
  }

  JsepSessionImpl() :
      JsepSession("anonymous"),
      mSessionId(0),
      mSessionVersion(0) {
    Init();
  }

  virtual ~JsepSessionImpl() {}

  // Implement JsepSession methods.
  virtual nsresult AddTrack(const RefPtr<JsepMediaStreamTrack>& track)
      MOZ_OVERRIDE;
  virtual nsresult RemoveTrack(size_t track_index) MOZ_OVERRIDE {
    MOZ_CRASH();  // Stub
  }
  virtual nsresult SetIceCredentials(const std::string& ufrag,
                                     const std::string& pwd) MOZ_OVERRIDE;
  virtual nsresult AddDtlsFingerprint(const std::string& algorithm,
                                      const std::string& value) MOZ_OVERRIDE;

  virtual nsresult ReplaceTrack(
    size_t track_index,
      const RefPtr<JsepMediaStreamTrack>& track) MOZ_OVERRIDE {
    MOZ_CRASH(); // Stub
  }

  virtual size_t num_local_tracks() const MOZ_OVERRIDE {
    return mLocalTracks.size();
  }
  virtual nsresult local_track(size_t index,
                               RefPtr<JsepMediaStreamTrack>* track)
    const MOZ_OVERRIDE;

  virtual size_t num_remote_tracks() const MOZ_OVERRIDE {
    return mRemoteTracks.size();
  }
  virtual nsresult remote_track(size_t index,
                                RefPtr<JsepMediaStreamTrack>* track)
      const MOZ_OVERRIDE;

  virtual nsresult CreateOffer(const JsepOfferOptions& options,
                               std::string* offer) MOZ_OVERRIDE;
  virtual nsresult CreateAnswer(const JsepAnswerOptions& options,
                                std::string* answer) MOZ_OVERRIDE;
  virtual nsresult SetLocalDescription(JsepSdpType type,
                                       const std::string& sdp) MOZ_OVERRIDE;

  virtual nsresult SetRemoteDescription(JsepSdpType type,
                                        const std::string& sdp) MOZ_OVERRIDE;

  virtual bool ice_controlling() const {
    return mIceControlling;
  }

  // Access transports.
  virtual size_t num_transports() const MOZ_OVERRIDE {
    return mTransports.size();
  }

  virtual nsresult transport(size_t index, RefPtr<JsepTransport>* transport)
    const MOZ_OVERRIDE {
    if (index >= mTransports.size())
      return NS_ERROR_INVALID_ARG;

    *transport = mTransports[index];

    return NS_OK;
  }

  // Access the negotiated track pairs.
  virtual size_t num_negotiated_track_pairs()
      const MOZ_OVERRIDE {
    return mNegotiatedTrackPairs.size();
  }

  virtual nsresult negotiated_track_pair(size_t index,
                                         const JsepTrackPair** pair) const {
    if (index >= mNegotiatedTrackPairs.size())
      return NS_ERROR_INVALID_ARG;

    *pair = mNegotiatedTrackPairs[index];

    return NS_OK;
  }

 private:
  struct JsepDtlsFingerprint {
    std::string mAlgorithm;
    std::string mValue;
  };

  struct JsepSendingTrack {
    RefPtr<JsepMediaStreamTrack> mTrack;
    Maybe<size_t> mAssignedMLine;
  };

  struct JsepReceivingTrack {
    RefPtr<JsepMediaStreamTrack> mTrack;
    Maybe<size_t> mAssignedMLine;
  };

  void Init();
  nsresult CreateGenericSDP(UniquePtr<Sdp>* sdp);
  void AddCodecs(SdpMediaSection::MediaType mediatype,
                 SdpMediaSection* msection);
  JsepCodecDescription* FindMatchingCodec(SdpMediaSection::MediaType mediatype,
    const SdpRtpmapAttributeList::Rtpmap& entry);
  void AddCommonCodecs(const SdpMediaSection& remote_section,
                       SdpMediaSection* msection);
  void SetupDefaultCodecs();
  void SetState(JsepSignalingState state);
  nsresult ParseSdp(const std::string& sdp, UniquePtr<Sdp>* parsedp);
  nsresult SetLocalDescriptionOffer(UniquePtr<Sdp> offer);
  nsresult SetLocalDescriptionAnswer(JsepSdpType type,
                                      UniquePtr<Sdp> answer);
  nsresult SetRemoteDescriptionOffer(UniquePtr<Sdp> offer);
  nsresult SetRemoteDescriptionAnswer(JsepSdpType type,
                                      UniquePtr<Sdp> answer);
  nsresult HandleNegotiatedSession(const UniquePtr<Sdp>& local,
                                   const UniquePtr<Sdp>& remote,
                                   bool is_offerer);
  nsresult DetermineSendingDirection(SdpDirectionAttribute::Direction offer,
                                    SdpDirectionAttribute::Direction answer,
                                    bool is_offerer,
                                    bool* sending, bool* receiving);
  nsresult AddTransportAttributes(SdpMediaSection* msection);
  nsresult CreateTrack(const SdpMediaSection& receive,
                       const SdpMediaSection& send,
                       UniquePtr<JsepTrack>* track);
  void ClearNegotiatedPairs() {
    for (auto p = mNegotiatedTrackPairs.begin();
         p != mNegotiatedTrackPairs.end(); ++p) {
      delete *p;
    }
    mNegotiatedTrackPairs.clear();
  }
  nsresult CreateTransport(const SdpMediaSection& msection,
                           RefPtr<JsepTransport>* transport);

  nsresult SetupTransport(const SdpAttributeList& remote,
                          const SdpAttributeList& offer,
                          const SdpAttributeList& answer,
                          const RefPtr<JsepTransport>& transport);


  std::vector<JsepSendingTrack> mLocalTracks;
  std::vector<JsepReceivingTrack> mRemoteTracks;
  std::vector<RefPtr<JsepTransport>> mTransports;
  std::vector<JsepTrackPair*> mNegotiatedTrackPairs;  // TODO(ekr@rtfm.com): Add to dtor

  bool mIceControlling;
  std::string mIceUfrag;
  std::string mIcePwd;
  std::vector<JsepDtlsFingerprint> mDtlsFingerprints;
  uint64_t mSessionId;
  uint64_t mSessionVersion;
  UniquePtr<Sdp> mGeneratedLocalDescription; // Created but not set.
  UniquePtr<Sdp> mCurrentLocalDescription;
  UniquePtr<Sdp> mCurrentRemoteDescription;
  UniquePtr<Sdp> mPendingLocalDescription;
  UniquePtr<Sdp> mPendingRemoteDescription;
  std::vector<JsepCodecDescription> mCodecs;
  SipccSdpParser mParser;
};

}  // namespace jsep
}  // namespace mozilla

#endif
