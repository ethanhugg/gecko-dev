/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPMEDIASECTION_H_
#define _SIPCCSDPMEDIASECTION_H_

#include "mozilla/Attributes.h"
#include "mozilla/UniquePtr.h"
#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/SipccSdpAttributeList.h"

extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdp;
class SdpErrorHolder;

class SipccSdpMediaSection MOZ_FINAL : public SdpMediaSection
{
  friend class SipccSdp;
public:
  ~SipccSdpMediaSection() {}

  virtual MediaType
  GetMediaType() const MOZ_OVERRIDE
  {
    return mMediaType;
  }

  virtual unsigned int GetPort() const MOZ_OVERRIDE;
  virtual unsigned int GetPortCount() const MOZ_OVERRIDE;
  virtual Protocol GetProtocol() const MOZ_OVERRIDE;
  virtual const SdpConnection& GetConnection() const MOZ_OVERRIDE;
  virtual uint32_t GetBandwidth(const std::string& type) const MOZ_OVERRIDE;
  virtual const std::vector<std::string>& GetFormats() const MOZ_OVERRIDE;

  virtual const SdpAttributeList &GetAttributeList() const MOZ_OVERRIDE;
  virtual SdpAttributeList &GetAttributeList() MOZ_OVERRIDE;
  virtual SdpDirectionAttribute GetDirectionAttribute() const MOZ_OVERRIDE;

  virtual void AddCodec(const std::string& pt, const std::string& name,
                        uint32_t clockrate, uint16_t channels = 0,
                        const std::string& fmtp = "") MOZ_OVERRIDE;

  virtual void AddCodec(uint8_t pt, const std::string& name,
                        uint32_t clockrate, uint16_t channels,
                        const std::string& fmtp) MOZ_OVERRIDE;

  virtual void AddDataChannel(uint16_t pt,
                              const std::string& sctpmap) MOZ_OVERRIDE;

  virtual void Serialize(std::ostream&) const MOZ_OVERRIDE;

  static bool LoadBandwidth(std::map<std::string, uint32_t>&,
                            sdp_t* sdp, uint16_t level,
                            SdpErrorHolder& errorHolder);

private:
  SipccSdpMediaSection(size_t level,
                       const SipccSdpAttributeList *sessionLevel)
      : SdpMediaSection(level), mAttributeList(sessionLevel) {}

  bool Load(sdp_t* sdp, uint16_t level, SdpErrorHolder& errorHolder);
  bool LoadConnection(sdp_t* sdp, uint16_t level, SdpErrorHolder& errorHolder);
  bool LoadProtocol(sdp_t* sdp, uint16_t level,
                    SdpErrorHolder& errorHolder);
  void LoadFormats(sdp_t* sdp, uint16_t level);
  bool LoadBandwidth(sdp_t* sdp, uint16_t level, SdpErrorHolder& errorHolder);

  // the following values are cached on first get
  MediaType mMediaType;
  uint16_t mPort;
  uint16_t mPortCount;
  Protocol mProtocol;
  std::vector<std::string> mFormats;

  UniquePtr<SdpConnection> mConnection;
  std::map<std::string, uint32_t> mBandwidths;

  SipccSdpAttributeList mAttributeList;
};

}

#endif
