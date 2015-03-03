/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Node ID.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/nodeid.h>

#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace OpcUa
{


  NodeID::NodeID(uint32_t integerId, uint16_t index)
  {
    Encoding = EV_NUMERIC;
    Data.NumericData.Identifier = integerId;
    Data.NumericData.NamespaceIndex = index;
  }

  NodeID::NodeID(std::string stringId, uint16_t index)
    : Encoding(EV_STRING), Data(stringId, index)
  {
  }

  NodeID::~NodeID()
  {
    if(Encoding & EV_STRING) {
      Data.StringData.Identifier.~basic_string();
    }
    else if(Encoding & EV_BYTE_STRING) {
      Data.BinaryData.Identifier.~vector();
    }
  }

  bool NodeID::IsInteger() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_TWO_BYTE || enc == EV_FOUR_BYTE || enc == EV_NUMERIC;
  }

  bool NodeID::IsString() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_STRING;
  }

  bool NodeID::IsBinary() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_BYTE_STRING;
  }

  bool NodeID::IsGuid() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_GUID;
  }

  std::string NodeID::GetStringIdentifier() const
  {
    if (IsString())
    {
      return Data.StringData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  std::vector<uint8_t> NodeID::GetBinaryIdentifier() const
  {
    if (IsBinary())
    {
      return Data.BinaryData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  Guid NodeID::GetGuidIdentifier() const
  {
    if (IsGuid())
    {
      return Data.GuidData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  uint32_t NodeID::GetIntegerIdentifier() const
  {
    switch (GetEncodingValue())
    {
      case EV_TWO_BYTE:
      {
        return Data.TwoByteData.Identifier;
      }
      case EV_FOUR_BYTE:
      {
        return Data.FourByteData.Identifier;
      }
      case EV_NUMERIC:
      {
        return Data.NumericData.Identifier;
      }
      default:
      {
        throw std::logic_error("Cannot get integer identifier from NodeID - it is not in numeric format.");
      }
    }
  }

  uint32_t NodeID::GetNamespaceIndex() const
  {
    switch (GetEncodingValue())
    {
      case EV_FOUR_BYTE:
        return Data.FourByteData.NamespaceIndex;
      case EV_NUMERIC:
        return Data.NumericData.NamespaceIndex;
      case EV_STRING:
        return Data.StringData.NamespaceIndex;
      case EV_GUID:
        return Data.GuidData.NamespaceIndex;
      case EV_BYTE_STRING:
        return Data.BinaryData.NamespaceIndex;
      default:
        return 0;
    }
  }

  void NodeID::SetNamespaceIndex(uint32_t ns)
  {
    switch (GetEncodingValue())
    {
      case EV_FOUR_BYTE:
        Data.FourByteData.NamespaceIndex = ns;
        return;
      case EV_NUMERIC:
        Data.NumericData.NamespaceIndex = ns;
        return;
      case EV_STRING:
        Data.StringData.NamespaceIndex = ns;
        return;
      case EV_GUID:
        Data.GuidData.NamespaceIndex = ns;
        return;
      case EV_BYTE_STRING:
        Data.BinaryData.NamespaceIndex = ns;
        return;
      default:
        return;
    }
  }

  NodeID::NodeID()
    : Encoding(EV_TWO_BYTE)
    , ServerIndex(0)
  {
  }

  void NodeID::CopyNodeID(const NodeID& node)
  {
    Encoding = node.Encoding;
    const NodeIDEncoding enc = node.GetEncodingValue();
    switch (enc)
    {
      case EV_TWO_BYTE:
      {
        Data.TwoByteData.Identifier = node.Data.TwoByteData.Identifier;
        break;
      }
      case EV_FOUR_BYTE:
      {
        Data.FourByteData.NamespaceIndex = node.Data.FourByteData.NamespaceIndex;
        Data.FourByteData.Identifier = node.Data.FourByteData.Identifier;
        break;
      }
      case EV_NUMERIC:
      {
        Data.NumericData.NamespaceIndex = node.Data.NumericData.NamespaceIndex;
        Data.NumericData.Identifier = node.Data.NumericData.Identifier;
        break;
      }
      case EV_STRING:
      {
        Data.StringData.NamespaceIndex = node.Data.StringData.NamespaceIndex;
        Data.StringData.Identifier = node.Data.StringData.Identifier;
        break;
      }
      case EV_GUID:
      {
        Data.GuidData.NamespaceIndex = node.Data.GuidData.NamespaceIndex;
        Data.GuidData.Identifier = node.Data.GuidData.Identifier;
        break;
      }
      case EV_BYTE_STRING:
      {
        Data.BinaryData.NamespaceIndex = node.Data.BinaryData.NamespaceIndex;
        Data.BinaryData.Identifier = node.Data.BinaryData.Identifier;
        break;
      }
      default:
      {
        throw std::logic_error("Invalid Node ID encoding value.");
      }
    }

    if (node.HasServerIndex())
    {
      ServerIndex = node.ServerIndex;
    }
    if (node.HasNamespaceURI())
    {
      NamespaceURI = node.NamespaceURI;
    }

  }

  NodeID::NodeID(const NodeID& node)
  {
    CopyNodeID(node);
  }

  NodeID::NodeID(const ExpandedNodeID& node)
  {
    CopyNodeID(node);
  }

  NodeID::operator ExpandedNodeID()
  {
    ExpandedNodeID node;
    node.CopyNodeID(*this);

    return node;
  } 

  NodeID& NodeID::operator=(const NodeID& node)
  {
    CopyNodeID(node);
    return *this;
  }

  NodeID& NodeID::operator=(const ExpandedNodeID& node)
  {
    CopyNodeID(node);
    return *this;
  }

  NodeID::NodeID(MessageID messageID)
    : Encoding(EV_FOUR_BYTE)
    , ServerIndex(0)
  {
    Data.FourByteData.Identifier = messageID;
  }

  NodeID::NodeID(ReferenceID referenceID)
    : Encoding(EV_NUMERIC)
    , ServerIndex(0)
  {
    Data.NumericData.Identifier = static_cast<uint32_t>(referenceID);
  }

  NodeID::NodeID(ObjectID objectID)
    : Encoding(EV_NUMERIC)
    , ServerIndex(0)
  {
    Data.NumericData.Identifier = static_cast<uint32_t>(objectID);
  }

  NodeID::NodeID(ExpandedObjectID objectID)
    : Encoding(EV_FOUR_BYTE)
    , ServerIndex(0)
  {
    Data.FourByteData.Identifier = static_cast<uint32_t>(objectID);
  }

  MessageID GetMessageID(const NodeID& id)
  {
    return static_cast<MessageID>(id.GetIntegerIdentifier());
  }


  bool NodeID::operator== (const NodeID& node) const
  {
    if (GetNamespaceIndex() != node.GetNamespaceIndex())
    {
      return false;
    }
    if (IsInteger() && node.IsInteger())
    {
      return GetIntegerIdentifier() == node.GetIntegerIdentifier();
    }
    if (IsString() && node.IsString())
    {
      return GetStringIdentifier() == node.GetStringIdentifier();
    }
    if (IsBinary() && node.IsBinary())
    {
      return GetBinaryIdentifier() == node.GetBinaryIdentifier();
    }
    if (IsGuid() && node.IsGuid())
    {
      return GetGuidIdentifier() == node.GetGuidIdentifier();
    }
    return false;
  }

  bool NodeID::operator < (const NodeID& node) const
  {
    if (GetNamespaceIndex() != node.GetNamespaceIndex())
    {
      return GetNamespaceIndex() < node.GetNamespaceIndex();
    }
    if (IsInteger() && node.IsInteger())
    {
      return GetIntegerIdentifier() < node.GetIntegerIdentifier();
    }
    if (IsString() && node.IsString())
    {
      return GetStringIdentifier() < node.GetStringIdentifier();
    }
    if (IsBinary() && node.IsBinary())
    {
      const std::vector<uint8_t>& l = GetBinaryIdentifier();
      const std::vector<uint8_t>& r = node.GetBinaryIdentifier();
      return std::lexicographical_compare(l.cbegin(), l.cend(), r.cbegin(), r.cend());
    }
    if (IsGuid() && node.IsGuid())
    {
      return GetGuidIdentifier() < node.GetGuidIdentifier();
    }
    return Encoding < node.Encoding; //FIXME Can we get there? and should we?

  }

  NodeIDEncoding NodeID::GetEncodingValue() const
  {
    return static_cast<NodeIDEncoding>(Encoding & EV_VALUE_MASK);
  }

  bool NodeID::HasNamespaceURI() const
  {
    return (Encoding & EV_NAMESPACE_URI_FLAG) != 0;
  }

  bool NodeID::HasServerIndex() const
  {
    return (Encoding & EV_SERVER_INDEX_FLAG) != 0;
  }

  void NodeID::SetNamespaceURI(const std::string& uri)
  {
    Encoding = static_cast<NodeIDEncoding>(Encoding | EV_NAMESPACE_URI_FLAG);
    NamespaceURI = uri;
  }

  void NodeID::SetServerIndex(uint32_t index)
  {
    Encoding = static_cast<NodeIDEncoding>(Encoding | EV_SERVER_INDEX_FLAG);
    ServerIndex = index;
  }

  bool NodeID::operator!= (const NodeID& node) const
  {
    return !(*this == node);
  }

  bool NodeID::operator!= (MessageID messageID) const
  {
    return !(*this == messageID);
  }

  bool NodeID::operator!= (ReferenceID referenceID) const
  {
    return !(*this == referenceID);
  }

  bool NodeID::operator!= (ObjectID objectID) const
  {
    return !(*this == objectID);
  }

  bool NodeID::operator!= (ExpandedObjectID objectID) const
  {
    return !(*this == objectID);
  }



  bool NodeID::operator== (MessageID messageID) const
  {
    return *this == NodeID(messageID);
  }

  bool NodeID::operator== (ReferenceID referenceID) const
  {
    return *this == NodeID(referenceID);
  }

  bool NodeID::operator== (ObjectID messageID) const
  {
    return *this == NodeID(messageID);
  }

  bool NodeID::operator== (ExpandedObjectID messageID) const
  {
    return *this == NodeID(messageID);
  }


  ///ExpandednNdeID
  ExpandedNodeID::ExpandedNodeID()
  {
    Encoding = EV_TWO_BYTE;
    ServerIndex = 0;
  }

  ExpandedNodeID::ExpandedNodeID(uint32_t integerId, uint16_t index)
  {
    Encoding = EV_NUMERIC;
    Data.NumericData.Identifier = integerId;
    Data.NumericData.NamespaceIndex = index;
  }

  ExpandedNodeID::ExpandedNodeID(std::string stringId, uint16_t index)
  {
    Encoding = EV_STRING;
    Data.StringData.Identifier = stringId;
    Data.StringData.NamespaceIndex = index;
  }

  ExpandedNodeID::ExpandedNodeID(const NodeID& node)
  {
    CopyNodeID(node);
  }

  ExpandedNodeID::ExpandedNodeID(const ExpandedNodeID& node)
  {
    CopyNodeID(node);
  }


  ExpandedNodeID::ExpandedNodeID(MessageID messageID)
  {
    Encoding = EV_FOUR_BYTE;
    ServerIndex = 0;
    Data.FourByteData.Identifier = messageID;
  }

  ExpandedNodeID::ExpandedNodeID(ReferenceID referenceID)
  {
    Encoding = EV_NUMERIC;
    ServerIndex = 0;
    Data.NumericData.Identifier = static_cast<uint32_t>(referenceID);
  }

  ExpandedNodeID::ExpandedNodeID(ObjectID objectID)
  {
    Encoding = EV_NUMERIC;
    ServerIndex = 0;
    Data.NumericData.Identifier = static_cast<uint32_t>(objectID);
  }

  ExpandedNodeID::ExpandedNodeID(ExpandedObjectID objectID)
  {
    Encoding = EV_FOUR_BYTE;
    ServerIndex = 0;
    Data.FourByteData.Identifier = static_cast<uint32_t>(objectID);
  }


  namespace Binary
  {
    template<>
    std::size_t RawSize<NodeIDEncoding>(const NodeIDEncoding&)
    {
      return 1;
    }

    template<>
    void DataSerializer::Serialize<NodeIDEncoding>(const NodeIDEncoding& value)
    {
      *this << static_cast<uint8_t>(value);
    }

    template<>
    void DataDeserializer::Deserialize<NodeIDEncoding>(NodeIDEncoding& value)
    {
      uint8_t tmp = 0;
      *this >> tmp;
      value = static_cast<NodeIDEncoding>(tmp);
    }

    template<>
    std::size_t RawSize<NodeID>(const NodeID& id)
    {
      std::size_t size = 0;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          size = 2;
          break;
        }
        case EV_FOUR_BYTE:
        {
          size = 4;
          break;
        }
        case EV_NUMERIC:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofNamespace = 2;
          const std::size_t sizeofIdentifier = 4;
          size = sizeofEncoding + sizeofNamespace + sizeofIdentifier;
          break;
        }
        case EV_STRING:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofSize = 4;
          const std::size_t sizeofNamespace = 2;
          size = sizeofEncoding + sizeofNamespace + sizeofSize + id.Data.StringData.Identifier.size();
          break;
        }
        case EV_BYTE_STRING:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofSize = 4;
          const std::size_t sizeofNamespace = 2;
          size = sizeofEncoding + sizeofNamespace + sizeofSize + id.Data.BinaryData.Identifier.size();
          break;
        }
        case EV_GUID:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofNamespace = 2;
          const std::size_t sizeofGuid = 16;
          size = sizeofEncoding + sizeofNamespace + sizeofGuid;
          break;
        }

      default:
        throw std::logic_error("Unable serialize NodeID. Unknown encoding type.");
      };

      return size;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::NodeID>(const OpcUa::NodeID& id)
    {
      //unset server and namespace flags in encoding, they should only be used in ExpandedNode ID
      uint8_t nodeid_encoding = id.Encoding;
      nodeid_encoding &= ~EV_SERVER_INDEX_FLAG;
      nodeid_encoding &= ~EV_NAMESPACE_URI_FLAG;

      *this << nodeid_encoding;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          *this << id.Data.TwoByteData.Identifier;
          break;
        }
        case EV_FOUR_BYTE:
        {
          *this << id.Data.FourByteData.NamespaceIndex;
          *this << id.Data.FourByteData.Identifier;
          break;
        }
        case EV_NUMERIC:
        {
          *this << id.Data.NumericData.NamespaceIndex;
          *this << id.Data.NumericData.Identifier;
          break;
        }
        case EV_STRING:
        {
          *this << id.Data.StringData.NamespaceIndex;
          *this << id.Data.StringData.Identifier;
          break;
        }
        case EV_BYTE_STRING:
        {
          *this << id.Data.BinaryData.NamespaceIndex;
          *this << id.Data.BinaryData.Identifier;
          break;
        }
        case EV_GUID:
        {
          *this << id.Data.GuidData.NamespaceIndex;
          *this << id.Data.GuidData.Identifier;
          break;
        }

      default:
        throw std::logic_error("Unable serialize NodeID. Unknown encoding type.");
      };

    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::NodeID>(OpcUa::NodeID& id)
    {
      *this >> id.Encoding;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          *this >> id.Data.TwoByteData.Identifier;
          break;
        }
        case EV_FOUR_BYTE:
        {
          *this >> id.Data.FourByteData.NamespaceIndex;
          *this >> id.Data.FourByteData.Identifier;
          break;
        }
        case EV_NUMERIC:
        {
          *this >> id.Data.NumericData.NamespaceIndex;
          *this >> id.Data.NumericData.Identifier;
          break;
        }
        case EV_STRING:
        {
          *this >> id.Data.StringData.NamespaceIndex;
          *this >> id.Data.StringData.Identifier;
          break;
        }
        case EV_BYTE_STRING:
        {
          *this >> id.Data.BinaryData.NamespaceIndex;
          *this >> id.Data.BinaryData.Identifier;
          break;
        }
        case EV_GUID:
        {
          *this >> id.Data.GuidData.NamespaceIndex;
          *this >> id.Data.GuidData.Identifier;
          break;
        }

        default:
        {
          throw std::logic_error("Unable to deserialize NodeId. Unknown encoding type received.");
        }
      };

      if (id.HasNamespaceURI())
      {
        *this >> id.NamespaceURI;
      }
      if (id.HasServerIndex())
      {
        *this >> id.ServerIndex;
      }
    };

    template<>
    std::size_t RawSize<ExpandedNodeID>(const ExpandedNodeID& id)
    {
      std::size_t size = RawSize((NodeID)id);

      if (id.HasNamespaceURI())
      {
        const std::size_t sizeofSize = 4;
        size += sizeofSize + id.NamespaceURI.size();
      }
      if (id.HasServerIndex())
      {
        const std::size_t sizeofServerIndex = 4;
        size += sizeofServerIndex;
      }
      return size;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::ExpandedNodeID>(const OpcUa::ExpandedNodeID& id)
    {
      *this << id.Encoding;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          *this << id.Data.TwoByteData.Identifier;
          break;
        }
        case EV_FOUR_BYTE:
        {
          *this << id.Data.FourByteData.NamespaceIndex;
          *this << id.Data.FourByteData.Identifier;
          break;
        }
        case EV_NUMERIC:
        {
          *this << id.Data.NumericData.NamespaceIndex;
          *this << id.Data.NumericData.Identifier;
          break;
        }
        case EV_STRING:
        {
          *this << id.Data.StringData.NamespaceIndex;
          *this << id.Data.StringData.Identifier;
          break;
        }
        case EV_BYTE_STRING:
        {
          *this << id.Data.BinaryData.NamespaceIndex;
          *this << id.Data.BinaryData.Identifier;
          break;
        }
        case EV_GUID:
        {
          *this << id.Data.GuidData.NamespaceIndex;
          *this << id.Data.GuidData.Identifier;
          break;
        }

      default:
        throw std::logic_error("Unable serialize ExpandedNodeID. Unknown encoding type.");
      };

      if (id.HasNamespaceURI())
      {
        *this << id.NamespaceURI;
      }
      if (id.HasServerIndex())
      {
        *this << id.ServerIndex;
      }
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::ExpandedNodeID>(OpcUa::ExpandedNodeID& id)
    {
      *this >> *(NodeID*) &id;
    };


  }

  NodeID::DataType::DataType(const std::string &stringId, uint16_t index)
  {
    StringData.NamespaceIndex = index;
    StringData.Identifier = stringId;
  }

  NodeID::DataType::DataType(const std::vector<uint8_t> &identifier, uint16_t index)
  {
    BinaryData.NamespaceIndex = index;
    BinaryData.Identifier = identifier;
  }

  // namespace Binary
} // namespace OpcUa

