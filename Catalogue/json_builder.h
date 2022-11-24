#pragma once

#include "json.h"

namespace json {

class Builder;
class Interface;
class KeyItemCorrect;
class ValueItemCorrect;
class ArrayItemCorrect;

class Interface {
 public:
  Interface() = default;
  Interface(Builder& builder) : builder_(builder) {}
  KeyItemCorrect Key(std::string key);
  Interface Value(const Node::Value value);
  ValueItemCorrect StartDict();
  ArrayItemCorrect StartArray();
  Interface EndDict();
  Interface EndArray();
  Node Build();

  Builder& GetBuilder() { return builder_; };

 protected:
  Builder& builder_;
};

class KeyItemCorrect : public Interface {
 public:
  KeyItemCorrect(Builder& builder) : Interface(builder) {}
  ValueItemCorrect Value(const Node::Value value);
  ValueItemCorrect StartDict();
  Interface StartArray();

  KeyItemCorrect Key(std::string key) = delete;
  Interface EndDict() = delete;
  Interface EndArray() = delete;
  Node Build() = delete;
};

class ValueItemCorrect : public Interface {
 public:
  ValueItemCorrect(Builder& builder) : Interface(builder) {}
  KeyItemCorrect Key(std::string key);
  Interface EndDict();

  Interface Value(const Node::Value value) = delete;
  ValueItemCorrect StartDict() = delete;
  ArrayItemCorrect StartArray() = delete;
  Interface EndArray() = delete;
  Node Build() = delete;
};

class ArrayItemCorrect : public Interface {
 public:
  ArrayItemCorrect(Builder& builder) : Interface(builder) {}
  ArrayItemCorrect Value(const Node::Value value);
  ValueItemCorrect StartDict();
  ArrayItemCorrect StartArray();
  Interface EndArray();

  KeyItemCorrect Key(std::string key) = delete;
  Interface EndDict() = delete;
  Node Build() = delete;
};

class Builder {
  typedef enum {
    IN_NULL,
    IN_ARRAY,
    IN_DICT,
    IN_BOOL,
    IN_INT,
    IN_DBL,
    IN_STR,
    ERROR,
    EMPTY
  } IndexVariant;
  typedef enum {
    CREATE,
    KEY,
    VALUE,
    START_DICT,
    START_ARRAY,
    END_DICT,
    END_ARRAY,
    BUILD
  } Requests;

  typedef struct {
    bool start_create_root = false;
    bool finish_create_root = false;
    IndexVariant type_root = EMPTY;
    std::string save_key_;
    bool status_key = false;
    int index_add = -1;
  } StatusBuild;

 public:
  KeyItemCorrect Key(std::string key);
  Interface Value(const Node::Value value);
  ValueItemCorrect StartDict();
  ArrayItemCorrect StartArray();
  Interface EndDict();
  Interface EndArray();
  Node Build();

 private:
  Node root_;

  std::vector<IndexVariant> for_check_tipe_form_;
  std::vector<Node> nods_form_chield_;
  std::vector<StatusBuild> status_build_chield;
  int index_chield = -1;
  StatusBuild status_root_;

  bool create_arroy_chield_ = false;
  bool create_dict_chield_ = false;

  std::list<Requests> history_ = {CREATE};

  void PrintError(std::string text_error);
  bool CheckBuildRoot();

  Node CheckIndexVariant(const Node::Value value);

  Node ValueNull(const Node::Value value);
  Node ValueArray(const Node::Value value);
  Node ValueDict(const Node::Value value);
  Node ValueBool(const Node::Value value);
  Node ValueInt(const Node::Value value);
  Node ValueDbl(const Node::Value value);
  Node ValueString(const Node::Value value);

  void EraceChield();

  bool CheckErrorStartDict();
  bool CheckErrorStartArray();
  bool CheckErrorEndDict();
  bool CheckErrorEndArray();

  // ---------- Value ----------
  bool CheckErrorValue();
  void InsertKeyAndValue(const Node::Value& value, Dict& src, std::string& key);
  // ---------- Value ----------
  // ---------- Key ----------
  bool CheckErrorKey(std::string& key);
  // ---------- Key ----------
};

}  // namespace json