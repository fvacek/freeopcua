/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Node ID.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/expanded_object_ids.h>
#include <opc/ua/protocol/guid.h>
#include <opc/ua/protocol/reference_ids.h>

#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace OpcUa
{

  enum NodeIDEncoding : uint8_t
  {
    EV_TWO_BYTE = 0,
    EV_FOUR_BYTE = 1,
    EV_NUMERIC = 2,
    EV_STRING = 3,
    EV_GUID = 4,
    EV_BYTE_STRING = 5,

    EV_SERVER_INDEX_FLAG = 0x40,
    EV_NAMESPACE_URI_FLAG = 0x80,

    EV_VALUE_MASK = 0x3f,
  };

  struct ExpandedNodeID;

  struct NodeID
  {
    NodeIDEncoding Encoding;
    std::string NamespaceURI;
    uint32_t ServerIndex;

    union DataType
    {
      struct TwoByteDataType
      {
        uint8_t Identifier;

        TwoByteDataType()
          : Identifier(0)
        {
        }

      } TwoByteData;

      struct FourByteDataType
      {
        uint8_t NamespaceIndex;
        uint16_t Identifier;

        FourByteDataType()
          : NamespaceIndex(0)
          , Identifier(0)
        {
        }
      }FourByteData;

      struct NumericDataType
      {
        uint16_t NamespaceIndex;
        uint32_t Identifier;

        NumericDataType()
          : NamespaceIndex(0)
          , Identifier(0)
        {
        }
      }NumericData;


      struct StringDataType
      {
        uint16_t NamespaceIndex;
        std::string Identifier;

        StringDataType()
          : NamespaceIndex(0)
        {
        }

      }StringData;

      struct BinaryDataType
      {
        uint16_t NamespaceIndex;
        std::vector<uint8_t> Identifier;

        BinaryDataType()
          : NamespaceIndex(0)
        {
        }

      }BinaryData;

      struct GuidDataType
      {
        uint16_t NamespaceIndex;
        Guid Identifier;

        GuidDataType()
          : NamespaceIndex(0)
        {
        }

      }GuidData;

      DataType() {}
      DataType(const std::string &stringId, uint16_t index);
      DataType(const std::vector<uint8_t> &identifier, uint16_t index);
      ~DataType() {}

    } Data;

    NodeID();
    NodeID(const NodeID& node);
    NodeID(const ExpandedNodeID& node);
    NodeID(MessageID messageID);
    NodeID(ReferenceID referenceID);
    NodeID(ObjectID objectID);
    NodeID(ExpandedObjectID objectID);
    NodeID(uint32_t integerId, uint16_t index);
    NodeID(std::string stringId, uint16_t index);
    ~NodeID();

    NodeID& operator= (const NodeID& node);
    NodeID& operator= (const ExpandedNodeID& node);
    
    explicit operator ExpandedNodeID(); 

    NodeID& operator= (MessageID messageID)
    {
      *this = NodeID(messageID);
      return *this;
    }

    NodeID& operator= (ReferenceID referenceID)
    {
      *this = NodeID(referenceID);
      return *this;
    }

    NodeID& operator= (ObjectID objectID)
    {
      *this = NodeID(objectID);
      return *this;
    }

    NodeID& operator= (ExpandedObjectID objectID)
    {
      *this = NodeID(objectID);
      return *this;
    }

    bool operator== (const NodeID& node) const;
    bool operator== (MessageID messageID) const;
    bool operator== (ReferenceID referenceID) const;
    bool operator== (ObjectID objectID) const;
    bool operator== (ExpandedObjectID objectID) const;

    bool operator!= (const NodeID& node) const;
    bool operator!= (MessageID messageID) const;
    bool operator!= (ReferenceID referenceID) const;
    bool operator!= (ObjectID objectID) const;
    bool operator!= (ExpandedObjectID objectID) const;

    bool operator< (const NodeID& node) const;

    NodeIDEncoding GetEncodingValue() const;
    bool HasNamespaceURI() const;
    bool HasServerIndex() const;

    void SetNamespaceURI(const std::string& uri);
    void SetServerIndex(uint32_t index);
    void SetNamespaceIndex(uint32_t ns);

    bool IsInteger() const;
    bool IsString() const;
    bool IsBinary() const;
    bool IsGuid() const;

    uint32_t GetNamespaceIndex() const;

    uint32_t GetIntegerIdentifier() const;
    std::string GetStringIdentifier() const;
    std::vector<uint8_t> GetBinaryIdentifier() const;
    Guid GetGuidIdentifier() const;

    protected:
    void CopyNodeID(const NodeID& node);
  };

  inline NodeID TwoByteNodeID(uint8_t value)
  {
    NodeID id;
    id.Encoding = EV_TWO_BYTE;
    id.Data.TwoByteData.Identifier = value;
    return id;
  }

  inline NodeID FourByteNodeID(uint16_t value, uint8_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_FOUR_BYTE;
    id.Data.FourByteData.Identifier = value;
    id.Data.FourByteData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID NumericNodeID(uint32_t value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_NUMERIC;
    id.Data.NumericData.Identifier = value;
    id.Data.NumericData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID StringNodeID(std::string value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_STRING;
    id.Data.StringData.Identifier = value;
    id.Data.StringData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID BinaryNodeID(std::vector<uint8_t> value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_BYTE_STRING;
    id.Data.BinaryData.Identifier = value;
    id.Data.BinaryData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID GuidNodeID(Guid value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_GUID;
    id.Data.GuidData.Identifier = value;
    id.Data.GuidData.NamespaceIndex = namespaceIndex;
    return id;
  }

  struct ExpandedNodeID : public NodeID
  {
    ExpandedNodeID();
    ExpandedNodeID(const NodeID& node);
    ExpandedNodeID(const ExpandedNodeID& node);
    ExpandedNodeID(MessageID messageID);
    ExpandedNodeID(ReferenceID referenceID);
    ExpandedNodeID(ObjectID objectID);
    ExpandedNodeID(ExpandedObjectID objectID);
    ExpandedNodeID(uint32_t integerId, uint16_t index);
    ExpandedNodeID(std::string stringId, uint16_t index);

   //using NodeID::NodeID;
   //using base::base;
  };

} // namespace OpcUa


