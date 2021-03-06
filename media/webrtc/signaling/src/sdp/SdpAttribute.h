/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPATTRIBUTE_H_
#define _SDPATTRIBUTE_H_

#include <vector>
#include <ostream>
#include <sstream>

#include "mozilla/UniquePtr.h"
#include "mozilla/Attributes.h"
#include "mozilla/Assertions.h"

#include "signaling/src/sdp/SdpEnum.h"

namespace mozilla {

class SdpAttribute
{
public:
  enum AttributeType {
    kFirstAttribute = 0,
    kBundleOnlyAttribute = 0,
    kCandidateAttribute,
    kConnectionAttribute,
    kDirectionAttribute,
    kExtmapAttribute,
    kFingerprintAttribute,
    kFmtpAttribute,
    kGroupAttribute,
    kIceLiteAttribute,
    kIceMismatchAttribute,
    kIceOptionsAttribute,
    kIcePwdAttribute,
    kIceUfragAttribute,
    kIdentityAttribute,
    kImageattrAttribute,
    kInactiveAttribute,
    kLabelAttribute,
    kMaxptimeAttribute,
    kMidAttribute,
    kMsidAttribute,
    kMsidSemanticAttribute,
    kPtimeAttribute,
    kRecvonlyAttribute,
    kRemoteCandidatesAttribute,
    kRtcpAttribute,
    kRtcpFbAttribute,
    kRtcpMuxAttribute,
    kRtcpRsizeAttribute,
    kRtpmapAttribute,
    kSctpmapAttribute,
    kSendonlyAttribute,
    kSendrecvAttribute,
    kSetupAttribute,
    kSsrcAttribute,
    kSsrcGroupAttribute,
    kOtherAttribute // we rely on this being the last element in the array
  };

  explicit SdpAttribute(AttributeType type)
      : mType(type) {}
  virtual ~SdpAttribute() {}

  AttributeType GetType() const {
    return mType;
  }

  virtual const std::string GetTypeName() const {
    return GetAttributeTypeString(mType);
  }

  static bool IsAllowedAtSessionLevel(AttributeType type);
  static bool IsAllowedAtMediaLevel(AttributeType type);

  virtual void Serialize(std::ostream&) const = 0;

  static const std::string GetAttributeTypeString(AttributeType type) {
    switch (type) {
      case kBundleOnlyAttribute: return "bundle-only";
      case kCandidateAttribute: return "candidate";
      case kConnectionAttribute: return "connection";
      case kExtmapAttribute: return "extmap";
      case kFingerprintAttribute: return "fingerprint";
      case kFmtpAttribute: return "fmtp";
      case kGroupAttribute: return "group";
      case kIceLiteAttribute: return "ice-lite";
      case kIceMismatchAttribute: return "ice-mismatch";
      case kIceOptionsAttribute: return "ice-options";
      case kIcePwdAttribute: return "ice-pwd";
      case kIceUfragAttribute: return "ice-ufrag";
      case kIdentityAttribute: return "identity";
      case kImageattrAttribute: return "imageattr";
      case kInactiveAttribute: return "inactive";
      case kLabelAttribute: return "label";
      case kMaxptimeAttribute: return "maxptime";
      case kMidAttribute: return "mid";
      case kMsidAttribute: return "msid";
      case kMsidSemanticAttribute: return "msid-semantic";
      case kPtimeAttribute: return "ptime";
      case kRecvonlyAttribute: return "recvonly";
      case kRemoteCandidatesAttribute: return "remote-candidates";
      case kRtcpAttribute: return "rtcp";
      case kRtcpFbAttribute: return "rtcp-fb";
      case kRtcpMuxAttribute: return "rtcp-mux";
      case kRtcpRsizeAttribute: return "rtcp-rsize";
      case kRtpmapAttribute: return "rtpmap";
      case kSctpmapAttribute: return "sctpmap";
      case kSendonlyAttribute: return "sendonly";
      case kSendrecvAttribute: return "sendrecv";
      case kSetupAttribute: return "setup";
      case kSsrcAttribute: return "ssrc";
      case kSsrcGroupAttribute: return "ssrc-group";
        // Error if kDirectionAttribute, kOtherAttribute and others
      default:
        MOZ_ASSERT(false, "Unexpected attribute type");
        return "";
    }
  }

protected:
  AttributeType mType;
};

inline std::ostream& operator <<(std::ostream& os, const SdpAttribute &attr)
{
  attr.Serialize(os);
  return os;
}

inline std::ostream& operator <<(std::ostream& os, const SdpAttribute::AttributeType type) {
  os << SdpAttribute::GetAttributeTypeString(type);
  return os;
}

///////////////////////////////////////////////////////////////////////////
// a=candidate, RFC5245
//-------------------------------------------------------------------------
//
// candidate-attribute   = "candidate" ":" foundation SP component-id SP
//                          transport SP
//                          priority SP
//                          connection-address SP     ;from RFC 4566
//                          port         ;port from RFC 4566
//                          SP cand-type
//                          [SP rel-addr]
//                          [SP rel-port]
//                          *(SP extension-att-name SP
//                               extension-att-value)
// foundation            = 1*32ice-char
// component-id          = 1*5DIGIT
// transport             = "UDP" / transport-extension
// transport-extension   = token              ; from RFC 3261
// priority              = 1*10DIGIT
// cand-type             = "typ" SP candidate-types
// candidate-types       = "host" / "srflx" / "prflx" / "relay" / token
// rel-addr              = "raddr" SP connection-address
// rel-port              = "rport" SP port
// extension-att-name    = byte-string    ;from RFC 4566
// extension-att-value   = byte-string
// ice-char              = ALPHA / DIGIT / "+" / "/"
//class SdpCandidateAttributeList : public SdpAttribute
//{
//public:
//  SdpCandidateAttributeList() :
//    SdpAttribute(kCandidateAttribute) {}
//
//  enum Transport {
//    kUdp,
//    kTcp
//  };
//
//  enum Type {
//    kHost,
//    kSrflx,
//    kPrflx,
//    kRelay
//  };
//
//  struct Candidate
//  {
//    std::string foundation;
//    uint16_t componentId;
//    Transport transport;
//    uint32_t priority;
//    std::string address;
//    uint16_t port;
//    Type type;
//    std::string raddr;
//    uint16_t rport;
//  };
//
//  void PushEntry(const std::string& foundation,
//                 uint16_t componentId,
//                 Transport transport,
//                 uint32_t priority,
//                 const std::string& address,
//                 uint16_t port,
//                 Type type,
//                 const std::string& raddr = "",
//                 uint16_t rport = 0) {
//    mCandidates.push_back({
//      foundation,
//      componentId,
//      transport,
//      priority,
//      address,
//      port,
//      type,
//      raddr,
//      rport
//    });
//  }
//
//  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE
//  {
//    for (auto i = mCandidates.begin(); i != mCandidates.end(); ++i) {
//      os << "a=" << mType << ":"
//         << i->foundation << " "
//         << i->componentId << " "
//         << i->transport << " "
//         << i->priority << " "
//         << i->address << " "
//         << i->port << " "
//         << i->type;
//      if (i->raddr.length()) {
//        os << " " << i->raddr;
//      }
//      if (i->rport) {
//        os << " " << i->rport;
//      }
//      os << CRLF;
//    }
//  }
//
//  std::vector<Candidate> mCandidates;
//};
//
//inline std::ostream& operator <<(std::ostream& os,
//                                 SdpCandidateAttributeList::Transport t)
//{
//  switch (t) {
//    case SdpCandidateAttributeList::kUdp: os << "UDP"; break;
//    case SdpCandidateAttributeList::kTcp: os << "TCP"; break;
//    default: MOZ_ASSERT(false); os << "?";
//
//  }
//  return os;
//}
//
//inline std::ostream& operator <<(std::ostream& os,
//                                 SdpCandidateAttributeList::Type t)
//{
//  switch (t) {
//    case SdpCandidateAttributeList::kHost: os << "host"; break;
//    case SdpCandidateAttributeList::kSrflx: os << "srflx"; break;
//    case SdpCandidateAttributeList::kPrflx: os << "prflx"; break;
//    case SdpCandidateAttributeList::kRelay: os << "relay"; break;
//    default: MOZ_ASSERT(false); os << "?";
//  }
//  return os;
//}

///////////////////////////////////////////////////////////////////////////
// a=conection, RFC4145
//-------------------------------------------------------------------------
//         connection-attr        = "a=connection:" conn-value
//         conn-value             = "new" / "existing"
class SdpConnectionAttribute : public SdpAttribute
{
public:
  enum ConnValue {
    kNew,
    kExisting
  };

  SdpConnectionAttribute(SdpConnectionAttribute::ConnValue value) :
    SdpAttribute(kConnectionAttribute),
    mValue(value) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  ConnValue mValue;
};

inline std::ostream& operator <<(std::ostream& os,
                                 SdpConnectionAttribute::ConnValue c)
{
  switch (c) {
    case SdpConnectionAttribute::kNew: os << "new"; break;
    case SdpConnectionAttribute::kExisting: os << "existing"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}


///////////////////////////////////////////////////////////////////////////
// a=sendrecv / a=sendonly / a=recvonly / a=inactive, RFC 4566
//-------------------------------------------------------------------------
class SdpDirectionAttribute : public SdpAttribute
{
 public:
  enum Direction {
    kSendrecv,
    kSendonly,
    kRecvonly,
    kInactive
  };

  SdpDirectionAttribute(Direction value)
      : SdpAttribute(kDirectionAttribute),
        mValue(value) {}

  Direction mValue;

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  const std::string GetTypeName() const MOZ_OVERRIDE;
};

inline std::ostream& operator <<(std::ostream& os,
                                 SdpDirectionAttribute::Direction d)
{
  switch (d) {
    case SdpDirectionAttribute::kSendonly: os << "sendonly"; break;
    case SdpDirectionAttribute::kRecvonly: os << "recvonly"; break;
    case SdpDirectionAttribute::kSendrecv: os << "sendrecv"; break;
    case SdpDirectionAttribute::kInactive: os << "inactive"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

inline const std::string SdpDirectionAttribute::GetTypeName() const {
  std::ostringstream ss;
  ss << mValue;
  return ss.str();
}

///////////////////////////////////////////////////////////////////////////
// a=extmap, RFC5285
//-------------------------------------------------------------------------
// RFC5285
//        extmap = mapentry SP extensionname [SP extensionattributes]
//
//        extensionname = URI
//
//        direction = "sendonly" / "recvonly" / "sendrecv" / "inactive"
//
//        mapentry = "extmap:" 1*5DIGIT ["/" direction]
//
//        extensionattributes = byte-string
//
//        URI = <Defined in RFC 3986>
//
//        byte-string = <Defined in RFC 4566>
//
//        SP = <Defined in RFC 5234>
//
//        DIGIT = <Defined in RFC 5234>
class SdpExtmapAttributeList : public SdpAttribute
{
public:
  SdpExtmapAttributeList() :
    SdpAttribute(kExtmapAttribute) {}

  struct Extmap
  {
    uint16_t entry;
    SdpDirectionAttribute::Direction direction;
    bool direction_specified;
    std::string extensionname;
    std::string extensionattributes;
  };

  void PushEntry(uint16_t entry,
                 SdpDirectionAttribute::Direction direction,
                 bool direction_specified,
                 const std::string& extensionname,
                 const std::string& extensionattributes = "") {
    mExtmaps.push_back({
      entry,
      direction,
      direction_specified,
      extensionname,
      extensionattributes
    });
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Extmap> mExtmaps;
};

///////////////////////////////////////////////////////////////////////////
// a=fingerprint, RFC4572
//-------------------------------------------------------------------------
//   fingerprint-attribute  =  "fingerprint" ":" hash-func SP fingerprint
//
//   hash-func              =  "sha-1" / "sha-224" / "sha-256" /
//                             "sha-384" / "sha-512" /
//                             "md5" / "md2" / token
//                             ; Additional hash functions can only come
//                             ; from updates to RFC 3279
//
//   fingerprint            =  2UHEX *(":" 2UHEX)
//                             ; Each byte in upper-case hex, separated
//                             ; by colons.
//
//   UHEX                   =  DIGIT / %x41-46 ; A-F uppercase
class SdpFingerprintAttributeList : public SdpAttribute
{
public:
  SdpFingerprintAttributeList():
    SdpAttribute(kFingerprintAttribute) {}

  enum HashAlgorithm {
    kSha1,
    kSha224,
    kSha256,
    kSha384,
    kSha512,
    kMd5,
    kMd2,
    kUnknownAlgorithm
  };

  struct Fingerprint {
    HashAlgorithm hashFunc;
    std::string fingerprint;
  };

  // For use by application programmers. Enforces that it's a known and
  // non-crazy algorithm.
  void PushEntry(std::string algorithm_str, const std::string& fingerprint,
                 bool enforcePlausible = true) {
    SdpFingerprintAttributeList::HashAlgorithm algorithm =
      SdpFingerprintAttributeList::kUnknownAlgorithm;

    if (algorithm_str == "sha-1") {
      algorithm = SdpFingerprintAttributeList::kSha1;
    } else if (algorithm_str == "sha-224") {
      algorithm = SdpFingerprintAttributeList::kSha224;
    } else if (algorithm_str == "sha-256") {
      algorithm = SdpFingerprintAttributeList::kSha256;
    } else if (algorithm_str == "sha-384") {
      algorithm = SdpFingerprintAttributeList::kSha384;
    } else if (algorithm_str == "sha-512") {
      algorithm = SdpFingerprintAttributeList::kSha512;
    } else if (algorithm_str == "md5") {
      algorithm = SdpFingerprintAttributeList::kMd5;
    } else if (algorithm_str == "md2") {
      algorithm = SdpFingerprintAttributeList::kMd2;
    }

    if ((algorithm == SdpFingerprintAttributeList::kUnknownAlgorithm) ||
        fingerprint.empty()) {
      if (enforcePlausible) {
        MOZ_CRASH();
      } else {
        return;
      }
    }

    PushEntry(algorithm, fingerprint);
  }

  void PushEntry(HashAlgorithm hashFunc, const std::string& fingerprint) {
    mFingerprints.push_back({hashFunc, fingerprint});
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Fingerprint> mFingerprints;
};

inline std::ostream& operator <<(std::ostream& os,
                                 SdpFingerprintAttributeList::HashAlgorithm a)
{
  switch (a) {
    case SdpFingerprintAttributeList::kSha1: os << "sha-1"; break;
    case SdpFingerprintAttributeList::kSha224: os << "sha-224"; break;
    case SdpFingerprintAttributeList::kSha256: os << "sha-256"; break;
    case SdpFingerprintAttributeList::kSha384: os << "sha-384"; break;
    case SdpFingerprintAttributeList::kSha512: os << "sha-512"; break;
    case SdpFingerprintAttributeList::kMd5: os << "md5"; break;
    case SdpFingerprintAttributeList::kMd2: os << "md2"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

///////////////////////////////////////////////////////////////////////////
// a=fmtp, RFC4566, RFC5576
//-------------------------------------------------------------------------
//       a=fmtp:<format> <format specific parameters>
//
// TODO - Specialize according to codec type, to aid in parsing
// format-specific parameters
class SdpFmtpAttributeList : public SdpAttribute
{
public:
  SdpFmtpAttributeList() :
    SdpAttribute(kFmtpAttribute) {}

  struct Fmtp {
    std::string format;
    std::string parameters;
  };

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  void PushEntry(const std::string& format, const std::string& parameters) {
    mFmtps.push_back({ format, parameters });
  }

  std::vector<Fmtp> mFmtps;
};

///////////////////////////////////////////////////////////////////////////
// a=group, RFC5888
//-------------------------------------------------------------------------
//         group-attribute     = "a=group:" semantics
//                               *(SP identification-tag)
//         semantics           = "LS" / "FID" / semantics-extension
//         semantics-extension = token
//         identification-tag  = token
class SdpGroupAttributeList : public SdpAttribute
{
public:
  SdpGroupAttributeList() :
    SdpAttribute(kGroupAttribute) {}

  enum Semantics {
    kLs,      // RFC5888
    kFid,     // RFC5888
    kSrf,     // RFC3524
    kAnat,    // RFC4091
    kFec,     // RFC5956
    kFecFr,   // RFC5956
    kCs,      // draft-mehta-rmt-flute-sdp-05
    kDdp,     // RFC5583
    kDup,     // RFC7104
    kBundle   // draft-ietf-mmusic-bundle
  };

  struct Group {
    Semantics semantics;
    std::vector<std::string> tags;
  };

  void PushEntry(Semantics semantics,
                 const std::vector<std::string>& tags) {
    mGroups.push_back({
      semantics,
      tags
    });
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Group> mGroups;
};

inline std::ostream& operator <<(std::ostream& os,
                                 SdpGroupAttributeList::Semantics s)
{
  switch (s) {
    case SdpGroupAttributeList::kLs: os << "LS"; break;
    case SdpGroupAttributeList::kFid: os << "FID"; break;
    case SdpGroupAttributeList::kSrf: os << "SRF"; break;
    case SdpGroupAttributeList::kAnat: os << "ANAT"; break;
    case SdpGroupAttributeList::kFec: os << "FEC"; break;
    case SdpGroupAttributeList::kFecFr: os << "FEC-FR"; break;
    case SdpGroupAttributeList::kCs: os << "CS"; break;
    case SdpGroupAttributeList::kDdp: os << "DDP"; break;
    case SdpGroupAttributeList::kDup: os << "DUP"; break;
    case SdpGroupAttributeList::kBundle: os << "BUNDLE"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

///////////////////////////////////////////////////////////////////////////
// a=ice-options, RFC5245
//-------------------------------------------------------------------------
// ice-options           = "ice-options" ":" ice-option-tag
//                           0*(SP ice-option-tag)
// ice-option-tag        = 1*ice-char
class SdpIceOptionsAttribute : public SdpAttribute
{
public:
  SdpIceOptionsAttribute(const std::vector<std::string>& options) :
    SdpAttribute(kIceOptionsAttribute),
    mOptions(options) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<std::string> mOptions;
};

///////////////////////////////////////////////////////////////////////////
// a=identity, draft-ietf-rtcweb-security-arch
//-------------------------------------------------------------------------
//   identity-attribute  = "identity:" identity-assertion
//                         [ SP identity-extension
//                           *(";" [ SP ] identity-extension) ]
//   identity-assertion  = base64
//   base64              = 1*(ALPHA / DIGIT / "+" / "/" / "=" )
//   identity-extension  = extension-att-name [ "=" extension-att-value ]
//   extension-att-name  = token
//   extension-att-value = 1*(%x01-09 / %x0b-0c / %x0e-3a / %x3c-ff)
//                         ; byte-string from [RFC4566] omitting ";"
class SdpIdentityAttribute : public SdpAttribute
{
public:
  SdpIdentityAttribute(const std::string &assertion,
                       const std::vector<std::string> &extensions =
                         std::vector<std::string>()) :
    SdpAttribute(kIdentityAttribute),
    mAssertion(assertion),
    mExtensions(extensions) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::string mAssertion;
  std::vector<std::string> mExtensions;
};

///////////////////////////////////////////////////////////////////////////
// a=imageattr, RFC6236
//-------------------------------------------------------------------------
//     image-attr = "imageattr:" PT 1*2( 1*WSP ( "send" / "recv" )
//                                       1*WSP attr-list )
//     PT = 1*DIGIT / "*"
//     attr-list = ( set *(1*WSP set) ) / "*"
//       ;  WSP and DIGIT defined in [RFC5234]
//
//     set= "[" "x=" xyrange "," "y=" xyrange *( "," key-value ) "]"
//                ; x is the horizontal image size range (pixel count)
//                ; y is the vertical image size range (pixel count)
//
//     key-value = ( "sar=" srange )
//               / ( "par=" prange )
//               / ( "q=" qvalue )
//                ; Key-value MAY be extended with other keyword
//                ;  parameters.
//                ; At most, one instance each of sar, par, or q
//                ;  is allowed in a set.
//                ;
//                ; sar (sample aspect ratio) is the sample aspect ratio
//                ;  associated with the set (optional, MAY be ignored)
//                ; par (picture aspect ratio) is the allowed
//                ;  ratio between the display's x and y physical
//                ;  size (optional)
//                ; q (optional, range [0.0..1.0], default value 0.5)
//                ;  is the preference for the given set,
//                ;  a higher value means a higher preference
//
//     onetonine = "1" / "2" / "3" / "4" / "5" / "6" / "7" / "8" / "9"
//                ; Digit between 1 and 9
//     xyvalue = onetonine *5DIGIT
//                ; Digit between 1 and 9 that is
//                ; followed by 0 to 5 other digits
//     step = xyvalue
//     xyrange = ( "[" xyvalue ":" [ step ":" ] xyvalue "]" )
//                ; Range between a lower and an upper value
//                ; with an optional step, default step = 1
//                ; The rightmost occurrence of xyvalue MUST have a
//                ; higher value than the leftmost occurrence.
//             / ( "[" xyvalue 1*( "," xyvalue ) "]" )
//                ; Discrete values separated by ','
//             / ( xyvalue )
//                ; A single value
//     spvalue = ( "0" "." onetonine *3DIGIT )
//                ; Values between 0.1000 and 0.9999
//             / ( onetonine "." 1*4DIGIT )
//                ; Values between 1.0000 and 9.9999
//     srange =  ( "[" spvalue 1*( "," spvalue ) "]" )
//                ; Discrete values separated by ','.
//                ; Each occurrence of spvalue MUST be
//                ; greater than the previous occurrence.
//             / ( "[" spvalue "-" spvalue "]" )
//                ; Range between a lower and an upper level (inclusive)
//                ; The second occurrence of spvalue MUST have a higher
//                ; value than the first
//             / ( spvalue )
//                ; A single value
//
//     prange =  ( "[" spvalue "-" spvalue "]" )
//                ; Range between a lower and an upper level (inclusive)
//                ; The second occurrence of spvalue MUST have a higher
//                ; value than the first
//
//     qvalue  = ( "0" "." 1*2DIGIT )
//             / ( "1" "." 1*2("0") )
//                ; Values between 0.00 and 1.00
//
//  XXX TBD -- We don't use this yet, and it's a project unto itself.
//

class SdpImageattrAttributeList : public SdpAttribute
{
public:
  SdpImageattrAttributeList() :
    SdpAttribute(kImageattrAttribute) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;
};

///////////////////////////////////////////////////////////////////////////
// a=msid, draft-ietf-mmusic-msid
//-------------------------------------------------------------------------
//   msid-attr = "msid:" identifier [ SP appdata ]
//   identifier = 1*64token-char ; see RFC 4566
//   appdata = 1*64token-char  ; see RFC 4566
class SdpMsidAttributeList : public SdpAttribute
{
public:
  SdpMsidAttributeList() :
      SdpAttribute(kMsidAttribute) {}

  struct Msid {
    std::string identifier;
    std::string appdata;
  };

  void PushEntry(const std::string& identifier,
                 const std::string& appdata = "") {
    mMsids.push_back({ identifier, appdata });
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Msid> mMsids;
};

///////////////////////////////////////////////////////////////////////////
// a=remote-candiate, RFC5245
//-------------------------------------------------------------------------
//   remote-candidate-att = "remote-candidates" ":" remote-candidate
//                           0*(SP remote-candidate)
//   remote-candidate = component-ID SP connection-address SP port
class SdpRemoteCandidatesAttribute : public SdpAttribute
{
public:
  struct Candidate {
    std::string id;
    std::string address;
    uint16_t port;
  };

  SdpRemoteCandidatesAttribute(const std::vector<Candidate>& candidates) :
    SdpAttribute(kRemoteCandidatesAttribute),
    mCandidates(candidates) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Candidate> mCandidates;
};

///////////////////////////////////////////////////////////////////////////
// a=rtcp, RFC3605
//-------------------------------------------------------------------------
//   rtcp-attribute =  "a=rtcp:" port  [nettype space addrtype space
//                         connection-address] CRLF
class SdpRtcpAttribute : public SdpAttribute
{
public:
  SdpRtcpAttribute(uint16_t port,
                   sdp::NetType netType = sdp::kNetTypeNone,
                   sdp::AddrType addrType = sdp::kAddrTypeNone,
                   const std::string& address = "") :
    SdpAttribute(kRtcpAttribute),
    mPort(port),
    mNetType(netType),
    mAddrType(addrType),
    mAddress(address) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  uint16_t mPort;
  sdp::NetType mNetType;
  sdp::AddrType mAddrType;
  std::string mAddress;
};

///////////////////////////////////////////////////////////////////////////
// a=rtcp-fb, RFC4585
//-------------------------------------------------------------------------
//    rtcp-fb-syntax = "a=rtcp-fb:" rtcp-fb-pt SP rtcp-fb-val CRLF
//
//    rtcp-fb-pt         = "*"   ; wildcard: applies to all formats
//                       / fmt   ; as defined in SDP spec
//
//    rtcp-fb-val        = "ack" rtcp-fb-ack-param
//                       / "nack" rtcp-fb-nack-param
//                       / "trr-int" SP 1*DIGIT
//                       / rtcp-fb-id rtcp-fb-param
//
//    rtcp-fb-id         = 1*(alpha-numeric / "-" / "_")
//
//    rtcp-fb-param      = SP "app" [SP byte-string]
//                       / SP token [SP byte-string]
//                       / ; empty
//
//    rtcp-fb-ack-param  = SP "rpsi"
//                       / SP "app" [SP byte-string]
//                       / SP token [SP byte-string]
//                       / ; empty
//
//    rtcp-fb-nack-param = SP "pli"
//                       / SP "sli"
//                       / SP "rpsi"
//                       / SP "app" [SP byte-string]
//                       / SP token [SP byte-string]
//                       / ; empty
//
// TODO: Specialize into ack/nack/etc
class SdpRtcpFbAttributeList : public SdpAttribute
{
public:
  SdpRtcpFbAttributeList() :
    SdpAttribute(kRtcpFbAttribute) {}

  enum Type {
    kAck,
    kApp,
    kCcm,
    kNack,
    kTrrInt
  };

  struct Feedback {
    std::string pt;
    Type type;
    std::string parameters;
  };

  void PushEntry(const std::string& pt,
                 Type type,
                 const std::string& parameters) {
    mFeedback.push_back({ pt, type, parameters });
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Feedback> mFeedback;
};

///////////////////////////////////////////////////////////////////////////
// a=rtpmap, RFC4566
//-------------------------------------------------------------------------
// a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]
class SdpRtpmapAttributeList : public SdpAttribute
{
public:
  SdpRtpmapAttributeList() :
    SdpAttribute(kRtpmapAttribute) {}

  struct Rtpmap {
    std::string pt;
    std::string name;
    uint32_t clock;
    // Technically, this could mean something else in the future.
    // In practice, that's probably not going to happen.
    uint32_t channels;
  };

  void PushEntry(const std::string &pt,
                 const std::string &name,
                 uint32_t clock,
                 uint32_t channels = 0) {
    mRtpmaps.push_back({pt, name, clock, channels});
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  bool HasEntry(const std::string& pt) const {
    for (auto it = mRtpmaps.begin(); it != mRtpmaps.end(); ++it) {
      if (it->pt == pt) {
        return true;
      }
    }
    return false;
  }

  const Rtpmap& GetEntry(const std::string& pt) const {
    for (auto it = mRtpmaps.begin(); it != mRtpmaps.end(); ++it) {
      if (it->pt == pt) {
        return *it;
      }
    }
    MOZ_CRASH();
  }

  std::vector<Rtpmap> mRtpmaps;
};

///////////////////////////////////////////////////////////////////////////
// a=sctpmap, draft-ietf-mmusic-sctp-sdp-06
//-------------------------------------------------------------------------
//         sctpmap-attr        =  "a=sctpmap:" sctpmap-number
//                                 app [max-message-size] [streams]
//         sctpmap-number      =  1*DIGIT
//         app                 =  token
//         max-message-size    =  "max-message-size" EQUALS 1*DIGIT
//         streams             =  "streams" EQUALS 1*DIGIT"
//
// We're going to pretend that there are spaces where they make sense.
//
// (draft-07 appears to have done something really funky here, but I
// don't beleive it).
class SdpSctpmapAttributeList : public SdpAttribute
{
public:
  SdpSctpmapAttributeList() :
    SdpAttribute(kSctpmapAttribute) {}

  struct Sctpmap {
    uint32_t number;
    std::string app;
    uint32_t maxMessageSize;
    uint32_t streams;
  };

  void PushEntry(uint32_t number,
                 std::string app,
                 uint32_t maxMessageSize = 0,
                 uint32_t streams = 0) {
    mSctpmaps.push_back({number, app, maxMessageSize, streams});
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Sctpmap> mSctpmaps;
};

///////////////////////////////////////////////////////////////////////////
// a=setup, RFC4145
//-------------------------------------------------------------------------
//       setup-attr           =  "a=setup:" role
//       role                 =  "active" / "passive" / "actpass" / "holdconn"
class SdpSetupAttribute : public SdpAttribute
{
public:
  enum Role {
    kActive,
    kPassive,
    kActpass,
    kHoldconn
  };

  SdpSetupAttribute(Role role) :
    SdpAttribute(kSetupAttribute),
    mRole(role) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  Role mRole;
};

inline std::ostream& operator <<(std::ostream& os, SdpSetupAttribute::Role r)
{
  switch (r) {
    case SdpSetupAttribute::kActive: os << "active"; break;
    case SdpSetupAttribute::kPassive: os << "passive"; break;
    case SdpSetupAttribute::kActpass: os << "actpass"; break;
    case SdpSetupAttribute::kHoldconn: os << "holdconn"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

///////////////////////////////////////////////////////////////////////////
// a=ssrc, RFC5576
//-------------------------------------------------------------------------
// ssrc-attr = "ssrc:" ssrc-id SP attribute
// ; The base definition of "attribute" is in RFC 4566.
// ; (It is the content of "a=" lines.)
//
// ssrc-id = integer ; 0 .. 2**32 - 1
//-------------------------------------------------------------------------
// TODO -- In the future, it might be nice if we ran a parse on the
// attribute section of this so that we could interpret it semantically.
// For WebRTC, the key use case for a=ssrc is assocaiting SSRCs with
// media sections, and we're not really going to care about the attribute
// itself. So we're just going to store it as a string for the time being.
class SdpSsrcAttributeList : public SdpAttribute
{
public:
  SdpSsrcAttributeList() :
    SdpAttribute(kSsrcAttribute) {}

  struct Ssrc {
    uint32_t ssrc;
    std::string attribute;
  };

  void PushEntry(uint32_t ssrc, const std::string &attribute) {
    mSsrcs.push_back({ssrc, attribute});
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<Ssrc> mSsrcs;
};

///////////////////////////////////////////////////////////////////////////
// a=ssrc-group, RFC5576
//-------------------------------------------------------------------------
// ssrc-group-attr = "ssrc-group:" semantics *(SP ssrc-id)
//
// semantics       = "FEC" / "FID" / token
//
// ssrc-id = integer ; 0 .. 2**32 - 1
class SdpSsrcGroupAttributeList : public SdpAttribute
{
public:
  enum Semantics {
    kFec,
    kFid,
    kFecFr,
    kDup
  };

  struct SsrcGroup {
    Semantics semantics;
    std::vector<uint32_t> ssrcs;
  };

  SdpSsrcGroupAttributeList() :
    SdpAttribute(kSsrcGroupAttribute) {}

  void PushEntry(Semantics semantics, const std::vector<uint32_t>& ssrcs) {
    mSsrcGroups.push_back({semantics, ssrcs});
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

  std::vector<SsrcGroup> mSsrcGroups;
};

inline std::ostream& operator <<(std::ostream& os,
                                 SdpSsrcGroupAttributeList::Semantics s)
{
  switch (s) {
    case SdpSsrcGroupAttributeList::kFec: os << "FEC"; break;
    case SdpSsrcGroupAttributeList::kFid: os << "FID"; break;
    case SdpSsrcGroupAttributeList::kFecFr: os << "FEC-FR"; break;
    case SdpSsrcGroupAttributeList::kDup: os << "DUP"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

///////////////////////////////////////////////////////////////////////////
class SdpMultiStringAttribute : public SdpAttribute
{
public:
  explicit SdpMultiStringAttribute(AttributeType type) :
      SdpAttribute(type) {}

  void PushEntry(const std::string& entry) {
    mValues.push_back(entry);
  }

  virtual void Serialize(std::ostream& os) const;

  std::vector<std::string> mValues;
};


// Used for any other kind of attribute not otherwise specialized
class SdpFlagAttribute : public SdpAttribute
{
public:
  explicit SdpFlagAttribute(AttributeType type) :
    SdpAttribute(type) {}

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;
};

// Used for any other kind of attribute not otherwise specialized
class SdpStringAttribute : public SdpAttribute
{
public:
  explicit SdpStringAttribute(AttributeType type,
                              const std::string& value) :
    SdpAttribute(type), mValue(value) {}

  const std::string& GetValue() const {
    return mValue;
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

private:
  std::string mValue;
};

// Used for any purely numeric attribute
class SdpNumberAttribute : public SdpAttribute
{
public:
  explicit SdpNumberAttribute(AttributeType type, uint32_t value = 0) :
    SdpAttribute(type),
    mValue(value) {}

  uint32_t GetValue() const {
    return mValue;
  }

  virtual void Serialize(std::ostream& os) const MOZ_OVERRIDE;

private:
  uint32_t mValue;
};

}

#endif
