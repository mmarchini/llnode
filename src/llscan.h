#ifndef SRC_LLSCAN_H_
#define SRC_LLSCAN_H_

#include <lldb/API/LLDB.h>
#include <map>
#include <set>

namespace llnode {

enum ReferenceInfoType { rByIndex, rByAttribute, rStringByIndex, rStringByAttribute };

class ReferenceInfo {
  public:
    // For search by value or attribute
    ReferenceInfo(uint64_t address, std::string type_name, int64_t index, uint64_t referred_address)
      :address(address), type_name(type_name), index(index), reference_type(rByIndex), referred_address(referred_address)  {};
    ReferenceInfo(uint64_t address, std::string type_name, std::string attribute, uint64_t referred_address)
      :address(address), type_name(type_name), attribute(attribute), reference_type(rByAttribute), referred_address(referred_address)    {};

      // For search by string value
    ReferenceInfo(uint64_t address, std::string type_name, int64_t index, uint64_t referred_address, std::string string_value)
      :address(address), type_name(type_name), index(index), reference_type(rStringByIndex), referred_address(referred_address), string_value(string_value)  {};
    ReferenceInfo(uint64_t address, std::string type_name, std::string attribute, uint64_t referred_address, std::string string_value)
      :address(address), type_name(type_name), attribute(attribute), reference_type(rStringByAttribute), referred_address(referred_address), string_value(string_value)    {};
    ~ReferenceInfo() {}

    void PrintRef(lldb::SBCommandReturnObject& result);

  private:
    uint64_t address;
    std::string type_name;
    int64_t index;
    std::string attribute;
    ReferenceInfoType reference_type;

    uint64_t referred_address;
    std::string string_value;
};

typedef std::set<ReferenceInfo*> ReferenceInfoSet;

class ReferenceRecord {
  public:
    inline ReferenceInfoSet& GetInstances() { return references_; };

    inline void AddReference(uint64_t address, std::string type_name, int64_t index, uint64_t referred_address) {
      references_.insert(new ReferenceInfo(address, type_name, index, referred_address));
    };
    inline void AddReference(uint64_t address, std::string type_name, std::string attribute, uint64_t referred_address) {
      references_.insert(new ReferenceInfo(address, type_name, attribute, referred_address));
    };

    inline void AddReference(uint64_t address, std::string type_name, int64_t index, uint64_t referred_address, std::string string_value) {
      references_.insert(new ReferenceInfo(address, type_name, index, referred_address, string_value));
    };
    inline void AddReference(uint64_t address, std::string type_name, std::string attribute, uint64_t referred_address, std::string string_value) {
      references_.insert(new ReferenceInfo(address, type_name, attribute, referred_address, string_value));
    };

    inline ReferenceInfoSet *GetReferences() { return &references_; }

  private:
    ReferenceInfoSet references_;
};

typedef std::map<uint64_t, ReferenceRecord*> ReferenceByValueRecordMap;
typedef std::map<std::string, ReferenceRecord*> ReferenceByPropertyRecordMap;
typedef std::map<std::string, ReferenceRecord*> ReferenceByStringRecordMap;



class FindObjectsCmd : public CommandBase {
 public:
  ~FindObjectsCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;
};

class FindInstancesCmd : public CommandBase {
 public:
  ~FindInstancesCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;

 private:
  bool detailed_;
};

class NodeInfoCmd : public CommandBase {
 public:
  ~NodeInfoCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;
};

class FindReferencesCmd : public CommandBase {
 public:
  ~FindReferencesCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;

  enum ScanType { kFieldValue, kPropertyName, kStringValue, kBadOption };

  char** ParseScanOptions(char** cmd, ScanType* type);

  class ObjectScanner {
   public:
    virtual ~ObjectScanner() {}
    virtual void ScanForRefs(ReferenceRecord *record,
                           v8::JSObject& js_obj, v8::Error& err) {}
    virtual void ScanForRefs(ReferenceRecord *record, v8::String& str,
                           v8::Error& err) {}
    virtual ReferenceInfoSet* GetReferences() {}
    virtual void AddReferences(ReferenceRecord *record) {}
  };

  ReferenceRecord *ScanForReferences(ObjectScanner *scanner);

  class ReferenceScanner : public ObjectScanner {
   public:
    ReferenceScanner(v8::Value search_value) : search_value_(search_value) {}

    void ScanForRefs(ReferenceRecord *record, v8::JSObject& js_obj,
                   v8::Error& err) override;
    void ScanForRefs(ReferenceRecord *record, v8::String& str,
                   v8::Error& err) override;
    ReferenceInfoSet* GetReferences() override;
    void AddReferences(ReferenceRecord *record) override;

   private:
    v8::Value search_value_;
  };


  class PropertyScanner : public ObjectScanner {
   public:
    PropertyScanner(std::string search_value) : search_value_(search_value) {}

    // We only scan properties on objects not Strings, use default no-op impl
    // of ScanForRefs for Strings.
    void ScanForRefs(ReferenceRecord *record, v8::JSObject& js_obj,
                   v8::Error& err) override;
    ReferenceInfoSet* GetReferences() override;
    void AddReferences(ReferenceRecord *record) override;

   private:
    std::string search_value_;
  };


  class StringScanner : public ObjectScanner {
   public:
    StringScanner(std::string search_value) : search_value_(search_value) {}

    void ScanForRefs(ReferenceRecord *record, v8::JSObject& js_obj,
                   v8::Error& err) override;
    void ScanForRefs(ReferenceRecord *record, v8::String& str,
                   v8::Error& err) override;
    ReferenceInfoSet* GetReferences() override;
    void AddReferences(ReferenceRecord *record) override;

   private:
    std::string search_value_;
  };
};

class MemoryVisitor {
 public:
  virtual ~MemoryVisitor() {}

  virtual uint64_t Visit(uint64_t location, uint64_t available) = 0;
};

class TypeRecord {
 public:
  TypeRecord(std::string& type_name)
      : type_name_(type_name), instance_count_(0), total_instance_size_(0) {}

  inline std::string& GetTypeName() { return type_name_; };
  inline uint64_t GetInstanceCount() { return instance_count_; };
  inline uint64_t GetTotalInstanceSize() { return total_instance_size_; };
  inline std::set<uint64_t>& GetInstances() { return instances_; };

  inline void AddInstance(uint64_t address, uint64_t size) {
    instances_.insert(address);
    instance_count_++;
    total_instance_size_ += size;
  };

  /* Sort records by instance count, use the other fields as tie breakers
   * to give consistent ordering.
   */
  static bool CompareInstanceCounts(TypeRecord* a, TypeRecord* b) {
    if (a->instance_count_ == b->instance_count_) {
      if (a->total_instance_size_ == b->total_instance_size_) {
        return a->type_name_ < b->type_name_;
      }
      return a->total_instance_size_ < b->total_instance_size_;
    }
    return a->instance_count_ < b->instance_count_;
  }


 private:
  std::string type_name_;
  uint64_t instance_count_;
  uint64_t total_instance_size_;
  std::set<uint64_t> instances_;
};

typedef std::map<std::string, TypeRecord*> TypeRecordMap;

class FindJSObjectsVisitor : MemoryVisitor {
 public:
  FindJSObjectsVisitor(lldb::SBTarget& target, TypeRecordMap& mapstoinstances);
  ~FindJSObjectsVisitor() {}

  uint64_t Visit(uint64_t location, uint64_t word);

  uint32_t FoundCount() { return found_count_; }

 private:
  struct MapCacheEntry {
    std::string type_name;
    bool is_histogram;
  };

  bool IsAHistogramType(v8::Map& map, v8::Error& err);

  lldb::SBTarget& target_;
  uint32_t address_byte_size_;
  uint32_t found_count_;

  TypeRecordMap& mapstoinstances_;
  std::map<int64_t, MapCacheEntry> map_cache_;
};


class LLScan {
 public:
  LLScan() {}

  bool ScanHeapForObjects(lldb::SBTarget target,
                          lldb::SBCommandReturnObject& result);
  bool GenerateMemoryRanges(lldb::SBTarget target,
                            const char* segmentsfilename);


  inline TypeRecordMap& GetMapsToInstances() { return mapstoinstances_; };

  // References By Value
  inline void AddReferencesByValue(uint64_t address, ReferenceRecord* reference_record) {
    references_by_value_[address] = reference_record;
  }
  inline ReferenceRecord* GetReferencesByValue(uint64_t address) {
    ReferenceByValueRecordMap::iterator it = references_by_value_.find(address);
    if(it != references_by_value_.end())
    {
       return it->second;
    }
    return nullptr;
  };

  // References By Property
  inline void AddReferencesByProperty(std::string property, ReferenceRecord* reference_record) {
    references_by_property_[property] = reference_record;
  }
  inline ReferenceRecord* GetReferencesByProperty(std::string property) {
    ReferenceByPropertyRecordMap::iterator it = references_by_property_.find(property);
    if(it != references_by_property_.end())
    {
       return it->second;
    }
    return nullptr;
  };

  // References By String Value
  inline void AddReferencesByString(std::string string_value, ReferenceRecord* reference_record) {
    references_by_string_[string_value] = reference_record;
  }
  inline ReferenceRecord* GetReferencesByString(std::string string_value) {
    ReferenceByStringRecordMap::iterator it = references_by_string_.find(string_value);
    if(it != references_by_string_.end())
    {
       return it->second;
    }
    return nullptr;
  };

 private:
  void ScanMemoryRanges(FindJSObjectsVisitor& v);
  void ClearMemoryRanges();
  void ClearMapsToInstances();

  class MemoryRange {
   public:
    MemoryRange(uint64_t start, uint64_t length)
        : start_(start), length_(length), next_(nullptr) {}

    uint64_t start_;
    uint64_t length_;
    MemoryRange* next_;
  };

  lldb::SBTarget target_;
  lldb::SBProcess process_;
  MemoryRange* ranges_ = nullptr;
  TypeRecordMap mapstoinstances_;

  ReferenceByValueRecordMap references_by_value_;
  ReferenceByPropertyRecordMap references_by_property_;
  ReferenceByStringRecordMap references_by_string_;
};

}  // namespace llnode


#endif  // SRC_LLSCAN_H_
